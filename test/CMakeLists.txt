include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        eff443c6ef5eb6ab598bfaae27f9427fdb4f6af7
        # URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(tests
        ../src/LogPostgresql.cpp
        LPtests.cpp
)

add_executable(GB
        GlobalTest.cpp
        ../src/LogPostgresql.cpp
)
include(GoogleTest)
gtest_discover_tests(tests)
include_directories(../include)
include_directories(${POSTGRESQL_INCLUDE_DIR})
target_link_libraries(GB
        PRIVATE pq
        PRIVATE nlohmann_json::nlohmann_json
        stdc++
)
target_link_libraries(tests
        GTest::gtest_main
        pq
        nlohmann_json::nlohmann_json
        stdc++
)