//
// Created by sommer on 04.11.23.
//

//#if USE_GCOAP

#include "gcoap_driver.h"

void gcoap_start_server(struct server_driver_t* sd){
    printf("backend call: starting gcoap server\n"); //TODO


};
void gcoap_register_resource(struct server_driver_t* sd, methods_selector_t ms,char* path,resource_handler_t rh){
    printf("backend call: register resource gcoap to path: %s\n",path); //TODO
};

server_driver_t gcoap_driver = {
        .start_server = &gcoap_start_server,
        .register_resource = &gcoap_register_resource
};

