#pragma once

#include <pebble.h>
#include "ErrorMessage.h"
    
#define LOADING_TIMEOUT_INTERVAL 108000000
    
//Error message types
//FATAL = exit program when window unloads
//NON_FATAL = show a message/warning but allow the user to back to the previous screen
//LOADING = message has no click provider, the only thing that will close it is completing whatever "task" it is doing
//          or timing out.
#define FATAL 100
#define NON_FATAL 101
#define LOADING 102

//Window* errorWindow;

void displayMessage(char* errorMessage, int type);
void loadingScreenTimer(void* data);
void errorWindow_unload(Window* window);
void errorWindow_load(Window* window);