//
// Created by sommer on 04.11.23.
//

#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "server_utils.h"
#include "server_drivers.h"
#include "../utils.h"
#include "../request.h"
#include "../response.h"

#define NUM_RESOURCES 10

typedef struct {
    coap_resource_t resource_pool[NUM_RESOURCES]; // TODO: memory management
} coap_server_t;



typedef struct {
    char* path;
    char allowed_methods;
    char allowed_transports;
    char* link_params;
    resource_handler_t handler;
    bool is_valid;
} coap_resource_t;

void coap_server_use_driver(coap_server_t* server, transport_t t, server_driver_t driver);
void coap_server_register_resource(coap_server_t* server, char* path, methods_selector_t methods,
                                   transports_selector_t  transports, char* rt,
                                   resource_handler_t handler);
void coap_server_start(coap_server_t* server);





void coap_server_debug_receive_request(coap_server_t* server, char* path, method_t m, transport_t t);

#endif //SERVER_H
