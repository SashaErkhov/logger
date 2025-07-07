cmake_minimum_required(VERSION 3.20)
project(logger)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(FetchContent)

FetchContent_Declare(
        tomlplusplus
        GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
        GIT_TAG v3.4.0 # или последняя
)
FetchContent_MakeAvailable(tomlplusplus)
add_library(LogPostgresql STATIC
        src/LogPostgresql.cpp
)
set(POSTGRESQL_INCLUDE_DIR "/usr/include/postgresql/" CACHE PATH "Path to PostgreSQL include directory")
include_directories(${POSTGRESQL_INCLUDE_DIR})
include_directories(include)
target_link_libraries(LogPostgresql
        PRIVATE pq
        PRIVATE tomlplusplus::tomlplusplus
        stdc++)
target_include_directories(LogPostgresql
        PUBLIC include
        PRIVATE src
)
set_target_properties(LogPostgresql PROPERTIES CXX_STANDARD 17)
add_subdirectory(test)