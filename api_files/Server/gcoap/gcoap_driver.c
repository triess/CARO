//
// Created by sommer on 04.11.23.
//

#include "gcoap_driver.h"


void gcoap_start_server(struct server_driver_t sd){
    printf("backend call: starting gcoap server"); //TODO
    

};
void register_resource(struct server_driver_t sd, methods_selector_t ms,char* path,resource_handler_t rh){
    printf("backend call: register resource gcoap to path: %s",path); //TODO
};

