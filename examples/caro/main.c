/*
 * Copyright (C) 2023 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       caro example
 *
 * @author      Niko Kirste <niko.kirste@mailbox.tu-dresden.de>
 *
 * @}
 */

#define ENABLE_DEBUG 1
#include "debug.h"

#include <stdio.h>
#include "net/caro.h"

#include "net/gcoap.h"


#include "msg.h"
#include "shell.h"



response_t* index_handler(caro_request_t* request) {
    printf("** HIER KÖNNTE IHR USER CODE STEHEN **\n");
    printf("Request:\n\tPath: %s\n\tMethod: %d\n\tTransport: %d\n", request->path, request->method, request->transport);
    for (int i = 0; i < request->options_len; i++) {
        caro_message_option_t opt = request->options[i];
        printf("Options #%d: %u\n", opt.opt_num, opt.int_value);
    }
    return NULL;
}

kernel_pid_t gcoap_init(void);



#define MAIN_QUEUE_SIZE (4)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

int caro_cli_cmd(int argc, char* argv[]) {
    (void)argc;
    (void)argv;



    if (strcmp(argv[1], "info") == 0) {

        if (IS_USED(MODULE_GCOAP_DTLS)) {
            printf("CoAP server is listening on port %u\n", CONFIG_GCOAPS_PORT);
        } else {
            printf("CoAP server is listening on port %u\n", CONFIG_GCOAP_PORT);
        }
#if IS_USED(MODULE_GCOAP_DTLS)
        printf("Connection secured with DTLS\n");
        printf("Free DTLS session slots: %d/%d\n", dsm_get_num_available_slots(),
                dsm_get_num_maximum_slots());
#endif
        printf("Configured Proxy: ");
        return 0;
    }




    DEBUG("DEBUG\n");

    caro_server_t server;
    caro_server_init(&server);
    caro_server_use_driver(&server, UDP, &gcoap_driver);
    caro_server_register_resource(&server, "/", GET | POST, UDP | TCP,
                                  ";ct=0;rt=\"count\";obs", &index_handler);
    caro_server_start(&server);


    caro_server_debug_receive_request(&server, "/", GET, TCP);

    return 0;
}

static const shell_command_t shell_commands[] = {
        { "caro", "caro example", caro_cli_cmd },
        { NULL, NULL, NULL }
};

int main(void)
{
    puts("Generated RIOT application: 'caro_example'");

    /* for the thread running the shell */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("caro example app");

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should never be reached */
    return 0;
}
