//
// Created by sommer on 04.11.23.
//

#include "../api_files/Server/server.h"

#include <stdio.h>

/*
CoapResponse indexHandler(CoapRequest req) {
    return createTextResponse("This is the index");
}
 */

void indexHandler(void) {
    printf("Hello World\n");
}

int main(int argc, char* argv[]) {
    CoapServer.registerResource("/", METHOD_GET | METHOD_POST, TRANSPORT_UDP | TRANSPORT_TCP, ";ct=0;rt=\"count\";obs", &indexHandler);
    CoapServer.start();

    // resource type (attribute)
    //


    // THIS IS DEBUGGING STUFF
    CoapServer.debugReceiveRequest("/", METHOD_GET, TRANSPORT_TLS);
}