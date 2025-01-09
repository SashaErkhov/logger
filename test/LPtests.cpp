#include "LogPostgresql.h"
#include <gtest/gtest.h>
#include <libpq-fe.h>
#include <fstream>
#include <exception>
#include <string>

PGconn* connection()
{
    std::ifstream file("test.json");
    std::string strForConnection;
    try{
        json j = json::parse(file);
        std::stringstream ss;
        ss<<"dbname="<<j["dbname"].get<std::string>()<<" user="
        <<j["user"].get<std::string>()<<"  password="<<j["password"].get<std::string>()
        <<" host="<<j["host"].get<std::string>()<<" port="<<j["port"].get<std::string>();
        strForConnection=ss.str();
    }catch (...)
    {
        std::cerr << "Can't parse config file " << std::endl;
        throw std::logic_error("Can't parse config file");
    }
    PGconn *conn = PQconnectdb(strForConnection.c_str());
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Can't connect to database: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        throw std::logic_error("Can't connect to database");
    }
    return conn;
}

inline void dropTableDB()
{
    PGconn *conn = connection();
    PGresult *res = PQexec(conn,"DROP TABLE IF EXISTS logs");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Error of dropping table: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        throw std::logic_error("Error of dropping table");
    }
    PQclear(res);
    PQfinish(conn);
}

TEST(LogPostgresql, constructorAndLog)
{
    dropTableDB();
    LPG::Logger logger("test.json");
    logger.debug("test-debug");
    logger.info("test-info");
    logger.notice("test-notice");
    logger.warning("test-warning");
    logger.error("test-error");
    logger.critical("test-critical");
    logger.alert("test-alert");
    logger.emergency("test-emergency");
    PGconn *conn = connection();
    PGresult *res = PQexec(conn,"SELECT * FROM logs");
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        std::cerr << "Error of selecting from table: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        throw std::logic_error("Error of selecting from table");
    }
    int nrows = PQntuples(res);
    EXPECT_EQ(nrows,8);

    std::string logLevel=PQgetvalue(res,0,1);
    EXPECT_EQ(logLevel,"DEBUG");
    logLevel=PQgetvalue(res,1,1);
    EXPECT_EQ(logLevel,"INFO");
    logLevel=PQgetvalue(res,2,1);
    EXPECT_EQ(logLevel,"NOTICE");
    logLevel=PQgetvalue(res,3,1);
    EXPECT_EQ(logLevel,"WARNING");
    logLevel=PQgetvalue(res,4,1);
    EXPECT_EQ(logLevel,"ERROR");
    logLevel=PQgetvalue(res,5,1);
    EXPECT_EQ(logLevel,"CRITICAL");
    logLevel=PQgetvalue(res,6,1);
    EXPECT_EQ(logLevel,"ALERT");
    logLevel=PQgetvalue(res,7,1);
    EXPECT_EQ(logLevel,"EMERGENCY");


    std::string message=PQgetvalue(res,0,2);
    EXPECT_EQ(message,"test-debug");
    message=PQgetvalue(res,1,2);
    EXPECT_EQ(message,"test-info");
    message=PQgetvalue(res,2,2);
    EXPECT_EQ(message,"test-notice");
    message=PQgetvalue(res,3,2);
    EXPECT_EQ(message,"test-warning");
    message=PQgetvalue(res,4,2);
    EXPECT_EQ(message,"test-error");
    message=PQgetvalue(res,5,2);
    EXPECT_EQ(message,"test-critical");
    message=PQgetvalue(res,6,2);
    EXPECT_EQ(message,"test-alert");
    message=PQgetvalue(res,7,2);
    EXPECT_EQ(message,"test-emergency");

    PQclear(res);
    PQfinish(conn);
}

