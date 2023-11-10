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


void _convert_gcoap_pkt_to_request(struct server_driver_t* driver, coap_pkt_t* pkt, caro_request_t* request, const char* path, method_t m, transport_t t){
    request->path = path;
    request->method = m;
    request->transport = t;
    request->gcoap_req = pkt;

    request->driver = driver;
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
            // TODO: check if it's ok to just pass the gcoap_driver
            _convert_gcoap_pkt_to_request(&gcoap_driver, pdu, &request, ctx->resource->path, method, UDP);
            //printf("TEST2: %p\n", (void*)&request);
            r.handler(&request);

            break;
        } else {
            printf("We did not find the corresponding handler!\n");
        }
    }

    // printf("OUR GCOAP DEBUG HANDLER WAS CALLED\n");
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

void _gcoap_get_request_header_data(struct server_driver_t* sd, caro_request_t* request, const char** path_p, uint8_t* ver_t_tkl_p, uint16_t* id_p, method_t* m_p, transport_t* t_p, uint16_t* opt_len_p, uint16_t* pay_len_p){
    (void)sd;

    if (path_p) *path_p = request->path;
    if (ver_t_tkl_p) *ver_t_tkl_p = request->gcoap_req->hdr->ver_t_tkl;
    if (id_p) *id_p = request->gcoap_req->hdr->id;
    if (m_p) *m_p = request->method;
    if (t_p) *t_p = request->transport;
    if (opt_len_p) *opt_len_p = request->gcoap_req->options_len;
    if (pay_len_p) *pay_len_p = request->gcoap_req->payload_len;
}

void _gcoap_get_request_opt_num(struct server_driver_t* sd, caro_request_t* request, uint32_t idx, uint16_t* num_p) {
    (void)sd;

    *num_p = request->gcoap_req->options[idx].opt_num;
}


void _gcoap_get_request_opt_as_uint(struct server_driver_t* sd, caro_request_t* request, uint32_t idx, uint32_t* val_p) {
    uint16_t opt_num;
    sd->get_request_opt_num(sd, request, idx, &opt_num);
    coap_opt_get_uint(request->gcoap_req, opt_num, val_p);
}

void _gcoap_get_request_opt_as_str(struct server_driver_t* sd, caro_request_t* request, uint32_t idx, char* val_p, size_t max_len) {
    uint16_t opt_num;
    sd->get_request_opt_num(sd, request, idx, &opt_num);
    coap_opt_get_string(request->gcoap_req, opt_num, (uint8_t*)val_p, max_len, ' '); // TODO: check separator or switch implementation
}

void _gcoap_get_request_payload(struct server_driver_t* sd, caro_request_t* request, const char** pay_p) {
    (void)sd;

    *pay_p = (const char*)request->gcoap_req->payload;
}

server_driver_t gcoap_driver = {
        .start_server = &_gcoap_start_server,
        .register_resource = &_gcoap_register_resource,
        .initialize_response = &_gcoap_initialize_response,
        .add_str_option = &_gcoap_add_str_option,
        .add_int_option = &_gcoap_add_int_option,
        .add_payload = &_gcoap_add_payload,
        .get_request_header_data = &_gcoap_get_request_header_data,
        .get_request_opt_num = &_gcoap_get_request_opt_num,
        .get_request_opt_as_uint = &_gcoap_get_request_opt_as_uint,
        .get_request_opt_as_str = &_gcoap_get_request_opt_as_str,
        .get_request_payload = &_gcoap_get_request_payload
};
