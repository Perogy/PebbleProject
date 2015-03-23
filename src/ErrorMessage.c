#include <pebble.h>
#include "ErrorMessage.h"

ErrorMessage* createErrorMessage(char* message, int type)
{
    ErrorMessage* em = (ErrorMessage*)malloc(sizeof(ErrorMessage));
    em->message = (char*)malloc(strlen(message) + 1);
    strcpy(em->message, message);
    em->type = type;
    return em;
}

void destroyErrorMessage(ErrorMessage* em)
{
    free(em->message);
    free(em);
}