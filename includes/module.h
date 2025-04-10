#pragma once

#include "log.h"

#define MODULE_MANAGER_VERSION 1

#define ModuleInputBufMax 256

enum ModuleType
{
    MODULE_TYPE_MID,
    MODULE_TYPE_HEAD,
    MODULE_TYPE_TAIL,
};

enum ModuleInputType
{
    MODULE_INP_TYPE_INT,
    MODULE_INP_TYPE_FLOAT,
    MODULE_INP_TYPE_STR,
    MODULE_INP_TYPE_PTR,
};

enum ModuleStatus
{
    MODULE_STATUS_OK,
    MODULE_STATUS_WARN,
    MODULE_STATUS_ERR,
};

typedef struct
{
    char *name;
    char *version;
    int mversion;
    enum ModuleType type;
} Module;

typedef struct
{
    enum ModuleInputType input_type;
    union
    {
        int i;
        float f;
        char str[ModuleInputBufMax];
        void *ptr;
    } data;
    enum ModuleStatus status;
    char opt_msg[ModuleInputBufMax];
} ModuleMsg;

#define module_printf(__fmt, ...) log_info("%s(%s): " __fmt, ModuleConfig.name, ModuleConfig.version, ##__VA_ARGS__)
