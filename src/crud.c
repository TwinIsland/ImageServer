#include <stdlib.h>
#include <stdint.h>

#include "crud.h"
#include "log.h"

int db_insert_image(sqlite3 *db, const ImageMeta *meta, const char *impath, int64_t original_id, const char *processing_params)
{
    if (!db || !meta)
    {
        return SQLITE_MISUSE;
    }

    const char *sql =
        "INSERT INTO images ("
        "original_id, file_name, file_impath, file_size, "
        "mime_type, width, height, md5_hash, processing_params"
        ") VALUES ("
        "?, ?, ?, ?, ?, ?, ?, ?, ?"
        ");";

    sqlite3_stmt *stmt = NULL;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        log_error("Failed to prepare statement: %s", sqlite3_errmsg(db));
        return rc;
    }

    // Bind parameters
    sqlite3_bind_int(stmt, 1, original_id); // NULL becomes 0
    sqlite3_bind_text(stmt, 2, meta->filename, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, impath, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, meta->size);
    sqlite3_bind_text(stmt, 5, meta->mime_type, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 6, meta->width);
    sqlite3_bind_int(stmt, 7, meta->height);
    sqlite3_bind_text(stmt, 8, meta->md5_hash, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 9, processing_params, -1, SQLITE_STATIC);

    // Execute insertion
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        log_error("Insert failed: %s", sqlite3_errmsg(db));
    }

    // Cleanup
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE ? SQLITE_OK : rc;
}

int db_get_next_image_id(sqlite3 *db, int64_t *next_id)
{
    if (!db || !next_id)
    {
        return SQLITE_MISUSE;
    }

    const char *sql = "SELECT COALESCE(MAX(image_id), 0) + 1 FROM images;";
    sqlite3_stmt *stmt = NULL;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        log_error("Failed to prepare statement: %s", sqlite3_errmsg(db));
        return rc;
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        *next_id = sqlite3_column_int64(stmt, 0);
    }
    else
    {
        log_error("Failed to get next ID: %s", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    return rc == SQLITE_ROW ? SQLITE_OK : (rc == SQLITE_DONE ? SQLITE_ERROR : rc);
}