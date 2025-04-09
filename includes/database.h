#pragma once

#include <sqlite3.h>

sqlite3 *db_connect(const char *db_path);
void db_close(sqlite3 *db);
int db_execute(sqlite3 *db, const char *sql,
               int (*callback)(void *, int, char **, char **), void *callback_arg);

sqlite3 *execute_sql_file(const char *sql_path, const char *db_path);
