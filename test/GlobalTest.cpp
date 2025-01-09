#include "LogPostgresql.h"

using namespace LPG;

int main(){
  Logger logger("test.txt");
  logger.debug("test-debug");
  logger.info("test-info");
  logger.notice("test-notice");
  logger.warning("test-warning");
  logger.error("test-error");
  logger.critical("test-critical");
  logger.alert("test-alert");
  logger.emergency("test-emergency");
  return 0;
}