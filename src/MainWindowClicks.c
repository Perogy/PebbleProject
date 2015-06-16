#include <pebble.h>
#include "Main.h"
#include "ErrorMessage.h"
#include "ErrorWindow.h"
#include "WindowData.h"
#include "MainWindowClicks.h"

//called when an item is selected
void menu_select_callback(ClickRecognizerRef recognizer, void *context)
{
    if (pageDepth == 1)
    {
        app_timer_cancel(textScrollTimer);
        scrollTextBackToStart();
        layer_mark_dirty(menu_layer_get_layer(myMenuLayer));
        MenuIndex currentIndex = menu_layer_get_selected_index(myMenuLayer);
        int currentRow = currentIndex.row;
        selectedProjectIndex = currentRow;
        sendProjectIDToPhone(currentRow);
        displayMessage("Loading...", 102);
    }
    if (pageDepth == 2)
    {
        //turn off input while saving
        window_set_click_config_provider(window, (ClickConfigProvider)0);
        MenuIndex currentIndex = menu_layer_get_selected_index(myMenuLayer);
        int currentRow = currentIndex.row;
        sendItemIDToPhone(currentRow);
    }
}

void up_click_handler(ClickRecognizerRef recognizer, void *context) 
{
    MenuIndex currentIndex = menu_layer_get_selected_index(myMenuLayer);
    if (currentIndex.row > 0)
    {
        app_timer_cancel(textScrollTimer);        
        //this will not work when it STARTS on a scrollable text field, needs to be fixed
        scrollTextBackToStart();
        currentIndex.row--;
        menu_layer_set_selected_index(myMenuLayer, currentIndex, MenuRowAlignCenter, true);
        WindowData* wd = window_get_user_data(window);
        textScrollTimer = app_timer_register(wd->config->scrollSpeed, timerTick, NULL);
    }
}

void down_click_handler(ClickRecognizerRef recognizer, void *context) 
{
    WindowData* wd = (WindowData*)window_get_user_data(window);
    MenuIndex currentIndex = menu_layer_get_selected_index(myMenuLayer);
    if (currentIndex.row < wd->projects->length)
    {
        app_timer_cancel(textScrollTimer);
        //scroll the text back to the beginning when unselecting
        scrollTextBackToStart();
        currentIndex.row++;
        menu_layer_set_selected_index(myMenuLayer, currentIndex, MenuRowAlignCenter, true);
        WindowData* wd = window_get_user_data(window);
        textScrollTimer = app_timer_register(wd->config->scrollSpeed, timerTick, NULL);
    }
}

void back_click_handler(ClickRecognizerRef recognizer, void *context) 
{
    WindowData* wd = (WindowData*)window_get_user_data(window);
    if (pageDepth != 1)
    {
        app_timer_cancel(textScrollTimer);
        scrollable = 0;
        destroyItemList(wd->items);
        pageDepth--;
        menu_layer_reload_data(myMenuLayer);
        MenuIndex mi;
        mi.row = 0;
        mi.section = 0;
        menu_layer_set_selected_index(myMenuLayer, mi, MenuRowAlignCenter, true);
        scrolledNumber = 0;
        textScrollTimer = app_timer_register(wd->config->scrollSpeed, timerTick, NULL);
    }
    else
    {
        window_stack_pop_all(1);
    }    
}