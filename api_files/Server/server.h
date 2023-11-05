//
// Created by sommer on 04.11.23.
//

#ifndef COAPI_SERVER_H
#define COAPI_SERVER_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>



typedef void (*ResourceHandler)(void);

struct CoapServer_t {
    void (*registerResource)(char*, char, char, char*, ResourceHandler);
    void (*start)(void);

    // TODO: delete
    void (*debugReceiveRequest)(char*, char, char);
};

struct Resource {
    char* path;
    char allowedMethods;
    char allowedTransports;
    char* linkParams;
    ResourceHandler handler;
    bool isValid;
};

typedef struct {
    coap_resource_t resourcePool[NUM_RESOURCES]; // TODO: change
} coap_server_t;

void coap_server_use_driver(coap_server_t* server, transport_t t, server_driver_t driver);
void coap_server_register_resource(coap_server_t* server, char* path, methods_selector_t methods,
                                   transports_selector_t  transports, char* rt, resource_handler_t handler);
void coap_server_start(coap_server_t* server);

void coap_server_debug_receive_request(coap_server_t* server, char* path, method_t m, transport_t t);

#endif //COAPI_SERVER_H
