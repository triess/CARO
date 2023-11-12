//
// Created by nikok on 09.11.2023.
//

#ifndef RIOT_CARO_SERVER_H
#define RIOT_CARO_SERVER_H

/* Add header includes here */

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"

#include "net/gcoap.h"

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
    GET_IDX = 0,
    POST_IDX,
    PUT_IDX,
    DELETE_IDX
} method_idx_t; // TODO: add prefix to name

typedef enum {
    GET = 1 << GET_IDX,
    POST = 1 << POST_IDX,
    PUT = 1 << PUT_IDX,
    DELETE = 1 << DELETE_IDX
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

typedef struct caro_request_t {
    const char* path;
    method_t method;
    transport_t transport;

    coap_pkt_t* gcoap_req;

    struct server_driver_t* driver;
} caro_request_t;

typedef struct response_t {
    uint8_t *buf;
    size_t buf_len;
    bool opts_finished;
    size_t res_len;

    coap_pkt_t* gcoap_res;

    struct server_driver_t* driver;
} caro_response_t;

typedef bool (*caro_resource_handler_t)(caro_request_t*, caro_response_t*);

typedef struct {
    char* path;
    methods_selector_t method;
    transports_selector_t transport;
    caro_resource_handler_t resource_handler;
} handler_table_entry;

typedef struct server_driver_t {
    void (*start_server)(struct server_driver_t*);
    void (*register_resource)(struct server_driver_t*, methods_selector_t, char*, caro_resource_handler_t);
    void (*get_request_header_data)(struct server_driver_t*, caro_request_t*, const char**, uint8_t*, uint16_t*, method_t*, transport_t*, uint16_t*, uint16_t*);
    void (*get_request_opt_num)(struct server_driver_t*, caro_request_t*, uint32_t, uint16_t*);
    void (*get_request_opt_as_uint)(struct server_driver_t*, caro_request_t*, uint32_t, uint32_t*);
    void (*get_request_opt_as_str)(struct server_driver_t*, caro_request_t*, uint32_t, char*, size_t max_len);
    void (*get_request_payload)(struct server_driver_t*, caro_request_t*, const char**);
    void (*initialize_response)(struct server_driver_t*, caro_request_t*, caro_response_t*, uint8_t);
    void (*response_add_uint_option)(struct server_driver_t*, caro_response_t*, uint16_t,uint32_t);
    void (*response_add_str_option)(struct server_driver_t*, caro_response_t*, uint16_t,const char*);
    void (*response_add_payload)(struct server_driver_t*, caro_response_t*, uint8_t*, size_t);
} server_driver_t;

typedef struct {
    server_driver_t* drivers[NUM_OF_TRANSPORTS];
} caro_server_t;

typedef struct {
    char* path;
    methods_selector_t allowed_methods;
    transports_selector_t allowed_transports;
    char* link_params;
    caro_resource_handler_t handler;
    caro_server_t* server;
    bool is_valid;
} caro_resource_t;

#define NUM_OF_RESOURCES 10
extern caro_resource_t resource_pool[NUM_OF_RESOURCES]; // TODO: memory management

void caro_server_init(caro_server_t* server);
void caro_server_use_driver(caro_server_t* server, transport_t t, server_driver_t* driver);
void caro_server_register_resource(caro_server_t* server, char* path, methods_selector_t methods,
                                   transports_selector_t  transports, char* rt,
                                   caro_resource_handler_t handler);
void caro_server_start(caro_server_t* server);

void caro_server_debug_receive_request(caro_server_t* server, char* path, method_t m, transport_t t);

// DRIVER STUFF


extern server_driver_t gcoap_driver;

#endif //RIOT_CARO_SERVER_H
