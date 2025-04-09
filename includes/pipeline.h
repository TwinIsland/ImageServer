#pragma once

#include "module.h"

struct lModule // lefted Module
{
    Module module;
    ModuleMsg (*ModuleCallFunc)(ModuleMsg *);
    void *handler;
};

// load all modules in pipline, return 1 if loaded successfully
// otherwise return 0
int pipeline_load();
void pipeline_unload();
