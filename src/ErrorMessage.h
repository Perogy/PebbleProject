#ifndef _ERRORMESSAGE_H_
#define _ERRORMESSAGE_H_

#include <pebble.h>

typedef struct errorMsg
{
    char* message;
    int type;
    ScrollLayer* errorScrollLayer;
    TextLayer* errorTextLayer;
    AppTimer* loadingTimeoutTimer;
} ErrorMessage;

ErrorMessage* createErrorMessage(char* message, int type, ScrollLayer* errorScrollLayer, TextLayer* errorTextLayer, AppTimer* loadingTimeoutTimer);
void destroyErrorMessage(ErrorMessage* em);


#endif