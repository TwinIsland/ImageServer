#pragma once

#include "sqlite3.h"

struct SystemState
{
    int is_running;
    int is_db_init;
};

typedef struct
{
    sqlite3 *persistent_db;
    sqlite3 *memory_db;
} DatabaseManager;

// enum IsPathType {
//     IsPathPVol,
//     IsPathEVol,
//     IsPathMVol,
// };

// typedef struct {
//     enum IsPathType type;
//     const char *path;
// } IsPath;

extern volatile struct SystemState SYSTEM_STATE;
extern DatabaseManager db;

#define PATH_BUFFER_SIZE 128
#define FILENAME_BUFFER_SIZE 64
#define EXT_BUFFER_SIZE 32
