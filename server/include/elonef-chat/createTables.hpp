#pragma once
#include <mysql_connection.h>

namespace Chat {
    void createTables(sql::Connection* conn);
}