#ifndef LOG_POSTGRESQL_H_2024
#define LOG_POSTGRESQL_H_2024

#include <libpq-fe.h>
#include <string>
#include <toml++/toml.hpp>
#include <cstdint>
#include <iostream>
#include <ctime>
#include <sstream>
#include <bitset>

namespace LPG
{
  class Logger
  {
    enum class Status_: std::uint8_t
    {
      NORMAL = 0,
      ERROR = 1
    };

    Status_ status_;
    PGconn *conn_;
    PGresult *res_;
    std::string strForConnection_;
    std::bitset<3> waysToSave_; // 0 - console, 1 - database, 2 - system
    std::string nameOfApplication_;


    const char* getDateTime__() const;
    void swap__(Logger&);
    void connection__();
    void sendToDatabase__(const char*, const char*);
    void sendToConsoleOutput__(const char*, const char*);
    void sendToConsoleError__(const char*, const char*);
    void sendToSystem__(const char*, const char*);
    void parseConfig__(const char*);

    Logger();
  public:
    Logger(const char*);
    ~Logger();
    Logger(const Logger&);
    Logger& operator=(const Logger&);
    Logger(Logger&&);
    Logger& operator=(Logger&&);

    void debug(const char*);
    void info(const char*);
    void notice(const char*);
    void warning(const char*);
    void error(const char*);
    void critical(const char*);
    void alert(const char*);
    void emergency(const char*);
  };

}

#endif// ! LOG_POSTGRESQL_H_2024