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

#include <stdio.h>
#include "net/caro.h"

response_t* index_handler(request_t* request) {
    (void)request;
    printf("Hello World\n");
    return NULL;
}

int main(void)
{
    puts("Generated RIOT application: 'caro_example'");
    
    coap_server_t server;
    coap_server_init(&server);
    coap_server_use_driver(&server, UDP, &gcoap_driver);
    coap_server_register_resource(&server, "/", GET | POST, UDP | TCP,
                                  ";ct=0;rt=\"count\";obs", &index_handler);
    coap_server_start(&server);


    coap_server_debug_receive_request(&server, "/", GET, TCP);

    return 0;
}
