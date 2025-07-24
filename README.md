# logger
Библиотека - логгер, использующий postgresql. Тестирование проводилось на Linux, но потенциально библиотека кроссплатформенная. Далее инструкция будет дана для Linux Ubuntu 24.04.1 LTS. Для Windows все аналогично, но используя другие команды. Используемый компилятор - g++. Но проблем с clang быть не должно. Особенностью именно этого логгера является настройка через конфиг в виде .toml и возможность взаимодействия с postgreSQL. То есть код пишется один раз, но конфиг изменяется под разные среды, сервера
## Install
### Dependencies
Для сборки будет необходим cmake
```shell
sudo apt update
sudo apt install cmake
```
Библиотека использует postgresql. Используйте инструкции с оф сайта. Для работы с удаленной базой можно не устанавливать полноценную базу данных. Можно ограничится библиотекой 'libpq-dev'.
### Build
#### 1) через add_subdirectory
Здесь есть два путя: скачать архив из релизов в github или через git:  
```shell
git clone https://github.com/SashaErkhov/logger.git
git checkout v2.0.0
```
В таком случае CMakeLists.txt выглядит примерно вот так:
```cmake
cmake_minimum_required(VERSION 3.20)
project(testLogger)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(main main.cpp)
add_subdirectory(logger)
target_link_libraries(main PRIVATE LogPostgresql)
```
#### 2) Fetch Content
Пример CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 3.20)
include(FetchContent)
project(test)
FetchContent_Declare(
        logger
        GIT_REPOSITORY https://github.com/SashaErkhov/logger
        GIT_TAG origin/newFeatures
)
FetchContent_MakeAvailable(logger)
add_executable(test main.cpp)
target_link_libraries(test LogPostgresql)
```
Библиотека использует вот такую переменную cmake:
${POSTGRESQL_INCLUDE_DIR} - путь до необходимых заголовочных файлов. Можете найти его через команду:
```shell
sudo find / -name libpq-fe.h
```
Это переменная cmake, так что ее легко изменить при создании build-директории или с помощью gui, обычно:
```
/usr/include/postgresql/
```
## How to use
Вот небольшой, но всеобъемлющий примерчик
```c++
#include <LogPostgresql.h>

int main()
{
    LPG::Logger logger("config.toml");
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
```
config.toml - это конфиг файл вида:
```toml
[meta]
name_of_application = "app"

[types_of_logs]
debug = true
info = true
notice = true
warning = true
error = true
critical = true
alert = true
emergency = true

[ways_to_save]
savingToConsole = true
savingToDatabase = true
savingToSystem = true

[database]
dbname = "testlogger"
user = "postgres"
password = "postgres"
host = "localhost"
port = "5432"
numOfRows = 15 # The number of rows that will be saved.
# The old ones wiil be deleted. Defeault - inf
```
Всего 8 уровня логирования: DEBUG, INFO, NOTICE, WARNING, ERROR, CRITICAL, ALERT, EMERGENCY