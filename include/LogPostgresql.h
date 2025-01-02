#ifndef LOG_POSTGRESQL_H_2024
#define LOG_POSTGRESQL_H_2024

namespace LogPostgresql
{

  enum class LogType
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
  public:
    Logger(const char*);
    ~Logger() = default;
    Logger(const Logger&);
    Logger& operator=(const Logger&);
    Logger(Logger&&);
    Logger& operator=(Logger&&);

    void log(LogType, const char*);

  };

}

#endif// ! LOG_POSTGRESQL_H_2024