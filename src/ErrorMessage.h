#ifndef _ERRORMESSAGE_H_
#define _ERRORMESSAGE_H_

#include <pebble.h>

typedef struct errorMsg
{
    char* message;
    int type;
} ErrorMessage;

ErrorMessage* createErrorMessage(char* message, int type);
void destroyErrorMessage(ErrorMessage* em);


#endif