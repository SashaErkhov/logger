#include "LogPostgresql.h"
#include <fstream>
#include <iostream>
#include <ctime>
#include <libpq-fe.h>
#include <sstream>

namespace LPG{

    const char* Logger::getDateTime__() const{
        time_t timestamp = time(0);
        time(&timestamp);
        char* dt = ctime(&timestamp);
        dt[24] = '\0';
        return dt;
    }

    void Logger::connection__()
    {
        conn_ = PQconnectdb(strForConnection_.c_str());
        if (PQstatus(conn_) != CONNECTION_OK) {
            std::cerr << getDateTime__()<<" [ERROR] Can't connect to database: " << PQerrorMessage(conn_) << std::endl;
            PQfinish(conn_);
            status_ = Status_::ERROR;
            return;
        }
    }

    void Logger::parseConfig__(const char* configFile)
    {
        std::ifstream file(configFile);
        try{
            json j = json::parse(file);
            std::stringstream ss;
            ss<<"dbname="<<j["dbname"].get<std::string>()<<" user="
            <<j["user"].get<std::string>()<<"  password="<<j["password"].get<std::string>()
            <<" host="<<j["host"].get<std::string>()<<" port="<<j["port"].get<std::string>();
            strForConnection_=ss.str();
        }catch (...)
        {
            std::cerr << getDateTime__()<<" [ERROR] Can't parse config file " << configFile << std::endl;
            status_ = Status_::ERROR;
            return;
        }
    }

    Logger::Logger(const char* configFile): status_(Status_::NORMAL)
    {
        parseConfig__(configFile);
        if (status_ == Status_::ERROR) return;
        connection__();
        if (status_ == Status_::ERROR) return;
        res_ = PQexec(conn_, "DO $$ BEGIN CREATE TYPE LogLevel AS ENUM ('DEBUG', 'INFO','NOTICE', 'WARNING', 'ERROR', 'CRITICAL', 'ALERT', 'EMERGENCY'); EXCEPTION WHEN duplicate_object THEN null; END $$;");
        if (PQresultStatus(res_) != PGRES_COMMAND_OK) {
            std::cerr <<getDateTime__()<<" [ERROR] Error of creating type: " << PQerrorMessage(conn_) << std::endl;
            PQclear(res_);
            PQfinish(conn_);
            status_ = Status_::ERROR;
            return;
        }
        PQclear(res_);
        res_ =
            PQexec(conn_, "CREATE TABLE IF NOT EXISTS Logs (dateTime TIMESTAMP DEFAULT now(), level LogLevel, message TEXT);");
        if (PQresultStatus(res_) != PGRES_COMMAND_OK) {
            std::cerr <<getDateTime__()<<" [ERROR] Error of creating table: " << PQerrorMessage(conn_) << std::endl;
            PQclear(res_);
            PQfinish(conn_);
            status_ = Status_::ERROR;
            return;
        }
        PQclear(res_);
    }

    Logger::~Logger()
    {
        if (status_ == Status_::ERROR)
        {
            std::cerr << getDateTime__()<<" [ERROR] Some problem in the past - logger" << std::endl;
            return;
        }
        PQfinish(conn_);
    }

    Logger::Logger(const Logger& other)
    {
        conn_=nullptr;
        res_=nullptr;
        status_=other.status_;
        strForConnection_=other.strForConnection_;
        if (status_ == Status_::NORMAL)
        {
            connection__();
        }
    }

    void Logger::swap__(Logger& other)
    {
        std::swap(conn_,other.conn_);
        std::swap(res_,other.res_);
        std::swap(status_,other.status_);
    }

    Logger& Logger::operator=(const Logger& other)
    {
        if (this != &other)
        {
            Logger tmp(other);
            swap__(tmp);
        }
        return *this;
    }

