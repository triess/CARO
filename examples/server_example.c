//
// Created by sommer on 04.11.23.
//

#include <stdio.h>
#include "../api_files/Server/server.h"
#include "../api_files/Server/gcoap/gcoap_driver.h"

response_t* index_handler(request_t* request) {
    printf("Hello World\n");
    return NULL;
}

int main(int argc, char* argv[]) {
    coap_server_t server;
    coap_server_use_driver(&server, UDP, gcoap_driver);
    coap_server_register_resource(&server, "/", GET | POST, UDP | TCP,
                                  ";ct=0;rt=\"count\";obs", &index_handler);
    coap_server_start(&server);

    return 0;
}


// THIS IS DEBUGGING STUFF
// coap_server_debug_receive_request(&server, "/", GET, TCP);

