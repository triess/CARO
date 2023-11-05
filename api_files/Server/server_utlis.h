//
// Created by sommer on 05.11.23.
//

#ifndef COAPI_SERVER_UTLIS_H
#define COAPI_SERVER_UTLIS_H

#include "../request.h"
#include "../response.h"

typedef response_t* (*resource_handler_t)(request_t*);

#endif //COAPI_SERVER_UTLIS_H
