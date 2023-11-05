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
        coap_resource_t* r = &server->resource_pool[i];
        if (!r->is_valid) {
            // found an unused resource place
            r->path = path;
            r->allowed_methods = methods;
            r->allowed_transports = transports;
            r->link_params = rt;
            r->handler = handler;
            r->is_valid = true;
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
        coap_resource_t r = server->resource_pool[i];
        if (r.is_valid && strcmp(path, r.path) == 0) {
            if (!is_transport_allowed(r.allowed_transports, t)) {
                printf("TRANSPORT %d NOT ALLOWED", t);
            } else if (!is_method_allowed(r.allowed_methods, m)) {
                printf("METHOD %d NOT ALLOWED", m);
            } else {
                // found a fitting handler
                r.handler(NULL);
            }
        }
    }
}