TEST(LogPostgresql, constructorOfCopying)
{
    dropTableDB();
    LPG::Logger logger("test.json");
    LPG::Logger logger2(logger);
    logger.debug("test-debug");
    logger.info("test-info");
    logger.warning("test-warning");
    logger.error("test-error");
    logger2.debug("test-debug");
    logger2.info("test-info");
    logger2.warning("test-warning");
    logger2.error("test-error");
    PGconn *conn = connection();
    PGresult *res = PQexec(conn,"SELECT * FROM logs");
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        std::cerr << "Error of selecting from table: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        throw std::logic_error("Error of selecting from table");
    }
    int nrows = PQntuples(res);
    EXPECT_EQ(nrows,8);

    std::string logLevel=PQgetvalue(res,0,1);
    EXPECT_EQ(logLevel,"DEBUG");
    logLevel=PQgetvalue(res,1,1);
    EXPECT_EQ(logLevel,"INFO");
    logLevel=PQgetvalue(res,2,1);
    EXPECT_EQ(logLevel,"WARNING");
    logLevel=PQgetvalue(res,3,1);
    EXPECT_EQ(logLevel,"ERROR");
    logLevel=PQgetvalue(res,4,1);
    EXPECT_EQ(logLevel,"DEBUG");
    logLevel=PQgetvalue(res,5,1);
    EXPECT_EQ(logLevel,"INFO");
    logLevel=PQgetvalue(res,6,1);
    EXPECT_EQ(logLevel,"WARNING");
    logLevel=PQgetvalue(res,7,1);
    EXPECT_EQ(logLevel,"ERROR");

    std::string message=PQgetvalue(res,0,2);
    EXPECT_EQ(message,"test-debug");
    message=PQgetvalue(res,1,2);
    EXPECT_EQ(message,"test-info");
    message=PQgetvalue(res,2,2);
    EXPECT_EQ(message,"test-warning");
    message=PQgetvalue(res,3,2);
    EXPECT_EQ(message,"test-error");
    message=PQgetvalue(res,4,2);
    EXPECT_EQ(message,"test-debug");
    message=PQgetvalue(res,5,2);
    EXPECT_EQ(message,"test-info");
    message=PQgetvalue(res,6,2);
    EXPECT_EQ(message,"test-warning");
    message=PQgetvalue(res,7,2);
    EXPECT_EQ(message,"test-error");

    PQclear(res);
    PQfinish(conn);
}

TEST(LogPostgresql, eqOfCopying)
{
    dropTableDB();
    LPG::Logger logger("test.json");
    LPG::Logger logger2 = logger;
    logger.debug("test-debug");
    logger.info("test-info");
    logger.warning("test-warning");
    logger.error("test-error");
    logger2.debug("test-debug");
    logger2.info("test-info");
    logger2.warning("test-warning");
    logger2.error("test-error");
    PGconn *conn = connection();
    PGresult *res = PQexec(conn,"SELECT * FROM logs");
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        std::cerr << "Error of selecting from table: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        throw std::logic_error("Error of selecting from table");
    }
    int nrows = PQntuples(res);
    EXPECT_EQ(nrows,8);

    std::string logLevel=PQgetvalue(res,0,1);
    EXPECT_EQ(logLevel,"DEBUG");
    logLevel=PQgetvalue(res,1,1);
    EXPECT_EQ(logLevel,"INFO");
    logLevel=PQgetvalue(res,2,1);
    EXPECT_EQ(logLevel,"WARNING");
    logLevel=PQgetvalue(res,3,1);
    EXPECT_EQ(logLevel,"ERROR");
    logLevel=PQgetvalue(res,4,1);
    EXPECT_EQ(logLevel,"DEBUG");
    logLevel=PQgetvalue(res,5,1);
    EXPECT_EQ(logLevel,"INFO");
    logLevel=PQgetvalue(res,6,1);
    EXPECT_EQ(logLevel,"WARNING");
    logLevel=PQgetvalue(res,7,1);
    EXPECT_EQ(logLevel,"ERROR");

    std::string message=PQgetvalue(res,0,2);
    EXPECT_EQ(message,"test-debug");
    message=PQgetvalue(res,1,2);
    EXPECT_EQ(message,"test-info");
    message=PQgetvalue(res,2,2);
    EXPECT_EQ(message,"test-warning");
    message=PQgetvalue(res,3,2);
    EXPECT_EQ(message,"test-error");
    message=PQgetvalue(res,4,2);
    EXPECT_EQ(message,"test-debug");
    message=PQgetvalue(res,5,2);
    EXPECT_EQ(message,"test-info");
    message=PQgetvalue(res,6,2);
    EXPECT_EQ(message,"test-warning");
    message=PQgetvalue(res,7,2);
    EXPECT_EQ(message,"test-error");

    PQclear(res);
    PQfinish(conn);
}

