//
// Created by sommer on 04.11.23.
//

#ifndef COAPI_DRIVERS_H
#define COAPI_DRIVERS_H

#include <stdio.h>
#include "request.h"

typedef struct CLIENT_DRIVER {
    REQUEST (*initRequest)(struct CLIENT_DRIVER,char*);
    void (*addOption)(void);
    void (*setPayload)(void);
    void (*sendRequest)(void);
}CLIENT_DRIVER;

typedef enum TRANSPORT{
    UDP,UDPS,TCP,TCPS,WS,BLE,OSCORE
}TRANSPORT;

void initWhatever() {
    printf("Hello World\n");
}

#endif //COAPI_DRIVERS_H
#define GCOAP
#ifdef GCOAP
#include "Client/gcoap/gcoap_handler.h"
struct CLIENT_DRIVER GCOAP_DRIVER = {
        .initRequest = null
};
#endif