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



bool index_handler(caro_request_t* request, caro_response_t* response) {
    printf("** HIER KÖNNTE IHR USER CODE STEHEN **\n");

    const char* path;
    method_t method;
    transport_t transport;
    uint16_t opt_len;
    uint16_t pay_len;

    request->driver->get_request_header_data(request->driver, request, &path, NULL, NULL, &method, &transport, &opt_len, &pay_len);

    printf("Request:\n\tPath: %s\n\tMethod: %d\n\tTransport: %d\n", path, method, transport);
    printf("Found %d Options\n", opt_len);

    for (int i = 0; i < opt_len; i++) {
        uint16_t opt_num;
        request->driver->get_request_opt_num(request->driver, request, i, &opt_num);

        if (opt_num == 17) {
            uint32_t opt_val;
            request->driver->get_request_opt_as_uint(request->driver, request, i, &opt_val);
            printf("Option #%d: %d %u\n", i, opt_num, opt_val);
        } else if (opt_num == 3 || opt_num == 292) {
            char opt_val[20];
            request->driver->get_request_opt_as_str(request->driver, request, i, opt_val, 20);
            printf("Option #%d: %d %s\n", i, opt_num, opt_val);
        } else {
            printf("Option #%d: %d\n", i, opt_num);
        }
    }

    printf("Payload len: %d\n", pay_len);
    const char* payload;
    request->driver->get_request_payload(request->driver, request, &payload);
    printf("Payload: %s\n", payload);


    // TODO: clean up driver access
    request->driver->initialize_response(request->driver, request, response, COAP_CODE_CONTENT); // TODO: find out the correct code and enum it
    request->driver->response_add_uint_option(request->driver, response, 12, 4);
    char* server_message = "Hello, this is a message generated from the server user handler!";
    request->driver->response_add_payload(request->driver, response, (uint8_t*)server_message, strlen(server_message));

    return true;
}

kernel_pid_t gcoap_init(void);



#define MAIN_QUEUE_SIZE (4)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

int caro_cli_cmd(int argc, char* argv[]) {
    (void)argc;
    (void)argv;


    if (argc >= 2 && strcmp(argv[1], "info") == 0) {

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


    caro_cli_cmd(0, NULL);

    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should never be reached */
    return 0;
}
