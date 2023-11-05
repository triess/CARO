//
// Created by sommer on 04.11.23.
//

#ifndef COAPI_SERVER_H
#define COAPI_SERVER_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "../utils.h"
#include "server_utlis.h"
#include "server_drivers.h"

#define NUM_RESOURCES 10

typedef struct {
    char* path;
    char allowedMethods;
    char allowedTransports;
    char* linkParams;
    resource_handler_t handler;
    bool isValid;
} coap_resource_t;

typedef struct {
    coap_resource_t resourcePool[NUM_RESOURCES]; // TODO: change
} coap_server_t;

void coap_server_use_driver(coap_server_t* server, transport_t t, server_driver_t driver);
void coap_server_register_resource(coap_server_t* server, char* path, methods_selector_t methods,
                                   transports_selector_t  transports, char* rt, resource_handler_t handler);
void coap_server_start(coap_server_t* server);

void coap_server_debug_receive_request(coap_server_t* server, char* path, method_t m, transport_t t);

#endif //COAPI_SERVER_H
