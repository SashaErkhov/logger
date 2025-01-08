# logger
Библиотека - логгер, использующий postgresql. Тестирование проводилось на Linux, но потенциально библиотека кроссплатформенная. Далее инструкция будет дана для Linux Ubuntu 24.04.1 LTS. Для Windows все аналогично, но используя другие команды. Используемый компилятор - g++. Но проблем с clang быть не должно.  
## Install
### Dependencies
Для сборки будет необходим cmake
```shell
sudo apt update
sudo apt install cmake
```
Библиотека использует postgresql. Используйте инструкции с оф сайта. Для работы с удаленной базой можно не устанавливать полноценную базу данных. Можно ограничится библиотекой 'libpq-dev'.
### Fork
Для использования необходимо скачать библиотеку в папку с проектом
```shell
git clone https://github.com/SashaErkhov/logger.git
git checkout v1.0.1
```
### Build
Ничего скачивать не нужно. Просто нужно изменить свой cmake
Пример cmake, который использует данную библиотеку
```cmake
cmake_minimum_required(VERSION 3.20)
project(testLogger)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(main main.cpp)
add_subdirectory(logger)
target_link_libraries(main PRIVATE LogPostgresql)
include_directories(${POSTGRESQL_INCLUDE_DIR})
```
${POSTGRESQL_INCLUDE_DIR} - путь до необходимых заголовочных файлов. Можете найти его через команду:
```shell
sudo find / -name libpq-fe.h
```
Это переменная cmake, так что ее легко изменить при создании build-директории или с помощью gui
## How to use
Вот небольшой, но всеобъемлющий примерчик
```c++
#include <iostream>
#include <LogPostgresql.h>

int main()
{
    LPG::Logger logger("config.txt");
    logger.log(LPG::LogLevel::DEBUG, "test-debug");
    logger.log(LPG::LogLevel::INFO, "test-info");
    logger.log(LPG::LogLevel::WARNING, "test-warning");
    logger.log(LPG::LogLevel::ERROR, "test-error");
    return 0;
}
```
config.txt - это конфиг файл вида:
```text
dbname=<> user=<> password=<> host=<> port=<>
```
Вместо <> подставьте своё  
Всего 4 уровня логирования: DEBUG, INFO, WARNING, ERROR