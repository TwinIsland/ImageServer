#pragma once

////// START OF CONFIGURATION //////

/*
 *      Server Configuration
 *
 */

#define CONFIG_SERVER_URL "http://0.0.0.0:8000" // server listening url

/*
 *      Database Configuration
 *
 */

#define CONFIG_pDatabase_PATH "pDatabase.sqlite3" // path for persistent database

/**
 *
 *      Pipeline Configuration
 *
 */

#define CONFIG_MODULES_PATH "./modules"

static const char *const CONFIG_PIPELINE[] = {
    "std_head_module.ismod",
    "webp_compress_module.ismod",
    "std_tail_module.ismod",
};

/**
 *
 *      Volume Configuration
 */

#define CONFIG_PVOL_DIRECTORY_PATH "./pvol"
#define CONFIG_PVOL_SIZE_LIMIT 8000 // in kb, set -1 for no limit
#define CONFIG_PVOL_IMAGE_NLIMIT -1 // max number of image, -1 for no limit

#define CONFIG_EVOL_DIRECTORY_PATH "./evol"
#define CONFIG_EVOL_SIZE_LIMIT -1    // in kb, set -1 for no limit
#define CONFIG_EVOL_IMGAE_N_LIMIT 10 // max number of image, -1 for no limit

#define CONFIG_MVOL_SIZE_LIMIT 1000  // in kb, set -1 for no limit
#define CONFIG_MVOL_IMGAE_N_LIMIT -1 // max number of image, -1 for no limit

////// END OF CONFIGURATION //////

#define CONFIG_PIPELINE_SIZE ((int)(sizeof(CONFIG_PIPELINE) / sizeof(CONFIG_PIPELINE[0])))
