//
// Created by sommer on 04.11.23.
//

#include "gcoap_driver.h"


void gcoap_start_server(struct server_driver_t sd){
    printf("backend call: starting gcoap server"); //TODO

    // from the example in gcoap in RIOT
    /* CoAP resources. Must be sorted by path (ASCII order). */
    static const coap_resource_t _resources[] = {
            { "/cli/stats", COAP_GET | COAP_PUT, _stats_handler, NULL },
            { "/riot/board", COAP_GET, _riot_board_handler, NULL },
    };

    static gcoap_listener_t _listener = {
            &_resources[0],
            ARRAY_SIZE(_resources),
            GCOAP_SOCKET_TYPE_UNDEF,
            _encode_link,
            NULL,
            NULL
    };



};
void register_resource(struct server_driver_t sd, methods_selector_t ms,char* path,resource_handler_t rh){
    printf("backend call: register resource gcoap to path: %s",path); //TODO
};

