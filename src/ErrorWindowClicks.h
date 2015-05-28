#ifndef _ERRORWINDOWCLICKS_H_
#define _ERRORWINDOWCLICKS_H_

#include <pebble.h>

//called when an item is selected
void error_select_callback(ClickRecognizerRef recognizer, void *context);
void error_up_click_handler(ClickRecognizerRef recognizer, void *context);
void error_down_click_handler(ClickRecognizerRef recognizer, void *context);
void error_back_click_handler(ClickRecognizerRef recognizer, void *context);

#endif