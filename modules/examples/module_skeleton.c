#include "module.h"

// Define module config
Module ModuleConfig = {
    .mversion = MODULE_MANAGER_VERSION,
    .name = "module_skeleton",
    .type = MODULE_TYPE_MID,
    .version = "1.0.0",
};

__attribute__((constructor)) void _module_init()
{
    // optional
}

__attribute__((destructor)) void _module_exit()
{
    // define your cleanup function
    // called before module exit
    return;
}

__attribute__((unused)) ModuleMsg module_call(ModuleMsg *input)
{
    // the passed input is guaranteed to be MODULE_STATUS_OK or MODULE_STATUS_WARN 
    // no need to check again.

    // Implement Your Module Below !!
    module_printf("hello world!");

    // every module must have a output
    return (ModuleMsg){
        .input_type = MODULE_INP_TYPE_INT,
        .data.i = 1,
        .status = MODULE_STATUS_OK,
        .opt_msg = "",
    };
}
