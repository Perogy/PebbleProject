#include <pebble.h>
#include "ErrorWindow.h"
#include "ErrorWindowClicks.h"
#include "Main.h"

void displayMessage(char* errorMessage, int type)
{
    //clear all other windows off the stack except main window
    while (window_stack_get_top_window() != window)
    {
        window_stack_pop(1);
    }
    TextLayer* errorTextLayer = 0;
    ScrollLayer* errorScrollLayer = 0;
    AppTimer* loadingTimeoutTimer = 0;
    ErrorMessage* em = createErrorMessage(errorMessage, type, errorScrollLayer, errorTextLayer, loadingTimeoutTimer);
    Window* errorWindow = window_create();
    window_set_user_data(errorWindow, em);
    window_set_window_handlers(errorWindow, (WindowHandlers) 
    {
        .load = errorWindow_load,
        .unload = errorWindow_unload,
    });
    

    window_stack_push(errorWindow, true);
}    

void loadingScreenTimer(void* data)
{
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
    //window_single_click_subscribe(BUTTON_ID_UP, error_up_click_handler);
    //window_single_click_subscribe(BUTTON_ID_DOWN, error_down_click_handler);
    //window_single_click_subscribe(BUTTON_ID_SELECT, error_select_callback);
}

void errorWindow_load(Window* window)
{   
    
    ErrorMessage* em = window_get_user_data(window);
    
    Layer *window_layer = window_get_root_layer(window);
    
    ScrollLayerCallbacks callbacks = 
        {
            .click_config_provider = (ClickConfigProvider) error_config_provider
        };
    
    //bounds of the window
    GRect bounds = layer_get_frame(window_layer);
    
    //maximum amount of scrollable text content (the 5000 will be resized to the actual size of the content later)
    GRect max_text_bounds = GRect(0, 0, bounds.size.w, 5000);
    
    GSize textSize = graphics_text_layout_get_content_size(em->message, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), max_text_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft);
    
    //setting the text content size to the actual size of the content now that we know what it is
    if (textSize.h > bounds.size.h)
        //add 4 for a bit of extra margin at the bottom
        max_text_bounds.size.h = textSize.h + 4;
    else
        max_text_bounds.size.h = bounds.size.h;
    
    //should be set to the size the scroll layer actually takes up
    em->errorScrollLayer = scroll_layer_create(bounds);
    em->errorTextLayer = text_layer_create(max_text_bounds);
    
    //by setting the click provider through set callbacks it allows you to keep the original scrolling functionality while
    //allowing you to add back or select button functionality.
    scroll_layer_set_callbacks(em->errorScrollLayer, callbacks);
    scroll_layer_set_click_config_onto_window(em->errorScrollLayer, window);
    
    if (em->type == LOADING)
    {
        em->loadingTimeoutTimer = app_timer_register(LOADING_TIMEOUT_INTERVAL, loadingScreenTimer, NULL);
        text_layer_set_text_alignment(em->errorTextLayer, GTextAlignmentCenter);
    }
    else
    {
        #ifdef PBL_ROUND
            text_layer_set_text_alignment(em->errorTextLayer, GTextAlignmentCenter);
        #else
            text_layer_set_text_alignment(em->errorTextLayer, GTextAlignmentLeft);
        #endif
    }
    
    text_layer_set_text(em->errorTextLayer, em->message);
    text_layer_set_font(em->errorTextLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
    
    //total size of the content
    scroll_layer_set_content_size(em->errorScrollLayer, GSize(bounds.size.w, max_text_bounds.size.h));
    
    #ifdef PBL_COLOR
        text_layer_set_background_color(em->errorTextLayer, GColorDarkCandyAppleRed);
        text_layer_set_text_color(em->errorTextLayer, GColorLightGray);
    #endif
    
    scroll_layer_add_child(em->errorScrollLayer, text_layer_get_layer(em->errorTextLayer));

    layer_add_child(window_layer, scroll_layer_get_layer(em->errorScrollLayer));
    
    #ifdef PBL_ROUND
        text_layer_enable_screen_text_flow_and_paging(em->errorTextLayer, 2);
    #endif
}