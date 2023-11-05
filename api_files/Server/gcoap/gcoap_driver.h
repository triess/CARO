//
// Created by sommer on 04.11.23.
//

#ifndef GCOAP_DRIVER_H
#define GCOAP_DRIVER_H

#include "../server_drivers.h"
#include "../server_utils.h"

void gcoap_start_server(struct server_driver_t* sd);
void gcoap_register_resource(struct server_driver_t* sd, methods_selector_t methods, char* path, resource_handler_t h);


extern server_driver_t gcoap_driver;


#endif //GCOAP_DRIVER_H
