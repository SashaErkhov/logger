#include "LogPostgresql.h"
#include <gtest/gtest.h>
#include <libpq-fe.h>
#include <string>
#include <exception>
#include <toml++/toml.hpp>

PGconn* connection(const char* configFile)
{
    std::string strForConnection;
    try{
        auto config = toml::parse_file(configFile);
        std::stringstream ss;
        ss<<"dbname="<<config["database"]["dbname"].value_or("testdb")
        <<" user="<<config["database"]["user"].value_or("postgres")
        <<"  password="<<config["database"]["password"].value_or("postgres")
        <<" host="<<config["database"]["host"].value_or("localhost")
        <<" port="<<config["database"]["port"].value_or("228");
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

inline void dropTableDB(const char* configFile)
{
    PGconn *conn = connection(configFile);
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
    std::cout << 1 << std::endl;
    dropTableDB("LPtest_forConn.toml");
    std::cout << 2 << std::endl;
    LPG::Logger logger("LPtest_constructorAndLog.toml");
    std::cout << 3 << std::endl;
    logger.debug("test-debug");
    logger.info("test-info");
    logger.notice("test-notice");
    logger.warning("test-warning");
    logger.error("test-error");
    logger.critical("test-critical");
    logger.alert("test-alert");
    logger.emergency("test-emergency");
    std::cout << 4 << std::endl;
    PGconn *conn = connection("LPtest_forConn.toml");
    std::cout << 5 << std::endl;
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


    std::string message=PQgetvalue(res,0,3);
    EXPECT_EQ(message,"test-debug");
    message=PQgetvalue(res,1,3);
    EXPECT_EQ(message,"test-info");
    message=PQgetvalue(res,2,3);
    EXPECT_EQ(message,"test-notice");
    message=PQgetvalue(res,3,3);
    EXPECT_EQ(message,"test-warning");
    message=PQgetvalue(res,4,3);
    EXPECT_EQ(message,"test-error");
    message=PQgetvalue(res,5,3);
    EXPECT_EQ(message,"test-critical");
    message=PQgetvalue(res,6,3);
    EXPECT_EQ(message,"test-alert");
    message=PQgetvalue(res,7,3);
    EXPECT_EQ(message,"test-emergency");

    PQclear(res);
    PQfinish(conn);
}

TEST(LogPostgresql, constructorOfCopying)
{
    dropTableDB("LPtest_forConn.toml");
    LPG::Logger logger("LPtest_constructorAndLog.toml");
    LPG::Logger logger2(logger);
    logger.debug("test-debug");
    logger.info("test-info");
    logger.warning("test-warning");
    logger.error("test-error");
    logger2.debug("test-debug");
    logger2.info("test-info");
    logger2.warning("test-warning");
    logger2.error("test-error");
    PGconn *conn = connection("LPtest_forConn.toml");
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

    std::string message=PQgetvalue(res,0,3);
    EXPECT_EQ(message,"test-debug");
    message=PQgetvalue(res,1,3);
    EXPECT_EQ(message,"test-info");
    message=PQgetvalue(res,2,3);
    EXPECT_EQ(message,"test-warning");
    message=PQgetvalue(res,3,3);
    EXPECT_EQ(message,"test-error");
    message=PQgetvalue(res,4,3);
    EXPECT_EQ(message,"test-debug");
    message=PQgetvalue(res,5,3);
    EXPECT_EQ(message,"test-info");
    message=PQgetvalue(res,6,3);
    EXPECT_EQ(message,"test-warning");
    message=PQgetvalue(res,7,3);
    EXPECT_EQ(message,"test-error");

    PQclear(res);
    PQfinish(conn);
}

TEST(LogPostgresql, eqOfCopying)
{
    dropTableDB("LPtest_forConn.toml");
    LPG::Logger logger("LPtest_constructorAndLog.toml");
    LPG::Logger logger2 = logger;
    logger.debug("test-debug");
    logger.info("test-info");
    logger.warning("test-warning");
    logger.error("test-error");
    logger2.debug("test-debug");
    logger2.info("test-info");
    logger2.warning("test-warning");
    logger2.error("test-error");
    PGconn *conn = connection("LPtest_forConn.toml");
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

    std::string message=PQgetvalue(res,0,3);
    EXPECT_EQ(message,"test-debug");
    message=PQgetvalue(res,1,3);
    EXPECT_EQ(message,"test-info");
    message=PQgetvalue(res,2,3);
    EXPECT_EQ(message,"test-warning");
    message=PQgetvalue(res,3,3);
    EXPECT_EQ(message,"test-error");
    message=PQgetvalue(res,4,3);
    EXPECT_EQ(message,"test-debug");
    message=PQgetvalue(res,5,3);
    EXPECT_EQ(message,"test-info");
    message=PQgetvalue(res,6,3);
    EXPECT_EQ(message,"test-warning");
    message=PQgetvalue(res,7,3);
    EXPECT_EQ(message,"test-error");

    PQclear(res);
    PQfinish(conn);
}

TEST(LogPostgresql, constructorOfMoving)
{
    dropTableDB("LPtest_forConn.toml");
    LPG::Logger logger("LPtest_constructorAndLog.toml");
    LPG::Logger logger2(std::move(logger));
    logger2.debug("test-debug");
    logger2.info("test-info");
    logger2.warning("test-warning");
    logger2.error("test-error");
    PGconn *conn = connection("LPtest_forConn.toml");
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

    std::string message=PQgetvalue(res,0,3);
    EXPECT_EQ(message,"test-debug");
    message=PQgetvalue(res,1,3);
    EXPECT_EQ(message,"test-info");
    message=PQgetvalue(res,2,3);
    EXPECT_EQ(message,"test-warning");
    message=PQgetvalue(res,3,3);
    EXPECT_EQ(message,"test-error");

    PQclear(res);
    PQfinish(conn);
}

TEST(LogPostgresql, eqOfMoving)
{
    dropTableDB("LPtest_forConn.toml");
    LPG::Logger logger("LPtest_constructorAndLog.toml");
    LPG::Logger logger2=std::move(logger);
    logger2.debug("test-debug");
    logger2.info("test-info");
    logger2.warning("test-warning");
    logger2.error("test-error");
    PGconn *conn = connection("LPtest_forConn.toml");
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

    std::string message=PQgetvalue(res,0,3);
    EXPECT_EQ(message,"test-debug");
    message=PQgetvalue(res,1,3);
    EXPECT_EQ(message,"test-info");
    message=PQgetvalue(res,2,3);
    EXPECT_EQ(message,"test-warning");
    message=PQgetvalue(res,3,3);
    EXPECT_EQ(message,"test-error");

    PQclear(res);
    PQfinish(conn);
}

 TEST(LogPostgresql, noExistFile)
 {
     dropTableDB("LPtest_forConn.toml");
     LPG::Logger logger("noExistFile.txt");
     PGconn *conn = connection("LPtest_forConn.toml");
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

TEST(LogPostgresql, numOfApplication)
{
    dropTableDB("LPtest_forConn.toml");
    LPG::Logger logger("LPtest_numOfApp.toml");
    logger.debug("test-debug");
    PGconn *conn = connection("LPtest_forConn.toml");
    PGresult *res = PQexec(conn,"SELECT * FROM logs");
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        std::cerr << "Error of selecting from table: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        throw std::logic_error("Error of selecting from table");
    }
    std::string appName=PQgetvalue(res,0,2);
    EXPECT_EQ(appName, "test-app");
    PQclear(res);
    PQfinish(conn);
}

TEST(LogPostgresql, waysToSave)
{
    dropTableDB("LPtest_forConn.toml");
    LPG::Logger logger("LPtest_waysToSave.toml");
    logger.debug("test-debug");
    logger.info("test-info");
    logger.warning("test-warning");
    logger.error("test-error");
    PGconn *conn = connection("LPtest_forConn.toml");
    PGresult *res =
             PQexec(conn, "CREATE TABLE IF NOT EXISTS Logs (dateTime TIMESTAMP DEFAULT now(), level LogLevel, message TEXT);");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr <<"Error of creating table: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        throw std::logic_error("Error of creating table");
    }
    PQclear(res);
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