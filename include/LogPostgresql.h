#ifndef LOG_POSTGRESQL_H_2024
#define LOG_POSTGRESQL_H_2024

#include <libpq-fe.h>
#include <string>

namespace LPG
{
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
    void connection__();
    void sendToDb__(const char*, const char*);
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