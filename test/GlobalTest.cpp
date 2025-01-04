#include "LogPostgresql.h"

using namespace LPG;

int main(){
  Logger logger("test.txt");
  logger.log(LogLevel::DEBUG, "test - debug");
  logger.log(LogLevel::INFO, "test - info");
  logger.log(LogLevel::WARNING, "test - warning");
  logger.log(LogLevel::ERROR, "test - error");
  return 0;
}