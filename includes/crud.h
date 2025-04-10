#pragma once

#include "database.h"
#include "image.h"

int db_insert_image(sqlite3 *db, const ImageMeta *meta, const char *impath, int64_t original_id, const char *processing_params);
int db_get_next_image_id(sqlite3 *db, int64_t *next_id);
