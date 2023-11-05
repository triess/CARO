//
// Created by sommer on 04.11.23.
//
#include "server.h"

void coap_server_use_driver(coap_server_t* server, transport_t t, server_driver_t driver) {
    // TODO: map server to transport
}

void coap_server_register_resource(coap_server_t* server, char* path, methods_selector_t methods,
                                   transports_selector_t  transports, char* rt, resource_handler_t handler) {
    printf("Register Resource\n");

    for (int i = 0; i < NUM_RESOURCES; i++) {
        coap_resource_t* r = &server->resourcePool[i];
        if (!r->isValid) {
            // found an unused resource place
            r->path = path;
            r->allowedMethods = methods;
            r->allowedTransports = transports;
            r->linkParams = rt;
            r->handler = handler;
            r->isValid = true;
            return;
        }
    }

    // no place for a new resource found
    // TODO: fail
}

void coap_server_start(coap_server_t* server) {
    printf("Start Server\n");
}

void coap_server_debug_receive_request(coap_server_t* server, char* path, method_t m, transport_t t) {
    printf("Received Request: %s, %d\n", path, m);

    for (int i = 0; i < NUM_RESOURCES; i++) {
        coap_resource_t r = server->resourcePool[i];
        if (r.isValid && strcmp(path, r.path) == 0) {
            if (!is_transport_allowed(r.allowedTransports, t)) {
                printf("TRANSPORT %d NOT ALLOWED", t);
            } else if (!is_method_allowed(r.allowedMethods, m)) {
                printf("METHOD %d NOT ALLOWED", m);
            } else {
                // found a fitting handler
                r.handler(NULL);
            }
        }
    }
}