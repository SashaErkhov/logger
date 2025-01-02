add_executable(GB
        GlobalTest.cpp
        ../src/LogPostgresql.cpp
)
include_directories(/usr/include/postgresql/)
target_link_libraries(GB PRIVATE pq stdc++)