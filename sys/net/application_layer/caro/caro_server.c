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

#include "net/caro.h"

#include "stdio.h"
#include "string.h"

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

    for (int i = 0; i < NUM_OF_RESOURCES; i++) {
        caro_resource_t resource = resource_pool[i];
        if (!resource.is_valid) continue;

        for (int j = 0; j < NUM_OF_TRANSPORTS; j++) {

            if ((resource.allowed_transports >> j) & 0x1) {
                if (server->drivers[j]) {
                    printf("Resource was delegated to driver\n");
                    server->drivers[j]->register_resource(server->drivers[j], resource.allowed_methods,
                                                          resource.path, resource.handler);
                } else {
                    printf("No driver was set up for the protocol\n");
                }
            }
        }
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


// DRIVER STUFF

#include "net/gcoap.h"

#define NUM_GCOAP_RESOURCES 10

static coap_resource_t _resources[NUM_GCOAP_RESOURCES];

static const char *_link_params[] = {
        ";ct=0;rt=\"count\";obs",
        NULL
};

static ssize_t _encode_link(const coap_resource_t *resource, char *buf,
                            size_t maxlen, coap_link_encoder_ctx_t *context) {
    ssize_t res = gcoap_encode_link(resource, buf, maxlen, context);
    if (res > 0) {
        if (_link_params[context->link_pos]
            && (strlen(_link_params[context->link_pos]) < (maxlen - res))) {
            if (buf) {
                memcpy(buf+res, _link_params[context->link_pos],
                       strlen(_link_params[context->link_pos]));
            }
            return res + strlen(_link_params[context->link_pos]);
        }
    }

    return res;
}

static gcoap_listener_t _listener = {
        &_resources[0],
        0,
        GCOAP_SOCKET_TYPE_UNDEF, // TODO: define sock types
        _encode_link,
        NULL,
        NULL
};


void _convert_gcoap_pkt_to_request(coap_pkt_t* pkt, caro_request_t* request,const char* path, method_t m, transport_t t){
    request->path = path;
    request->method = m;
    request->transport = t;
    request->ver_t_tkl = pkt->hdr->ver_t_tkl;
    request->id = pkt->hdr->id;
    for (int i = 0; i < pkt->options_len; i++) {
        coap_optpos_t opt = pkt->options[i];

        caro_message_option_t caro_opt;
        caro_opt.opt_num = opt.opt_num;

        coap_opt_get_uint(pkt, opt.opt_num, &caro_opt.int_value);
        // coap_opt_get_string(pkt, opt.opt_num, (uint8_t*)caro_opt.str_value, 255, '\0');

        request->options[i] = caro_opt;
    }
    request->options_len = pkt->options_len;
    request->payload = pkt->payload;
    request->payload_len = pkt->payload_len;
}

static ssize_t _gcoap_universal_udp_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len, coap_request_ctx_t *ctx)
{
    (void)pdu;
    (void)buf;
    (void)len;

    method_t method = coap_method2flag(coap_get_code_detail(pdu));

    printf("Method: %d\n", method);

    for (int i = 0; i < NUM_OF_RESOURCES; i++) {
        caro_resource_t r = resource_pool[i];
        if (!r.is_valid) continue;

        if (strcmp(r.path, ctx->resource->path) == 0 &&
            is_method_allowed(r.allowed_methods, method) &&
            is_transport_allowed(r.allowed_transports, UDP)) { // TODO: also compare the method
            printf("We found the corresponding handler!\n");

            caro_request_t request;
            //printf("TEST1: %p\n", (void*)&request);
            _convert_gcoap_pkt_to_request(pdu, &request, ctx->resource->path, method, UDP);
            //printf("TEST2: %p\n", (void*)&request);
            r.handler(&request);
        } else {
            printf("We did not find the corresponding handler!\n");
        }
    }

    printf("OUR GCOAP DEBUG HANDLER WAS CALLED\n");
    return 0;
}

static bool is_added = false;

void _gcoap_start_server(struct server_driver_t* sd){
    (void)sd;
    printf("backend call: starting gcoap server\n"); //TODO
}

void _gcoap_register_resource(struct server_driver_t* sd, methods_selector_t ms, char* path, caro_resource_handler_t rh) {
    (void)sd;
    (void)rh;
    printf("backend call: register resource gcoap to path: %s\n",path); //TODO

    coap_resource_t resource = { path, ms, &_gcoap_universal_udp_handler, NULL };
    _resources[_listener.resources_len] = resource;
    _listener.resources_len = _listener.resources_len + 1;

    if (!is_added) {
        gcoap_register_listener(&_listener);
        is_added = true;
    }
}

void _gcoap_initialize_response(struct server_driver_t* sd, response_t* response) {
    (void)sd;
    (void)response;
    printf("initialize response\n");
}

void _gcoap_add_str_option(struct server_driver_t* sd, uint16_t opt_num, char* opt_val) {
    (void)sd;
    (void)opt_num;
    (void)opt_val;
    printf("add string option\n");
}

void _gcoap_add_int_option(struct server_driver_t* sd, uint16_t opt_num, uint32_t opt_val) {
    (void)sd;
    (void)opt_num;
    (void)opt_val;
    printf("add int option\n");
}

void _gcoap_add_payload(struct server_driver_t* sd, uint8_t * payload) {
    (void)sd;
    (void)payload;
    printf("add payload\n");
}

server_driver_t gcoap_driver = {
        .start_server = &_gcoap_start_server,
        .register_resource = &_gcoap_register_resource,
        .initialize_response = &_gcoap_initialize_response,
        .add_str_option = &_gcoap_add_str_option,
        .add_int_option = &_gcoap_add_int_option,
        .add_payload = &_gcoap_add_payload
};
