//
// Created by sommer on 04.11.23.
//

#ifndef COAPI_UTILS_H
#define COAPI_UTILS_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    UDP = 1 << 0,
    UDPS = 1 << 1,
    TCP = 1 << 2,
    TCPS = 1 << 3,
    WS = 1 << 4,
    BLE = 1 << 5,
    OSCORE = 1 << 6,
    SLIPMUX = 1 << 7
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
    NUM_OF
} selected_implementations;

const int available_transport[NUM_OF] = {
#ifdef COAPI_USE_GCOAP
    TRANSPORT.UDP | TRANSPORT.UDPS,
#endif
#ifdef COAPI_USE_LIBCOAP
    TRANSPORT.UDP | TRANSPORT.UDPS | TRANSPORT.TCP | TRANSPORT.TCPS | TRANSPORT.WS,
#endif
#ifdef COAPI_USE_NANOCOAP
    TRANSPORT.UDP | TRANSPORT.UDPS,
#endif
};

static inline bool is_transport_allowed(transports_selector_t ts, transport_t t) {
    return (ts & t) != 0;
}

static inline bool is_method_allowed(methods_selector_t ms, method_t m) {
    return (ms & m) != 0;
}

#endif //COAPI_UTILS_H
