//
// Created by sommer on 04.11.23.
//

#ifndef COAPI_DRIVERS_H
#define COAPI_DRIVERS_H

#include <stdio.h>
#include "../request.h"

typedef struct client_driver_t {
    request_t (*initRequest)(struct client_driver_t,char*);
    void (*addOption)(void);
    void (*setPayload)(void);
    void (*sendRequest)(void);
}client_driver_t;


void initWhatever() {
    printf("Hello World\n");
}

#endif //COAPI_DRIVERS_H