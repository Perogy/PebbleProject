#include <pebble.h>
#include "ErrorWindowClicks.h"
#include "Main.h"

//called when an item is selected
void error_select_callback(ClickRecognizerRef recognizer, void *context)
{

}

void error_up_click_handler(ClickRecognizerRef recognizer, void *context) 
{
}

void error_down_click_handler(ClickRecognizerRef recognizer, void *context) 
{
}

void error_back_click_handler(ClickRecognizerRef recognizer, void *context) 
{
    //pop the loading screen (or screens) off the window stack
    while (window_stack_get_top_window() != window)
    {
        window_stack_pop(1);
    }
    textScrollTimer = app_timer_register(TEXT_SCROLL_INTERVAL, timerTick, NULL);
}