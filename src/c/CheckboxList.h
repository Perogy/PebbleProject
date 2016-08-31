#ifndef _CHECKBOXLIST_H_
#define _CHECKBOXLIST_H_

#include <pebble.h>

typedef struct layer
{
    Layer** checkboxLayer;
    GRect* checkboxFrame;
    GRect* checkboxRect;
    int* checked;
    int length;
} itemCheckboxLayer;

#endif