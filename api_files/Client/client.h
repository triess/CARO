//
// Created by sommer on 05.11.23.
//

#ifndef COAPI_CLIENT_H
#define COAPI_CLIENT_H

#include "../utils.h"

typedef struct {

} coap_request_t;

typedef struct {
    char* payload;
} coap_response_t;

typedef struct {

} client_driver_t;

typedef enum {
    OPTN_CONTENT_FORMAT,
    OPTN_ACCEPT
} coap_option_t;

void coap_request_init(coap_request_t* request, char* uri, method_t m, client_driver_t);
void coap_request_add_options(coap_request_t* request, coap_option_t opt, char* val);
void coap_request_set_payload(coap_request_t* request, char* payload);
coap_response_t coap_request_send(coap_request_t* request);

extern client_driver_t gcoap_driver;

#endif //COAPI_CLIENT_H
