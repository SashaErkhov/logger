cmake_minimum_required(VERSION 3.20)
project(logger VERSION 2.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(FetchContent)

FetchContent_Declare(
        tomlplusplus
        GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
        GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)
add_library(LogPostgresql STATIC
        src/LogPostgresql.cpp
)
set(POSTGRESQL_INCLUDE_DIR "/usr/include/postgresql/" CACHE PATH "Path to PostgreSQL include directory")
target_link_libraries(LogPostgresql PUBLIC
        pq
        tomlplusplus::tomlplusplus
        stdc++
        m
)
target_include_directories(LogPostgresql PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
target_include_directories(LogPostgresql PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${POSTGRESQL_INCLUDE_DIR}
)
target_compile_features(LogPostgresql PUBLIC cxx_std_17)
add_subdirectory(test)