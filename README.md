# logger
Библиотека - логгер, использующий postgresql. Тестирование проводилось на Linux, но потенциально библиотека кроссплатформенная. Далее инструкция будет дана для Linux Ubuntu 24.04.1 LTS. Для Windows все аналогично, но используя другие команды. Используемый компилятор - g++. Но проблем с clang быть не должно.  
## Install
### Dependencies
Для сборки будет необходим cmake
```shell
sudo apt update
sudo apt install cmake
```
Библиотека использует postgresql. Используйте инструкции с оф сайта. Для работы с удаленной базой можно не устанавливать полноценную базу данных. Можно ограничится библиотекой 'libpq'.
### Fork
Для использования необходимо скачать проект в папку с проектом
```shell
git clone https://github.com/SashaErkhov/logger.git
```
или
```shell
git submodule add https://github.com/SashaErkhov/logger.git logger
git submodule init
git submodule update
```
### Build
Не готово - требует тестирования и проверок