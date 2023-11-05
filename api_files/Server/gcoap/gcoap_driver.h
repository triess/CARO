//
// Created by sommer on 04.11.23.
//

#ifndef GCOAP_DRIVER_H
#define GCOAP_DRIVER_H
#include "../server_drivers.h"
void registerRuntimeResource(struct server_driver_t sd){};
void startServer(struct server_driver_t sd){};

void gcoap_start_server(struct server_driver_t sd);
void gcoap_register_resource(struct server_driver_t sd, methods_selector_t methods, char* path, resource_handler_t h);


server_driver_t gcoap_driver = {
    .start_server = &gcoap_start_server,
    .register_resource = &gcoap_register_resource
};


#endif //GCOAP_DRIVER_H
