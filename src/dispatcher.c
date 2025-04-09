#include <stdlib.h>
#include <stdio.h>

#include "mongoose.h"
#include "log.h"

#include "utils.h"
#include "CONFIG.h"
#include "common.h"
#include "pipeline.h"
#include "handlers.h"

#define URI_CAPS_MAX_BUFF 512

// Connection event handler function
static void fn(struct mg_connection *c, int ev, void *ev_data)
{
    static const struct mg_http_serve_opts dummy_opts;
    static struct mg_str caps[URI_CAPS_MAX_BUFF];

    if (ev == MG_EV_HTTP_MSG)
    {
        struct mg_http_message *hm = (struct mg_http_message *)ev_data;
        if (mg_match(hm->uri, mg_str("/"), NULL))
        {
            mg_http_serve_file(c, hm, "assets/info.html", &dummy_opts);
        }
        else if (mg_match(hm->uri, mg_str("/img/#"), caps))
        {
            log_trace("accessing file %.*s", caps->len, caps->buf);
            
            const char *processed_img_ispath = pipline_run(hm);

            if (!processed_img_ispath) {
                mg_http_reply(c, 400, "", "bad request parameter, pipline failed");
                return;
            }

            const char *processed_img_path = handler_parse_IsPath(processed_img_ispath);
            free((char *)processed_img_ispath);

            // log_debug("%s", processed_img_path);

            mg_http_serve_file(c, hm, processed_img_path, &dummy_opts);
            free((char *)processed_img_path);
        }
        else if (mg_match(hm->uri, mg_str("/assets/*"), NULL))
        {
            static const struct mg_http_serve_opts assets_opts = {.root_dir = "/assets=assets"};
            mg_http_serve_dir(c, hm, &assets_opts);
        }
        else
        {
            mg_http_serve_file(c, hm, "assets/404.html", &dummy_opts);
        }
    }
}

void start_dispatcher_loop()
{
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
    mg_http_listen(&mgr, CONFIG_SERVER_URL, fn, NULL);
    log_info("start listening on: %s", CONFIG_SERVER_URL);
    SYSTEM_STATE.is_running = 1;

    while (SYSTEM_STATE.is_running)
    {
        mg_mgr_poll(&mgr, 1000); // Infinite event loop
    }
}