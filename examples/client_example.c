//
// Created by sommer on 05.11.23.
//

#include "stdio.h"
#include "../api_files/Client/client.h"

int main(int argc, char* argv[]) {
    coap_request_t request;
    coap_request_init(&request, "coap+tcp://192.168.178.47/temp", PUT, gcoap_driver);
    coap_request_add_options(&request, OPTN_CONTENT_FORMAT, "text/plain");
    coap_request_set_payload(&request, "24°C");
    coap_response_t response = coap_request_send(&request);

    printf("Server Response: %s\n", response.payload);

    return 0;
}