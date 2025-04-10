
#include <sqlite3.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "log.h"

/*
 * Establishes a new database connection
 * Parameters:
 *   db_path - Path to database file (use ":memory:" for in-memory DB)
 * Returns:
 *   Pointer to sqlite3 connection handle, NULL on failure
 */
sqlite3 *db_connect(const char *db_path)
{
    sqlite3 *db = NULL;
    int rc = sqlite3_open(db_path, &db);

    if (rc != SQLITE_OK)
    {
        log_error("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    if (sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, NULL, NULL) != SQLITE_OK)
    {
        log_error("Failed to enable foreign key support\n");
    }

    sqlite3_busy_timeout(db, 5000);

    return db;
}

/*
 * Closes a database connection
 * Parameters:
 *   db - Pointer to sqlite3 connection handle
 */
void db_close(sqlite3 *db)
{
    if (db)
    {
        // Finalize any remaining statements
        sqlite3_stmt *stmt;
        while ((stmt = sqlite3_next_stmt(db, NULL)) != NULL)
        {
            sqlite3_finalize(stmt);
        }

        // Close the connection
        int rc = sqlite3_close(db);
        if (rc != SQLITE_OK)
        {
            log_error("Failed to close database: %s\n", sqlite3_errmsg(db));
        }
    }
}

/*
 * Executes a SQL query on an existing database connection
 * Parameters:
 *   db - Pointer to sqlite3 connection handle
 *   sql - SQL query to execute
 *   callback - Callback function for results (NULL for no results)
 *   callback_arg - First argument to callback function
 * Returns:
 *   SQLITE_OK on success, error code on failure
 */
int db_execute(sqlite3 *db, const char *sql,
               int (*callback)(void *, int, char **, char **), void *callback_arg)
{
    if (!db || !sql)
    {
        return SQLITE_ERROR;
    }

    char *err_msg = NULL;
    int rc = sqlite3_exec(db, sql, callback, callback_arg, &err_msg);

    if (rc != SQLITE_OK)
    {
        log_error("SQL error: %s\nQuery: %s\n", err_msg, sql);
        sqlite3_free(err_msg);
    }

    return rc;
}

/*
 * Execute a sql file for a given db destination
 * exit when db cannot be processed
 *
 * Parameters:
 *   sql_path - Path to sql file
 *   db_path - Path to database file  (use ":memory:" for in-memory DB)
 * Returns:
 *   db - Pointer to sqlite3 connection handle
 */
sqlite3 *execute_sql_file(const char *sql_path, const char *db_path)
{
    sqlite3 *db;
    char *err_msg = NULL;
    FILE *fp = NULL;
    long file_size;
    char *sql_buffer = NULL;
    int rc;

    rc = sqlite3_open(db_path, &db);
    if (rc != SQLITE_OK)
    {
        log_error("Cannot open database: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        remove(db_path);
        exit(EXIT_FAILURE);
    }

    fp = fopen(sql_path, "rb");
    if (!fp)
    {
        log_error("Cannot open SQL file: %s", sql_path);
        sqlite3_close(db);
        remove(db_path);
        exit(EXIT_FAILURE);
    }

    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    rewind(fp);

    sql_buffer = (char *)malloc(file_size + 1);
    if (!sql_buffer)
    {
        log_error("Memory allocation failed");
        fclose(fp);
        sqlite3_close(db);
        remove(db_path);
        exit(EXIT_FAILURE);
    }

    if (fread(sql_buffer, 1, file_size, fp) != (size_t)file_size)
    {
        log_error("Failed to read SQL file");
        free(sql_buffer);
        fclose(fp);
        sqlite3_close(db);
        remove(db_path);
        exit(EXIT_FAILURE);
    }
    sql_buffer[file_size] = '\0';

    rc = sqlite3_exec(db, sql_buffer, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK)
    {
        log_error("SQL error: %s", err_msg);
        sqlite3_free(err_msg);
    }

    free(sql_buffer);
    fclose(fp);

    if (rc != SQLITE_OK)
    {
        remove(db_path);
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }

    return db;
}

