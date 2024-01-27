/*
 * Copyright (C) 2023 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     module_caro
 * @{
 *
 * @file
 * @brief       caro implementation
 *
 * @author      Niko Kirste <niko.kirste@mailbox.tu-dresden.de>
 *
 * @}
 */

#include "net/caro/caro_server.h"

#include <stdio.h>
#include <string.h>

/* Implementation of the module */

// TODO: move or refactor
int transport_to_index(transport_t t) {
    switch (t) {
        case UDP: return IDX_UDP;
        case UDPS: return IDX_UDPS;
        case TCP: return IDX_TCP;
        case TCPS: return IDX_TCPS;
        case WS: return IDX_WS;
        case BLE: return IDX_BLE;
        case SLIPMUX: return IDX_SLIPMUX;
        case OSCORE: return IDX_OSCORE;
        default: return -1;
    }
}


caro_resource_t resource_pool[NUM_OF_RESOURCES]; // TODO: memory management


void caro_server_init(caro_server_t* server) {
    memset(server, 0, sizeof(*server));
}

void caro_server_use_driver(caro_server_t* server, transport_t t, server_driver_t* driver) {
    printf("Debug\n");
    // TODO: map server to transport
    int transport_index = transport_to_index(t);
    printf("Insert Driver at %d %p\n", transport_index, (void*)driver);
    server->drivers[transport_index] = driver;
}

// user visible function
void caro_server_register_resource(caro_server_t* server, char* path, methods_selector_t methods,
                                   transports_selector_t  transports, char* rt, caro_resource_handler_t handler) {
    printf("Register Resource\n");

    for (int i = 0; i < NUM_OF_RESOURCES; i++) {
        caro_resource_t* r = &resource_pool[i];
        if (!r->is_valid) {
            // found an unused resource place
            r->path = path;
            r->allowed_methods = methods;
            r->allowed_transports = transports;
            r->link_params = rt;
            r->handler = handler;
            r->server = server;
            r->is_valid = true;

            return;
        }
    }

    // no place for a new resource found
    // TODO: fail
}

// user visible function
void caro_server_start(caro_server_t* server) {
    printf("Start Server\n");

    server_driver_t* used_drivers[NUM_OF_RESOURCES]; // TODO: improve memory management
    memset(used_drivers, 0, NUM_OF_RESOURCES * sizeof(server_driver_t*));

    for (int i = 0; i < NUM_OF_RESOURCES; i++) {
        caro_resource_t resource = resource_pool[i];
        if (!resource.is_valid) continue;

        for (int j = 0; j < NUM_OF_TRANSPORTS; j++) {

            if ((resource.allowed_transports >> j) & 0x1) {
                if (server->drivers[j]) {
                    printf("Resource was delegated to driver\n");
                    server->drivers[j]->register_resource(server->drivers[j], resource.allowed_methods,
                                                          resource.path, resource.handler);

                    for (int k = 0; k < NUM_OF_RESOURCES; k++) {
                        if (!used_drivers[k]) {
                            used_drivers[k] = server->drivers[j];
                            break;
                        } else if (used_drivers[k] == server->drivers[j]) {
                            break; // do not add twice
                        }
                    }
                } else {
                    printf("No driver was set up for the protocol\n");
                }
            }
        }
    }

    for (int i = 0; i < NUM_OF_RESOURCES; i++) {
        if (!used_drivers[i]) break;
        used_drivers[i]->start_server(used_drivers[i]);
    }
}

// user visible function for now TODO
void caro_server_debug_receive_request(caro_server_t* server, char* path, method_t m, transport_t t) {
    (void)server;

    printf("Received Request: %s, %d\n", path, m);
    for (int i = 0; i < NUM_OF_RESOURCES; i++) {
        caro_resource_t r = resource_pool[i];
        if (r.is_valid && strcmp(path, r.path) == 0) {
            if (!is_transport_allowed(r.allowed_transports, t)) {
                printf("TRANSPORT %d NOT ALLOWED", t);
            } else if (!is_method_allowed(r.allowed_methods, m)) {
                printf("METHOD %d NOT ALLOWED", m);
            } else {
                // found a fitting handler

                /*
                caro_request_t request;
                r.handler(&request);
                 */
            }
            break;
        }
    }
}