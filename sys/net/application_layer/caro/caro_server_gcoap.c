//
// Created by nikok on 23.11.2023.
//




// DRIVER STUFF

#include "net/caro/caro_server_gcoap.h"

#include "net/caro/caro_server.h"

#include <stddef.h>
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
    request->req_obj = pkt;

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

            caro_response_t response;
            response.buf = buf;
            response.buf_len = len;
            r.handler(&request, &response);

            // TODO: do we need a method to send a response?
            return response.res_len;

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

void _gcoap_get_request_header_data(struct server_driver_t* sd, caro_request_t* request, const char** path_p, uint8_t* ver_t_tkl_p, uint16_t* id_p, method_t* m_p, transport_t* t_p, uint16_t* opt_len_p, uint16_t* pay_len_p){
    (void)sd;

    if (path_p) *path_p = request->path;
    if (ver_t_tkl_p) *ver_t_tkl_p = ((coap_pkt_t*)request->req_obj)->hdr->ver_t_tkl;
    if (id_p) *id_p = ((coap_pkt_t*)request->req_obj)->hdr->id;
    if (m_p) *m_p = request->method;
    if (t_p) *t_p = request->transport;
    if (opt_len_p) *opt_len_p = ((coap_pkt_t*)request->req_obj)->options_len;
    if (pay_len_p) *pay_len_p = ((coap_pkt_t*)request->req_obj)->payload_len;
}

void _gcoap_get_request_opt_num(struct server_driver_t* sd, caro_request_t* request, uint32_t idx, uint16_t* num_p) {
    (void)sd;

    *num_p = ((coap_pkt_t*)request->req_obj)->options[idx].opt_num;
}


void _gcoap_get_request_opt_as_uint(struct server_driver_t* sd, caro_request_t* request, uint32_t idx, uint32_t* val_p) {
    uint16_t opt_num;
    sd->get_request_opt_num(sd, request, idx, &opt_num);
    coap_opt_get_uint(((coap_pkt_t*)request->req_obj), opt_num, val_p);
}

void _gcoap_get_request_opt_as_str(struct server_driver_t* sd, caro_request_t* request, uint32_t idx, char* val_p, size_t max_len) {
    uint16_t opt_num;
    sd->get_request_opt_num(sd, request, idx, &opt_num);
    coap_opt_get_string(((coap_pkt_t*)request->req_obj), opt_num, (uint8_t*)val_p, max_len, ' '); // TODO: check separator or switch implementation
}

void _gcoap_get_request_payload(struct server_driver_t* sd, caro_request_t* request, const char** pay_p) {
    (void)sd;

    *pay_p = (const char*)((coap_pkt_t*)request->req_obj)->payload;
}

void _gcoap_initialize_response(struct server_driver_t* sd, caro_request_t* request, caro_response_t* response, uint8_t code) {
    (void)sd;
    printf("initialize response\n");

    response->opts_finished = false;
    response->res_len = 0;

    response->res_obj = request->req_obj; // TODO: Does this even make sense?
    gcoap_resp_init(response->res_obj, response->buf, response->buf_len, code);
}

void _gcoap_response_add_uint_option(struct server_driver_t* sd, caro_response_t* response, uint16_t opt_num, uint32_t opt_val) {
    (void)sd;
    (void)opt_num;
    (void)opt_val;
    printf("add int option\n");

    coap_opt_add_uint(response->res_obj, opt_num, opt_val);
}

void _gcoap_response_add_str_option(struct server_driver_t* sd, caro_response_t* response, uint16_t opt_num, const char* opt_val) {
    (void)sd;
    (void)response;
    (void)opt_num;
    (void)opt_val;
    printf("add string option\n");
}

void _gcoap_response_add_payload(struct server_driver_t* sd, caro_response_t* response, uint8_t * payload, size_t pay_len) {
    (void)sd;
    (void)response;
    (void)payload;
    printf("add payload\n");

    if (!response->opts_finished) {
        response->res_len = coap_opt_finish(response->res_obj, COAP_OPT_FINISH_PAYLOAD);
        response->opts_finished = true;
    }

    // TODO: check buf size with pay_len
    memcpy(((coap_pkt_t*)response->res_obj)->payload, payload, pay_len);
    response->res_len += pay_len;
}


server_driver_t gcoap_driver = {
        .start_server = &_gcoap_start_server,
        .register_resource = &_gcoap_register_resource,
        .get_request_header_data = &_gcoap_get_request_header_data,
        .get_request_opt_num = &_gcoap_get_request_opt_num,
        .get_request_opt_as_uint = &_gcoap_get_request_opt_as_uint,
        .get_request_opt_as_str = &_gcoap_get_request_opt_as_str,
        .get_request_payload = &_gcoap_get_request_payload,
        .initialize_response = &_gcoap_initialize_response,
        .response_add_str_option = &_gcoap_response_add_str_option,
        .response_add_uint_option = &_gcoap_response_add_uint_option,
        .response_add_payload = &_gcoap_response_add_payload
};