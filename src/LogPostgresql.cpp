#include "LogPostgresql.h"

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
        try{
            auto config = toml::parse_file(configFile);
            std::stringstream ss;
            ss<<"dbname="<<config["database"]["dbname"].value_or("postgres")
            <<" user="<<config["database"]["user"].value_or("postgres")
            <<"  password="<<config["database"]["password"].value_or("postgres")
            <<" host="<<config["database"]["host"].value_or("localhost")
            <<" port="<<config["database"]["port"].value_or("5432");
            strForConnection_=ss.str();
            nameOfApplication_ = config["meta"]["name_of_application"].value_or("LogPostgresql");
            waysToSave_[0] = config["ways_to_save"]["savingToConsole"].value_or(false);
            waysToSave_[1] = config["ways_to_save"]["savingToDatabase"].value_or(false);
            waysToSave_[2] = config["ways_to_save"]["savingToSystem"].value_or(false);
            typesOfLogs_[0] = config["types_of_logs"]["debug"].value_or(false);
            typesOfLogs_[1] = config["types_of_logs"]["info"].value_or(false);
            typesOfLogs_[2] = config["types_of_logs"]["notice"].value_or(false);
            typesOfLogs_[3] = config["types_of_logs"]["warning"].value_or(false);
            typesOfLogs_[4] = config["types_of_logs"]["error"].value_or(false);
            typesOfLogs_[5] = config["types_of_logs"]["critical"].value_or(false);
            typesOfLogs_[6] = config["types_of_logs"]["alert"].value_or(false);
            typesOfLogs_[7] = config["types_of_logs"]["emergency"].value_or(false);
            numOfRows_ = config["database"]["numOfRows"].value_or(-1);
        }catch (...)
        {
            std::cerr << getDateTime__()<<" [ERROR] Can't parse config file " << configFile << std::endl;
            status_ = Status_::ERROR;
            return;
        }
    }

    Logger::Logger()
    {
        status_ = Status_::ERROR;
        conn_=nullptr;
        res_=nullptr;
        strForConnection_="";
        waysToSave_ = 0;
        nameOfApplication_ = "LogPostgresql";
        typesOfLogs_ = 0;
        numOfRows_ = -1;
    }

    Logger::Logger(const char* configFile): status_(Status_::NORMAL), conn_(nullptr), res_(nullptr), strForConnection_("")
    {
        parseConfig__(configFile);
        if (status_ == Status_::ERROR) return;
        if (!waysToSave_[1]) return;
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
            PQexec(conn_, "CREATE TABLE IF NOT EXISTS Logs (dateTime TIMESTAMP DEFAULT now() PRIMARY KEY, level LogLevel,nameApp TEXT DEFAULT 'LogPostgresql', message TEXT);");
        if (PQresultStatus(res_) != PGRES_COMMAND_OK) {
            std::cerr <<getDateTime__()<<" [ERROR] Error of creating table: " << PQerrorMessage(conn_) << std::endl;
            PQclear(res_);
            PQfinish(conn_);
            status_ = Status_::ERROR;
            return;
        }
        PQclear(res_);
        if (numOfRows_ < 0) return;
        std:: stringstream ss;
        ss << "CREATE OR REPLACE FUNCTION trim_logs() RETURNS TRIGGER AS $$ BEGIN DELETE FROM logs WHERE dateTime NOT IN ( SELECT dateTime FROM logs ORDER BY dateTime DESC LIMIT "
        << numOfRows_ << "); RETURN NULL; END; $$ LANGUAGE plpgsql;";
        res_ =
            PQexec(conn_,  ss.str().c_str());
        if (PQresultStatus(res_) != PGRES_COMMAND_OK) {
            std::cerr <<getDateTime__()<<" [ERROR] Error of creating table: " << PQerrorMessage(conn_) << std::endl;
            PQclear(res_);
            PQfinish(conn_);
            status_ = Status_::ERROR;
            return;
        }
        PQclear(res_);
        res_ =
            PQexec(conn_, "CREATE OR REPLACE TRIGGER trim_logs_trigger AFTER INSERT ON logs FOR EACH STATEMENT EXECUTE FUNCTION trim_logs();");
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
        waysToSave_ = other.waysToSave_;
        nameOfApplication_ = other.nameOfApplication_;
        typesOfLogs_ = other.typesOfLogs_;
        numOfRows_ = other.numOfRows_;
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
        std::swap(strForConnection_, other.strForConnection_);
        std::swap(waysToSave_, other.waysToSave_);
        std::swap(nameOfApplication_, other.nameOfApplication_);
        std::swap(typesOfLogs_, other.typesOfLogs_);
        std::swap(numOfRows_, other.numOfRows_);
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

    Logger::Logger(Logger&& other): Logger()
    {
        swap__(other);
    }

    Logger& Logger::operator=(Logger&& other)
    {
        if (this != &other)
        {
            swap__(other);
        }
        return *this;
    }


    void Logger::sendToDatabase__(const char* logLevel, const char* message)
    {
        std::stringstream ss;
        ss<<"INSERT INTO Logs (dateTime, level,nameApp, message) VALUES (now(), '"<<logLevel<<"','"<<nameOfApplication_<<"','"<<message<<"');";
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

    void Logger::sendToConsoleOutput__(const char* logLevel, const char* message)
    {
        std::cout << getDateTime__()<<" [" << logLevel << "] - "<< nameOfApplication_ << ": " << message << std::endl;
    }

    void Logger::sendToConsoleError__(const char* logLevel, const char* message)
    {
        std::cerr << getDateTime__()<<" [" << logLevel << "] - "<< nameOfApplication_ << ": " << message << std::endl;
    }

// TODO I don't know how to do it better
#ifdef _WIN32
#include <windows.h>
    
    void Logger::sendToSystem__(const char* logLevel, const char* message)
    {
        HANDLE hEventSource = RegisterEventSource(NULL, "LogPostgresql");
        if (!hEventSource) return;

        WORD wType;
        if (strcmp(logLevel, "DEBUG") == 0) wType = EVENTLOG_INFORMATION_TYPE;
        else if (strcmp(logLevel, "INFO") == 0) wType = EVENTLOG_INFORMATION_TYPE;
        else if (strcmp(logLevel, "NOTICE") == 0) wType = EVENTLOG_INFORMATION_TYPE;
        else if (strcmp(logLevel, "WARNING") == 0) wType = EVENTLOG_WARNING_TYPE;
        else wType = EVENTLOG_ERROR_TYPE;

        LPCSTR strings[1] = { (nameOfApplication_ + ": " + message) };
        ReportEvent(hEventSource, wType, 0, 0, NULL, 1, 0, strings, NULL);
        DeregisterEventSource(hEventSource);
    }
#elif defined(__linux__) || defined(__APPLE__)
#include <syslog.h>

    void Logger::sendToSystem__(const char* logLevel, const char* message)
    {
        int priority;
        if (strcmp(logLevel, "DEBUG") == 0) priority = LOG_DEBUG;
        if (strcmp(logLevel, "INFO") == 0) priority = LOG_INFO;
        if (strcmp(logLevel, "NOTICE") == 0) priority = LOG_NOTICE;
        if (strcmp(logLevel, "WARNING") == 0) priority = LOG_WARNING;
        if (strcmp(logLevel, "ERROR") == 0) priority = LOG_ERR;
        if (strcmp(logLevel, "CRITICAL") == 0) priority = LOG_CRIT;
        if (strcmp(logLevel, "ALERT") == 0) priority = LOG_ALERT;
        if (strcmp(logLevel, "EMERGENCY") == 0) priority = LOG_EMERG;

        openlog(nameOfApplication_.c_str(), LOG_PID | LOG_CONS, LOG_USER);
        syslog(priority, "%s", message);
        closelog();
    }
#else
    void Logger::sendToSystem__(const char* logLevel, const char* message)
    {
        return;
    }
#endif

    void Logger::debug(const char* message)
    {
        if (!typesOfLogs_[0]) return;
        if (status_ == Status_::ERROR)
        {
            std::cerr << getDateTime__()<<" [ERROR] Some problem in the past - logger" << std::endl;
            return;
        }
        if (waysToSave_[0]) sendToConsoleOutput__("DEBUG", message);
        if (waysToSave_[1]) sendToDatabase__("DEBUG", message);
        if (waysToSave_[2]) sendToSystem__("DEBUG", message);
    }

    void Logger::info(const char* message)
    {
        if (!typesOfLogs_[1]) return;
        if (status_ == Status_::ERROR)
        {
            std::cerr << getDateTime__()<<" [ERROR] Some problem in the past - logger" << std::endl;
            return;
        }
        if (waysToSave_[0]) sendToConsoleOutput__("INFO", message);
        if (waysToSave_[1]) sendToDatabase__("INFO", message);
        if (waysToSave_[2]) sendToSystem__("INFO", message);
    }

    void Logger::notice(const char* message)
    {
        if (!typesOfLogs_[2]) return;
        if (status_ == Status_::ERROR)
        {
            std::cerr << getDateTime__()<<" [ERROR] Some problem in the past - logger" << std::endl;
            return;
        }
        if (waysToSave_[0]) sendToConsoleOutput__("NOTICE", message);
        if (waysToSave_[1]) sendToDatabase__("NOTICE", message);
        if (waysToSave_[2]) sendToSystem__("NOTICE", message);
    }

    void Logger::warning(const char* message)
    {
        if (!typesOfLogs_[3]) return;
        if (status_ == Status_::ERROR)
        {
            std::cerr << getDateTime__()<<" [ERROR] Some problem in the past - logger" << std::endl;
            return;
        }
        if (waysToSave_[0]) sendToConsoleOutput__("WARNING", message);
        if (waysToSave_[1]) sendToDatabase__("WARNING", message);
        if (waysToSave_[2]) sendToSystem__("WARNING", message);
    }

    void Logger::error(const char* message)
    {
        if (!typesOfLogs_[4]) return;
        if (status_ == Status_::ERROR)
        {
            std::cerr << getDateTime__()<<" [ERROR] Some problem in the past - logger" << std::endl;
            return;
        }
        if (waysToSave_[0]) sendToConsoleError__("ERROR", message);
        if (waysToSave_[1]) sendToDatabase__("ERROR", message);
        if (waysToSave_[2]) sendToSystem__("ERROR", message);
    }

    void Logger::critical(const char* message)
    {
        if (!typesOfLogs_[5]) return;
        if (status_ == Status_::ERROR)
        {
            std::cerr << getDateTime__()<<" [ERROR] Some problem in the past - logger" << std::endl;
            return;
        }
        if (waysToSave_[0]) sendToConsoleError__("CRITICAL", message);
        if (waysToSave_[1]) sendToDatabase__("CRITICAL", message);
        if (waysToSave_[2]) sendToSystem__("CRITICAL", message);
    }

    void Logger::alert(const char* message)
    {
        if (!typesOfLogs_[6]) return;
        if (status_ == Status_::ERROR)
        {
            std::cerr << getDateTime__()<<" [ERROR] Some problem in the past - logger" << std::endl;
            return;
        }
        if (waysToSave_[0]) sendToConsoleError__("ALERT", message);
        if (waysToSave_[1]) sendToDatabase__("ALERT", message);
        if (waysToSave_[2]) sendToSystem__("ALERT", message);
    }

    void Logger::emergency(const char* message)
    {
        if (!typesOfLogs_[7]) return;
        if (status_ == Status_::ERROR)
        {
            std::cerr << getDateTime__()<<" [ERROR] Some problem in the past - logger" << std::endl;
            return;
        }
        if (waysToSave_[0]) sendToConsoleError__("EMERGENCY", message);
        if (waysToSave_[1]) sendToDatabase__("EMERGENCY", message);
        if (waysToSave_[2]) sendToSystem__("EMERGENCY", message);
    }
}