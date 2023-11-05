//
// Created by sommer on 04.11.23.
//

#ifndef SERVER_DRIVERS_H
#define SERVER_DRIVERS_H

#include <stdio.h>
#include "server_utils.h"
#include "../utils.h"

typedef struct server_driver_t {
    void (*start_server)(struct server_driver_t*);
    void (*register_resource)(struct server_driver_t*, methods_selector_t,char*,resource_handler_t);
} server_driver_t;
#endif //SERVER_DRIVERS_H