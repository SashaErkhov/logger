cmake_minimum_required(VERSION 3.20)
project(logger)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(FetchContent)

FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_TAG v3.11.3
        #URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)
add_library(LogPostgresql STATIC
        src/LogPostgresql.cpp
)
set(POSTGRESQL_INCLUDE_DIR "/usr/include/postgresql/" CACHE PATH "Path to PostgreSQL include directory")
include_directories(${POSTGRESQL_INCLUDE_DIR})
include_directories(include)
target_link_libraries(LogPostgresql
        PRIVATE pq
        PRIVATE nlohmann_json::nlohmann_json
        stdc++)
target_include_directories(LogPostgresql
        PUBLIC include
        PRIVATE src
)
set_target_properties(LogPostgresql PROPERTIES CXX_STANDARD 17)
add_subdirectory(test)