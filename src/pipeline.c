#include <dlfcn.h>

#include "pipeline.h"
#include "common.h"
#include "log.h"

#include "CONFIG.h"
#include "mongoose.h"

static struct lModule Pipeline[CONFIG_PIPELINE_SIZE] = {{{0}, 0, 0}};
static int module_count = 0;

static int module_register(struct lModule lmodule)
{
    if (lmodule.module.mversion != MODULE_MANAGER_VERSION)
    {
        log_error("%s version mistached (%s!=%s)", lmodule.module.name, lmodule.module.mversion, MODULE_MANAGER_VERSION);
        return -1;
    }

    if (Pipeline[module_count].module.name != NULL)
    {
        log_error("duplicate module %s", lmodule.module.name);
        return -1;
    }

    if (module_count == 0 && lmodule.module.type != MODULE_TYPE_HEAD)
    {
        log_error("the first module in pipeline should always be head module");
        return -1;
    }

    if (module_count == CONFIG_PIPELINE_SIZE - 1 && lmodule.module.type != MODULE_TYPE_TAIL)
    {
        log_error("the first module in pipeline should always be tail module");
        return -1;
    }

    Pipeline[module_count] = lmodule;

    log_info("module loaded: %s (%s)", lmodule.module.name, lmodule.module.version);
    return module_count++;
}

int pipeline_load()
{
    for (int i = 0; i < CONFIG_PIPELINE_SIZE; ++i)
    {
        char module_path[PATH_BUFFER_SIZE];

        snprintf(module_path, sizeof(module_path), "%s/%s", CONFIG_MODULES_PATH, CONFIG_PIPELINE[i]);

        void *handler = dlopen(module_path, RTLD_LAZY);
        if (!handler)
        {
            log_error("Failed to load module: %s", dlerror());
            pipeline_unload();
            return 0;
        }

        ModuleMsg (*ModuleCallFunc)(ModuleMsg *) = (ModuleMsg (*)(ModuleMsg *))dlsym(handler, "_module_call");
        Module *moduleConfig = (Module *)dlsym(handler, "ModuleConfig");

        if (!ModuleCallFunc || !moduleConfig)
        {
            log_error("Module doesn't export required symbols: %s", dlerror());
            dlclose(handler);
            pipeline_unload();
            return 0;
        }

        struct lModule current_module = (struct lModule){
            .module = *moduleConfig,
            .handler = handler,
            .ModuleCallFunc = ModuleCallFunc,
        };

        int ret = module_register(current_module);

        if (ret == -1)
        {
            log_error("fail to register module: %s", current_module.module.name);
            dlclose(handler);
            pipeline_unload();
            return 0;
        }
    }

    return 1;
}

void pipeline_unload()
{
    for (int i = 0; i < CONFIG_PIPELINE_SIZE; ++i)
    {
        if (Pipeline[i].module.name)
        {
            log_info("unload module: %s", Pipeline[i].module.name);            
            dlclose(Pipeline[i].handler);
        }
    }
    log_info("pipline unloaded!"); 
    module_count = 0;
}

const char *pipline_run(struct mg_http_message *hm)
{
    ModuleMsg cur_msg = (ModuleMsg) {
        .input_type = MODULE_INP_TYPE_STR,
        .data.ptr = hm,
        .status = MODULE_STATUS_OK,
        .opt_msg = "",
    };
    
    for (int i = 0; i < CONFIG_PIPELINE_SIZE; ++i) {
        struct lModule *cur_module = &Pipeline[i];
        cur_msg = cur_module->ModuleCallFunc(&cur_msg);
        
        if (cur_msg.status == MODULE_STATUS_WARN) {
            log_warn("module '%s' warning: %s", cur_module->module.name, cur_msg.opt_msg);
        }
        else if (cur_msg.status == MODULE_STATUS_ERR) {
            log_error("module '%s' error: %s", cur_module->module.name, cur_msg.opt_msg);
            return NULL;
        }
        log_info("module' %s' done\t%s", cur_module->module.name, cur_msg.opt_msg);        
    }

    if (cur_msg.input_type != MODULE_INP_TYPE_STR) {
        log_error("invalid tail module message type");
        return NULL;
    }

    return strdup(cur_msg.data.str);
}