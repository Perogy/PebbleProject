#include <pebble.h>
#include "ErrorWindow.h"

void displayMessage(char* errorMessage, int type)
{   
    ErrorMessage* em = createErrorMessage(errorMessage, type);
    errorWindow = window_create();
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
    displayMessage("Loading timed out", 100);
}

void errorWindow_unload(Window* window)
{
    ErrorMessage* em = window_get_user_data(errorWindow);
    scroll_layer_destroy(errorScrollLayer);
    text_layer_destroy(errorTextLayer);
    app_timer_cancel(loadingTimeoutTimer);
    
    if (em->type == FATAL)
    {
        destroyErrorMessage(em);
        window_stack_pop_all(1);
    }

}
void errorWindow_load(Window* window)
{
    ErrorMessage* em = window_get_user_data(errorWindow);
    
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_frame(window_layer);
    GRect max_text_bounds = GRect(0, 0, bounds.size.w, 2000);
    
    errorScrollLayer = scroll_layer_create(bounds);

    //if the window is not a "loading" window
    if (em->type != LOADING)
    {
        scroll_layer_set_click_config_onto_window(errorScrollLayer, window);
    }
    else
    {
        //set a timeout on the loading window (may want to do this in javascript instead)
        loadingTimeoutTimer = app_timer_register(LOADING_TIMEOUT_INTERVAL, loadingScreenTimer, NULL);
    }

    errorTextLayer = text_layer_create(max_text_bounds);
    text_layer_set_text(errorTextLayer, em->message);

    text_layer_set_font(errorTextLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));

    GSize max_size = text_layer_get_content_size(errorTextLayer);
    text_layer_set_size(errorTextLayer, max_size);
    scroll_layer_set_content_size(errorScrollLayer, GSize(bounds.size.w, max_size.h + 10));

    scroll_layer_add_child(errorScrollLayer, text_layer_get_layer(errorTextLayer));

    layer_add_child(window_layer, scroll_layer_get_layer(errorScrollLayer));
}