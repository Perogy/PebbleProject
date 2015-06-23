#include <pebble.h>
#include "ErrorWindow.h"
#include "ErrorWindowClicks.h"
#include "Main.h"

void displayMessage(char* errorMessage, int type)
{   
    TextLayer* errorTextLayer = 0;
    ScrollLayer* errorScrollLayer = 0;
    AppTimer* loadingTimeoutTimer = 0;
    ErrorMessage* em = createErrorMessage(errorMessage, type, errorScrollLayer, errorTextLayer, loadingTimeoutTimer);
    Window* errorWindow = window_create();

    window_set_window_handlers(errorWindow, (WindowHandlers) 
    {
        .load = errorWindow_load,
        .unload = errorWindow_unload,
    });
    window_set_user_data(errorWindow, em);

    window_stack_push(errorWindow, true);
}    

void loadingScreenTimer(void* data)
{
    while (window_stack_get_top_window() != window)
    {
        window_stack_pop(1);
    }
    displayMessage("Loading timed out. You may have lost connection, or have a poor connection to the internet.", 100);
}

void errorWindow_unload(Window* window)
{
    ErrorMessage* em = window_get_user_data(window);
    destroyErrorMessage(em);
}
void error_config_provider(Window *window) 
{
    window_single_click_subscribe(BUTTON_ID_BACK, error_back_click_handler);
    window_single_click_subscribe(BUTTON_ID_UP, error_up_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, error_down_click_handler);
    window_single_click_subscribe(BUTTON_ID_SELECT, error_select_callback);
}

void errorWindow_load(Window* window)
{   
    ErrorMessage* em = window_get_user_data(window);
    
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_frame(window_layer);
    //scroll layer is pretty useless at this size but we'll keep it in here in case we ever need to show a scrolling message in the future
    GRect max_text_bounds = GRect(0, 0, bounds.size.w, bounds.size.h);
    
    em->errorScrollLayer = scroll_layer_create(bounds);

    //if the window is not a "loading" window
    //if (em->type != LOADING)
   // {
        window_set_click_config_provider(window, (ClickConfigProvider) error_config_provider);
    //}
   // else
    //{
        //set a timeout on the loading window (may want to do this in javascript instead)
        //em->loadingTimeoutTimer = app_timer_register(LOADING_TIMEOUT_INTERVAL, loadingScreenTimer, NULL);
    //}
    
    if (em->type == LOADING)
        em->loadingTimeoutTimer = app_timer_register(LOADING_TIMEOUT_INTERVAL, loadingScreenTimer, NULL);

    em->errorTextLayer = text_layer_create(max_text_bounds);
    text_layer_set_text(em->errorTextLayer, em->message);
    text_layer_set_font(em->errorTextLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));

    text_layer_set_size(em->errorTextLayer, GSize(max_text_bounds.size.w, max_text_bounds.size.h));
    text_layer_set_text_alignment(em->errorTextLayer, GTextAlignmentCenter);
    scroll_layer_set_content_size(em->errorScrollLayer, GSize(bounds.size.w, bounds.size.h));
    
    #ifdef PBL_COLOR
        text_layer_set_background_color(em->errorTextLayer, GColorDarkCandyAppleRed);
        text_layer_set_text_color(em->errorTextLayer, GColorLightGray);
    #endif
    
    scroll_layer_add_child(em->errorScrollLayer, text_layer_get_layer(em->errorTextLayer));

    layer_add_child(window_layer, scroll_layer_get_layer(em->errorScrollLayer));
}