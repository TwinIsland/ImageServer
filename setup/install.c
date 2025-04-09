#include "install.h"
#include "database.h"
#include "CONFIG.h"

void install(DatabaseManager *db_manager)
{
    db_manager->persistent_db = execute_sql_file("setup/pdb_init.sql", CONFIG_pDatabase_PATH);
    db_manager->memory_db = execute_sql_file("setup/mdb_init.sql", ":memory:");
}