//
// Created by sommer on 04.11.23.
//
#include "server.h"

void coap_server_use_driver(coap_server_t* server, transport_t t, server_driver_t driver) {
    // TODO: map server to transport
}

void coap_register_resource(coap_server_t* server, char* path, char allowedMethods, char allowedTransports, char* linkParams, resource_handler_t handler) {
    printf("Register Resource\n");

    for (int i = 0; i < NUM_RESOURCES; i++) {
        coap_resource_t* r = &server->resourcePool[i];
        if (!r->isValid) {
            // found an unused resource place
            r->path = path;
            r->allowedMethods = allowedMethods;
            r->allowedTransports = allowedTransports;
            r->linkParams = linkParams;
            r->handler = handler;
            r->isValid = true;
            return;
        }
    }

    // no place for a new resource found
    // TODO: fail
}

void coap_start_server(coap_server_t* server) {
    printf("Start Server\n");
}

void coap_debug_receive_request(coap_server_t* server, char* path, char method, char transport) {
    printf("Received Request: %s, %d\n", path, method);

    for (int i = 0; i < NUM_RESOURCES; i++) {
        coap_resource_t r = server->resourcePool[i];
        if (r.isValid && strcmp(path, r.path) == 0) {
            if (!is_transport_allowed(r.allowedTransports, transport)) {
                printf("TRANSPORT %d NOT ALLOWED", transport);
            } else if (!is_method_allowed(r.allowedMethods, method)) {
                printf("METHOD %d NOT ALLOWED", method);
            } else {
                // found a fitting handler
                r.handler();
            }
        }
    }
}