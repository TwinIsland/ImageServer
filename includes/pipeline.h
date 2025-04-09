#pragma once

#include "module.h"
#include "mongoose.h"

struct lModule // lefted Module
{
    Module module;
    ModuleMsg (*ModuleCallFunc)(ModuleMsg *);
    void *handler;
};

// load all modules in pipline,
// return 1 if loaded successfully, otherwise return 0
int pipeline_load();

// unload all modules from pipline
// return 1 if loaded successfully, otherwise return 0
void pipeline_unload();

// run the pipline
// return the final image address, return NULL if failed
// WARNING: need to free the return value
const char *pipline_run(struct mg_http_message *hm);