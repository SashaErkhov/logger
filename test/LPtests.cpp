#include "LogPostgresql.h"
#include <gtest/gtest.h>
#include <libpq-fe.h>
#include <fstream>
#include <exception>
#include <string>

PGconn* connection()
{
    std::ifstream file;
    file.open("test.txt",std::ios_base::binary);
    if (!file.is_open())
    {
        std::cerr<<"Can't open file \"test.txt\""<<std::endl;
        throw std::logic_error("Can't open file \"test.txt\"");
    }
    char line[1024];
    file.read(line,1024);
    for (size_t i=0;i<1024;++i)
    {
        if (line[i]=='p' and line[i+1]=='o' and line[i+2]=='r'
            and line[i+3]=='t' and line[i+4]=='=')
        {
            line[i+9] = '\0';
            break;
        }
    }
    PGconn *conn = PQconnectdb(line);
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
    LPG::Logger logger("test.txt");
    logger.log(LPG::LogLevel::DEBUG,"test-debug");
    logger.log(LPG::LogLevel::INFO,"test-info");
    logger.log(LPG::LogLevel::WARNING,"test-warning");
    logger.log(LPG::LogLevel::ERROR,"test-error");
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

TEST(LogPostgresql, constructorOfCopying)
{
    dropTableDB();
    LPG::Logger logger("test.txt");
    LPG::Logger logger2(logger);
    logger.log(LPG::LogLevel::DEBUG,"test-debug");
    logger.log(LPG::LogLevel::INFO,"test-info");
    logger.log(LPG::LogLevel::WARNING,"test-warning");
    logger.log(LPG::LogLevel::ERROR,"test-error");
    logger2.log(LPG::LogLevel::DEBUG,"test-debug");
    logger2.log(LPG::LogLevel::INFO,"test-info");
    logger2.log(LPG::LogLevel::WARNING,"test-warning");
    logger2.log(LPG::LogLevel::ERROR,"test-error");
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
    LPG::Logger logger("test.txt");
    LPG::Logger logger2 = logger;
    logger.log(LPG::LogLevel::DEBUG,"test-debug");
    logger.log(LPG::LogLevel::INFO,"test-info");
    logger.log(LPG::LogLevel::WARNING,"test-warning");
    logger.log(LPG::LogLevel::ERROR,"test-error");
    logger2.log(LPG::LogLevel::DEBUG,"test-debug");
    logger2.log(LPG::LogLevel::INFO,"test-info");
    logger2.log(LPG::LogLevel::WARNING,"test-warning");
    logger2.log(LPG::LogLevel::ERROR,"test-error");
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
    LPG::Logger logger("test.txt");
    LPG::Logger logger2(std::move(logger));
    logger.log(LPG::LogLevel::DEBUG,"test-debug");
    logger.log(LPG::LogLevel::INFO,"test-info");
    logger.log(LPG::LogLevel::WARNING,"test-warning");
    logger.log(LPG::LogLevel::ERROR,"test-error");
    logger2.log(LPG::LogLevel::DEBUG,"test-debug");
    logger2.log(LPG::LogLevel::INFO,"test-info");
    logger2.log(LPG::LogLevel::WARNING,"test-warning");
    logger2.log(LPG::LogLevel::ERROR,"test-error");
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
    LPG::Logger logger("test.txt");
    LPG::Logger logger2=std::move(logger);
    logger.log(LPG::LogLevel::DEBUG,"test-debug");
    logger.log(LPG::LogLevel::INFO,"test-info");
    logger.log(LPG::LogLevel::WARNING,"test-warning");
    logger.log(LPG::LogLevel::ERROR,"test-error");
    logger2.log(LPG::LogLevel::DEBUG,"test-debug");
    logger2.log(LPG::LogLevel::INFO,"test-info");
    logger2.log(LPG::LogLevel::WARNING,"test-warning");
    logger2.log(LPG::LogLevel::ERROR,"test-error");
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
    logger.log(LPG::LogLevel::DEBUG,"test-debug");
    logger.log(LPG::LogLevel::INFO,"test-info");
    logger.log(LPG::LogLevel::WARNING,"test-warning");
    logger.log(LPG::LogLevel::ERROR,"test-error");
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