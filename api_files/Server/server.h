//
// Created by sommer on 04.11.23.
//

#ifndef COAPI_SERVER_H
#define COAPI_SERVER_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define METHOD_GET 1
#define METHOD_POST 1 << 1
#define METHOD_PUT 1 << 2
#define METHOD_DELETE 1 << 3

#define METHOD_ALLOWED(a, b) ((a & b) != 0)

#define TRANSPORT_UDP 1
#define TRANSPORT_DTLS 1 << 1
#define TRANSPORT_TCP 1 << 2
#define TRANSPORT_TLS 1 << 3

#define TRANSPORT_ALLOWED(a, b) ((a & b) != 0)

typedef void (*ResourceHandler)(void);

struct CoapServer_t {
    void (*registerResource)(char*, char, char, char*, ResourceHandler);
    void (*start)(void);

    // TODO: delete
    void (*debugReceiveRequest)(char*, char, char);
};

struct Resource {
    char* path;
    char allowedMethods;
    char allowedTransports;
    char* linkParams;
    ResourceHandler handler;
    bool isValid;
};

#define NUM_RESOURCES 10
struct Resource resourcePool[NUM_RESOURCES]; // TODO: change

void coap_register_resource(char* path, char allowedMethods, char allowedTransports, char* linkParams, ResourceHandler handler) {
    printf("Register Resource\n");

    for (int i = 0; i < NUM_RESOURCES; i++) {
        struct Resource* r = &resourcePool[i];
        if (!r->isValid) {
            // found an unused resource place
            r->path = path;
            r->allowedMethods = allowedMethods;
            r->allowedTransports = allowedTransports;
            r->linkParams = linkParams;
            r->handler = handler;
            r->isValid = true;
            return;
        }
    }

    // no place for a new resource found
    // TODO: fail
}

void coap_start_server() {
    printf("Start Server\n");
}

void coap_debug_receive_request(char* path, char method, char transport) {
    printf("Received Request: %s, %d\n", path, method);

    for (int i = 0; i < NUM_RESOURCES; i++) {
        struct Resource r = resourcePool[i];
        if (r.isValid && strcmp(path, r.path) == 0) {
            if (!TRANSPORT_ALLOWED(r.allowedTransports, transport)) {
                printf("TRANSPORT %d NOT ALLOWED", transport);
            } else if (!METHOD_ALLOWED(r.allowedMethods, method)) {
                printf("METHOD %d NOT ALLOWED", method);
            } else {
                // found a fitting handler
                r.handler();
            }
        }
    }
}

struct CoapServer_t CoapServer = {
        .registerResource = &coap_register_resource,
        .start = &coap_start_server,

        // TODO: delete
        .debugReceiveRequest = &coap_debug_receive_request
};

#endif //COAPI_SERVER_H
