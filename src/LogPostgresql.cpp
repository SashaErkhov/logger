#include "../include/LogPostgresql.h"
#include <fstream>
#include <iostream>
#include <ctime>
#include <libpq-fe.h>

inline const char* getDateTime() {
    time_t timestamp = time(0);
    time(&timestamp);
    return ctime(&timestamp);
}

namespace LogPostgresql{
    Logger::Logger(const char* configFile): status_(Status_::NORMAL)
    {
        std::ifstream file;
        file.open(configFile,std::ios_base::binary);
        if (!file.is_open())
        {
            std::cerr << getDateTime()<<" [ERROR] Can't open config file " << configFile << std::endl;
            status_ = Status_::ERROR;
            return;
        }
        char line[1024];
        file.read(line, 1024);
        PGconn *conn = PQconnectdb(line);
        if (PQstatus(conn) != CONNECTION_OK) {
            std::cerr << getDateTime()<<" [ERROR] Can't connect to database: " << PQerrorMessage(conn) << std::endl;
            PQfinish(conn);
            status_ = Status_::ERROR;
            return;
        }
    }
}
