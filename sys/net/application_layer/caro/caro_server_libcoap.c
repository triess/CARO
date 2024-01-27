//
// Created by nikok on 23.11.2023.
//

#include "net/caro/caro_server_libcoap.h"

#include <coap3/coap.h>

void _libcoap_start_server(struct server_driver_t* sd){
    (void)sd;
    printf("libcoap start server\n");

    // libcoap start
    coap_startup();
}

void _libcoap_register_resource(struct server_driver_t* sd, methods_selector_t ms, char* path, caro_resource_handler_t rh) {
    (void)sd;
    (void)ms;
    (void)path;
    (void)rh;
    printf("libcoap register resource\n");
}

void _libcoap_get_request_header_data(struct server_driver_t* sd, caro_request_t* request, const char** path_p, uint8_t* ver_t_tkl_p, uint16_t* id_p, method_t* m_p, transport_t* t_p, uint16_t* opt_len_p, uint16_t* pay_len_p){
    (void)sd;
    (void)request;
    (void)path_p;
    (void)ver_t_tkl_p;
    (void)id_p;
    (void)m_p;
    (void)t_p;
    (void)opt_len_p;
    (void)pay_len_p;
}

void _libcoap_get_request_opt_num(struct server_driver_t* sd, caro_request_t* request, uint32_t idx, uint16_t* num_p) {
    (void)sd;
    (void)request;
    (void)idx;
    (void)num_p;
}


void _libcoap_get_request_opt_as_uint(struct server_driver_t* sd, caro_request_t* request, uint32_t idx, uint32_t* val_p) {
    (void)sd;
    (void)request;
    (void)idx;
    (void)val_p;
}

void _libcoap_get_request_opt_as_str(struct server_driver_t* sd, caro_request_t* request, uint32_t idx, char* val_p, size_t max_len) {
    (void)sd;
    (void)request;
    (void)idx;
    (void)val_p;
    (void)max_len;
}

void _libcoap_get_request_payload(struct server_driver_t* sd, caro_request_t* request, const char** pay_p) {
    (void)sd;
    (void)request;
    (void)pay_p;
}

void _libcoap_initialize_response(struct server_driver_t* sd, caro_request_t* request, caro_response_t* response, uint8_t code) {
    (void)sd;
    (void)request;
    (void)response;
    (void)code;
}

void _libcoap_response_add_uint_option(struct server_driver_t* sd, caro_response_t* response, uint16_t opt_num, uint32_t opt_val) {
    (void)sd;
    (void)response;
    (void)opt_num;
    (void)opt_val;
}

void _libcoap_response_add_str_option(struct server_driver_t* sd, caro_response_t* response, uint16_t opt_num, const char* opt_val) {
    (void)sd;
    (void)response;
    (void)opt_num;
    (void)opt_val;
}

void _libcoap_response_add_payload(struct server_driver_t* sd, caro_response_t* response, uint8_t * payload, size_t pay_len) {
    (void)sd;
    (void)response;
    (void)payload;
    (void)pay_len;
}


server_driver_t libcoap_driver = {
        .start_server = &_libcoap_start_server,
        .register_resource = &_libcoap_register_resource,
        .get_request_header_data = &_libcoap_get_request_header_data,
        .get_request_opt_num = &_libcoap_get_request_opt_num,
        .get_request_opt_as_uint = &_libcoap_get_request_opt_as_uint,
        .get_request_opt_as_str = &_libcoap_get_request_opt_as_str,
        .get_request_payload = &_libcoap_get_request_payload,
        .initialize_response = &_libcoap_initialize_response,
        .response_add_str_option = &_libcoap_response_add_str_option,
        .response_add_uint_option = &_libcoap_response_add_uint_option,
        .response_add_payload = &_libcoap_response_add_payload
};