//
// Created by sommer on 04.11.23.
//
#ifndef COAPI_REQUEST_H
#define COAPI_REQUEST_H

#include "Server/server_drivers.h"
#include <stdint.h>
typedef struct REQUEST{
    DRIVER driver;
    TRANSPORT transport;
}REQUEST;
#endif //COAPI_REQUEST_H
