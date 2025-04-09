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

static const char * const CONFIG_PIPELINE[] = {
    "std_head_module.ismod",
    "std_tail_module.ismod"
};


////// END OF CONFIGURATION //////

#define CONFIG_PIPELINE_SIZE ((int)(sizeof(CONFIG_PIPELINE)/sizeof(CONFIG_PIPELINE[0])))
