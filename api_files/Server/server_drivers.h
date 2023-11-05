//
// Created by sommer on 04.11.23.
//

#ifndef SERVER_DRIVERS_H
#define SERVER_DRIVERS_H

#include <stdio.h>
#include "../utils.h"
#include "server_utlis.h"

typedef struct SERVER_DRIVER {
    void (*startServer)(struct SERVER_DRIVER);
    void (*registerRuntimeResource)(struct SERVER_DRIVER);
}SERVER_DRIVER;


void initWhatever() {
    printf("Hello World\n");
}

#endif //SERVER_DRIVERS_H