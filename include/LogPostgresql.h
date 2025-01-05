#ifndef LOG_POSTGRESQL_H_2024
#define LOG_POSTGRESQL_H_2024

#include <libpq-fe.h>
#include <string>

namespace LPG
{

  enum class LogLevel
  {
    DEBUG,
    INFO,
    WARNING,
    ERROR
  };

  class Logger
  {
    enum class Status_
    {
      NORMAL,
      ERROR
    };

    Status_ status_;
    PGconn *conn_;
    PGresult *res_;
    std::string configFile_;

    const char* getDateTime__() const;
    void swap__(Logger&);
    const char* getLevelString__(LogLevel) const;
    void connection__();
  public:
    Logger(const char*);
    ~Logger();
    Logger(const Logger&);
    Logger& operator=(const Logger&);
    Logger(Logger&&);
    Logger& operator=(Logger&&);

    void log(LogLevel, const char*);

  };

}

#endif// ! LOG_POSTGRESQL_H_2024