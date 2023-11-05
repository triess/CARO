//
// Created by sommer on 04.11.23.
//

#ifndef COAPI_LIBCOAP_DRIVER_H
#define COAPI_LIBCOAP_DRIVER_H

#include "../server_drivers.h"

void registerRuntimeResource(struct server_driver_t sd){};
void startServer(struct server_driver_t sd){};

void libcoap_start_server(struct server_driver_t sd);
void libcoap_register_resource(struct server_driver_t sd);


server_driver_t libcoap_driver = {
        .start_server = &libcoap_start_server,
        .register_resource = &libcoap_register_resource
};

#endif //COAPI_LIBCOAP_DRIVER_H
