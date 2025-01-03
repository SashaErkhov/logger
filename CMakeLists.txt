cmake_minimum_required(VERSION 3.18)
project(logger)
set(CMAKE_CXX_STANDARD 17)
add_library(LogPostgresql STATIC
        src/LogPostgresql.cpp
)
include_directories(/usr/include/postgresql/)
target_link_libraries(LogPostgresql PRIVATE pq stdc++)
target_include_directories(LogPostgresql
        PUBLIC include
        PRIVATE src
)
set_target_properties(LogPostgresql PROPERTIES CXX_STANDARD 17)
add_subdirectory(test)