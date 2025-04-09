#include <stdlib.h>
#include <signal.h>

#include "dispatcher.h"
#include "install.h"
#include "log.h"
#include "utils.h"

#include "pipeline.h"
#include "CONFIG.h"
#include "database.h"
#include "common.h"

DatabaseManager db;
volatile struct SystemState SYSTEM_STATE = {
    .is_running = 1,
};

void handle_signal(int sig)
{
    if (sig == SIGINT || sig == SIGTERM)
    {
        log_info("shutting down...");
        SYSTEM_STATE.is_running = 0;
    }
}

int main()
{
    signal(SIGINT, handle_signal);  // Ctrl+C
    signal(SIGTERM, handle_signal); // Termination request

    if (!is_file_exist(CONFIG_pDatabase_PATH))
        install(&db);

    log_info("establish persistent database connection");
    SYSTEM_STATE.is_db_init = 1;

    if (!pipeline_load())
    {
        log_error("pipline initialize failed");
        exit(EXIT_FAILURE);
    }
    log_info("pipline initialized!");

    log_info("initialize system state");
    start_dispatcher_loop();

    // do cleanup
    pipeline_unload(); 
    log_info("bye!");
}