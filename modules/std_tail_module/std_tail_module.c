#include <string.h>

#include "module.h"

Module ModuleConfig = {
    .mversion = MODULE_MANAGER_VERSION,
    .name = "std_tail_module",
    .type = MODULE_TYPE_TAIL,
    .version = "1.0.0",
};

__attribute__((destructor)) void _module_exit()
{
    // define your cleanup function
    // called before module exit
    return;
}

__attribute__((unused)) ModuleMsg _module_call(ModuleMsg *input)
{
    module_printf("(TAIL) hi, I receive: %s", input->data.str);

    ModuleMsg msg = (ModuleMsg){
        .input_type = MODULE_INP_TYPE_STR,
        .status = MODULE_STATUS_OK,
        .opt_msg = "",
    };

    strcpy(msg.data.str, "assets/sample.webp");
    return msg;
}
