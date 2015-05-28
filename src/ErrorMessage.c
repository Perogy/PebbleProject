#include <pebble.h>
#include "ErrorMessage.h"

ErrorMessage* createErrorMessage(char* message, int type, ScrollLayer* errorScrollLayer, TextLayer* errorTextLayer, AppTimer* loadingTimeoutTimer)
{
    ErrorMessage* em = (ErrorMessage*)malloc(sizeof(ErrorMessage));
    em->message = (char*)malloc(strlen(message) + 1);
    strcpy(em->message, message);
    em->type = type;
    em->errorScrollLayer = errorScrollLayer;
    em->errorTextLayer = errorTextLayer;
    em->loadingTimeoutTimer = loadingTimeoutTimer;
    return em;
}

void destroyErrorMessage(ErrorMessage* em)
{
    free(em->message);
    scroll_layer_destroy(em->errorScrollLayer);
    text_layer_destroy(em->errorTextLayer);
    if (em->type == 102)
        app_timer_cancel(em->loadingTimeoutTimer);
    free(em);
}