    Logger::Logger(Logger&& other)
    {
        if (other.status_ == Status_::ERROR)
        {
            status_ = Status_::ERROR;
            conn_=other.conn_=nullptr;
            res_=other.res_=nullptr;
        } else
        {
            swap__(other);
            other.status_=Status_::ERROR;
            other.conn_=nullptr;
            other.res_=nullptr;
        }
    }

    Logger& Logger::operator=(Logger&& other)
    {
        if (this != &other)
        {
            swap__(other);
            other.status_=Status_::ERROR;
            other.conn_=nullptr;
            other.res_=nullptr;
        }
        return *this;
    }


    void Logger::sendToDb__(const char* logLevel, const char* message)
    {
        std::stringstream ss;
        ss<<"INSERT INTO Logs (dateTime, level, message) VALUES (now(), '"<<logLevel<<"', '"<<message<<"');";
        res_=PQexec(conn_, ss.str().c_str());
        if (PQresultStatus(res_) != PGRES_COMMAND_OK)
        {
            std::cerr << getDateTime__()<<" [ERROR] Error of inserting into database: " << PQerrorMessage(conn_) << std::endl;
            PQclear(res_);
            PQfinish(conn_);
            status_ = Status_::ERROR;
            return;
        }
        PQclear(res_);
    }


    void Logger::debug(const char* message)
    {
        if (status_ == Status_::ERROR)
        {
            std::cerr << getDateTime__()<<" [ERROR] Some problem in the past - logger" << std::endl;
            return;
        }
        std::cout << getDateTime__()<<" [DEBUG] " << message << std::endl;
        sendToDb__("DEBUG", message);
    }

    void Logger::info(const char* message)
    {
        if (status_ == Status_::ERROR)
        {
            std::cerr << getDateTime__()<<" [ERROR] Some problem in the past - logger" << std::endl;
            return;
        }
        std::cout << getDateTime__()<<" [INFO] " << message << std::endl;
        sendToDb__("INFO", message);
    }

    void Logger::notice(const char* message)
    {
        if (status_ == Status_::ERROR)
        {
            std::cerr << getDateTime__()<<" [ERROR] Some problem in the past - logger" << std::endl;
            return;
        }
        std::cout << getDateTime__()<<" [NOTICE] " << message << std::endl;
        sendToDb__("NOTICE", message);
    }

    void Logger::warning(const char* message)
    {
        if (status_ == Status_::ERROR)
        {
            std::cerr << getDateTime__()<<" [ERROR] Some problem in the past - logger" << std::endl;
            return;
        }
        std::cerr << getDateTime__()<<" [WARNING] " << message << std::endl;
        sendToDb__("WARNING", message);
    }

    void Logger::error(const char* message)
    {
        if (status_ == Status_::ERROR)
        {
            std::cerr << getDateTime__()<<" [ERROR] Some problem in the past - logger" << std::endl;
            return;
        }
        std::cerr << getDateTime__()<<" [ERROR] " << message << std::endl;
        sendToDb__("ERROR", message);
    }

    void Logger::critical(const char* message)
    {
        if (status_ == Status_::ERROR)
        {
            std::cerr << getDateTime__()<<" [ERROR] Some problem in the past - logger" << std::endl;
            return;
        }
        std::cerr << getDateTime__()<<" [CRITICAL] " << message << std::endl;
        sendToDb__("CRITICAL", message);
    }

    void Logger::alert(const char* message)
    {
        if (status_ == Status_::ERROR)
        {
            std::cerr << getDateTime__()<<" [ERROR] Some problem in the past - logger" << std::endl;
            return;
        }
        std::cerr << getDateTime__()<<" [ALERT] " << message << std::endl;
        sendToDb__("ALERT", message);
    }

    void Logger::emergency(const char* message)
    {
        if (status_ == Status_::ERROR)
        {
            std::cerr << getDateTime__()<<" [ERROR] Some problem in the past - logger" << std::endl;
            return;
        }
        std::cerr << getDateTime__()<<" [EMERGENCY] " << message << std::endl;
        sendToDb__("EMERGENCY", message);
    }
}
//hehe