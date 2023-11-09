/*
 * Copyright (C) 2023 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_caro caro
 * @ingroup     sys
 * @brief       Coap Api for Riot Os (CARO)
 *
 * @{
 *
 * @file
 *
 * @author      Niko Kirste <niko.kirste@mailbox.tu-dresden.de>
 */

#ifndef CARO_H
#define CARO_H

/* Add header includes here */

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"

/* Declare the API of the module */

#ifdef __cplusplus
}
#endif

#define COAPI_USE_GCOAP // TODO: move to application code / config

typedef enum {
    IDX_UDP = 0,
    IDX_UDPS,
    IDX_TCP,
    IDX_TCPS,
    IDX_WS,
    IDX_BLE,
    IDX_SLIPMUX,
    IDX_OSCORE,
    NUM_OF_TRANSPORTS,
} transport_idx_t; // TODO: add prefix to name

typedef enum {
    UDP = 1 << IDX_UDP,
    UDPS = 1 << IDX_UDPS,
    TCP = 1 << IDX_TCP,
    TCPS = 1 << IDX_TCPS,
    WS = 1 << IDX_WS,
    BLE = 1 << IDX_BLE,
    SLIPMUX = 1 << IDX_SLIPMUX,
    OSCORE = 1 << IDX_OSCORE,
} transport_t; // TODO: add prefix to name

typedef uint8_t transports_selector_t;

typedef enum {
    GET = 1 << 0,
    POST = 1 << 1,
    PUT = 1 << 2,
    DELETE = 1 << 3
} method_t; // TODO: add prefix to name

typedef uint8_t methods_selector_t;

typedef enum selected_implementations {
#ifdef COAPI_USE_GCOAP
    GCOAP,
#endif
#ifdef COAPI_USE_LIBCOAP
    LIBCOAP,
#endif
#ifdef COAPI_USE_NANOCOAP
    NANOCOAP,
#endif
    NUM_OF_IMPLEMENTATIONS,
} selected_implementations_t;

static const int available_transport[NUM_OF_IMPLEMENTATIONS] = {
#ifdef COAPI_USE_GCOAP
    UDP | UDPS,
#endif
#ifdef COAPI_USE_LIBCOAP
    UDP | UDPS | TCP | TCPS | WS,
#endif
#ifdef COAPI_USE_NANOCOAP
    UDP | UDPS,
#endif
};

static inline bool is_transport_allowed(transports_selector_t ts, transport_t t) {
    return (ts & t) != 0;
}

static inline bool is_method_allowed(methods_selector_t ms, method_t m) {
    return (ms & m) != 0;
}

typedef struct request_t{
//    client_driver_t driver; // TODO
    transport_t transport;
} request_t;

typedef struct response_t{
//     server_driver_t driver; // TODO
    transport_t transport;
} response_t;

typedef response_t* (*resource_handler_t)(request_t*);

typedef struct server_driver_t {
    void (*start_server)(struct server_driver_t*);
    void (*register_resource)(struct server_driver_t*, methods_selector_t, char*, resource_handler_t);
} server_driver_t;

typedef struct {
    char* path;
    char allowed_methods;
    char allowed_transports;
    char* link_params;
    resource_handler_t handler;
    bool is_valid;
} caro_coap_resource_t;

#define NUM_OF_RESOURCES 10

typedef struct {
    server_driver_t* drivers[NUM_OF_TRANSPORTS];
    caro_coap_resource_t resource_pool[NUM_OF_RESOURCES]; // TODO: memory management
} coap_server_t;

void coap_server_init(coap_server_t* server);
void coap_server_use_driver(coap_server_t* server, transport_t t, server_driver_t* driver);
void coap_server_register_resource(coap_server_t* server, char* path, methods_selector_t methods,
                                   transports_selector_t  transports, char* rt,
                                   resource_handler_t handler);
void coap_server_start(coap_server_t* server);

void coap_server_debug_receive_request(coap_server_t* server, char* path, method_t m, transport_t t);


// DRIVER STUFF

void gcoap_start_server(struct server_driver_t* sd);
void gcoap_register_resource(struct server_driver_t* sd, methods_selector_t methods, char* path, resource_handler_t h);

extern server_driver_t gcoap_driver;

#endif /* CARO_H */
/** @} */