TEST(LogPostgresql, constructorOfMoving)
{
    dropTableDB();
    LPG::Logger logger("test.json");
    LPG::Logger logger2(std::move(logger));
    logger.debug("test-debug");
    logger.info("test-info");
    logger.warning("test-warning");
    logger.error("test-error");
    logger2.debug("test-debug");
    logger2.info("test-info");
    logger2.warning("test-warning");
    logger2.error("test-error");
    PGconn *conn = connection();
    PGresult *res = PQexec(conn,"SELECT * FROM logs");
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        std::cerr << "Error of selecting from table: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        throw std::logic_error("Error of selecting from table");
    }
    int nrows = PQntuples(res);
    EXPECT_EQ(nrows,4);

    std::string logLevel=PQgetvalue(res,0,1);
    EXPECT_EQ(logLevel,"DEBUG");
    logLevel=PQgetvalue(res,1,1);
    EXPECT_EQ(logLevel,"INFO");
    logLevel=PQgetvalue(res,2,1);
    EXPECT_EQ(logLevel,"WARNING");
    logLevel=PQgetvalue(res,3,1);
    EXPECT_EQ(logLevel,"ERROR");

    std::string message=PQgetvalue(res,0,2);
    EXPECT_EQ(message,"test-debug");
    message=PQgetvalue(res,1,2);
    EXPECT_EQ(message,"test-info");
    message=PQgetvalue(res,2,2);
    EXPECT_EQ(message,"test-warning");
    message=PQgetvalue(res,3,2);
    EXPECT_EQ(message,"test-error");

    PQclear(res);
    PQfinish(conn);
}

TEST(LogPostgresql, eqOfMoving)
{
    dropTableDB();
    LPG::Logger logger("test.json");
    LPG::Logger logger2=std::move(logger);
    logger.debug("test-debug");
    logger.info("test-info");
    logger.warning("test-warning");
    logger.error("test-error");
    logger2.debug("test-debug");
    logger2.info("test-info");
    logger2.warning("test-warning");
    logger2.error("test-error");
    PGconn *conn = connection();
    PGresult *res = PQexec(conn,"SELECT * FROM logs");
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        std::cerr << "Error of selecting from table: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        throw std::logic_error("Error of selecting from table");
    }
    int nrows = PQntuples(res);
    EXPECT_EQ(nrows,4);

    std::string logLevel=PQgetvalue(res,0,1);
    EXPECT_EQ(logLevel,"DEBUG");
    logLevel=PQgetvalue(res,1,1);
    EXPECT_EQ(logLevel,"INFO");
    logLevel=PQgetvalue(res,2,1);
    EXPECT_EQ(logLevel,"WARNING");
    logLevel=PQgetvalue(res,3,1);
    EXPECT_EQ(logLevel,"ERROR");

    std::string message=PQgetvalue(res,0,2);
    EXPECT_EQ(message,"test-debug");
    message=PQgetvalue(res,1,2);
    EXPECT_EQ(message,"test-info");
    message=PQgetvalue(res,2,2);
    EXPECT_EQ(message,"test-warning");
    message=PQgetvalue(res,3,2);
    EXPECT_EQ(message,"test-error");

    PQclear(res);
    PQfinish(conn);
}

TEST(LogPostgresql, noExistFile)
{
    dropTableDB();
    LPG::Logger logger("noExistFile.txt");
    PGconn *conn = connection();
    PGresult *res =
            PQexec(conn, "CREATE TABLE IF NOT EXISTS Logs (dateTime TIMESTAMP DEFAULT now(), level LogLevel, message TEXT);");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr <<"Error of creating table: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        throw std::logic_error("Error of creating table");
    }
    PQclear(res);
    logger.debug("test-debug");
    logger.info("test-info");
    logger.warning("test-warning");
    logger.error("test-error");
    res = PQexec(conn,"SELECT * FROM logs");
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        std::cerr << "Error of selecting from table: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        throw std::logic_error("Error of selecting from table");
    }
    int nrows = PQntuples(res);
    EXPECT_EQ(nrows,0);
    PQclear(res);
    PQfinish(conn);
}