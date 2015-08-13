#include <pebble.h>
#include "Main.h"
#include "ErrorMessage.h"
#include "ErrorWindow.h"
#include "WindowData.h"
#include "MainWindowClicks.h"
#include "Messaging.h"

//called when an item is selected
void menu_select_callback(ClickRecognizerRef recognizer, void *context)
{
    WindowData* wd = (WindowData*)window_get_user_data(window);
    if (wd->currentPage == 1)
    {
        app_timer_cancel(wd->textScrollTimer);
        scrollTextBackToStart();
        layer_mark_dirty(menu_layer_get_layer(myMenuLayer));
        MenuIndex currentIndex = menu_layer_get_selected_index(myMenuLayer);
        int currentRow = currentIndex.row;
        wd->selectedProjectIndex = currentRow;
        sendProjectIDToPhone(currentRow);
        displayMessage("Loading...", 102);
    }
    if (wd->currentPage == 2)
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
    WindowData* wd = (WindowData*)window_get_user_data(window);
    MenuIndex currentIndex = menu_layer_get_selected_index(myMenuLayer);
    if (currentIndex.row > 0)
    {
        app_timer_cancel(wd->textScrollTimer);        
        //this will not work when it STARTS on a scrollable text field, needs to be fixed
        scrollTextBackToStart();
        currentIndex.row--;
        menu_layer_set_selected_index(myMenuLayer, currentIndex, MenuRowAlignCenter, true);
        WindowData* wd = window_get_user_data(window);
        wd->textScrollTimer = app_timer_register(wd->config->scrollSpeed, timerTick, NULL);
    }
}

void down_click_handler(ClickRecognizerRef recognizer, void *context) 
{
    WindowData* wd = (WindowData*)window_get_user_data(window);
    MenuIndex currentIndex = menu_layer_get_selected_index(myMenuLayer);
    if (currentIndex.row < getLengthOfCurrentPage())
    {
        app_timer_cancel(wd->textScrollTimer);
        //scroll the text back to the beginning when unselecting
        scrollTextBackToStart();
        currentIndex.row++;
        menu_layer_set_selected_index(myMenuLayer, currentIndex, MenuRowAlignCenter, true);
        WindowData* wd = window_get_user_data(window);
        wd->textScrollTimer = app_timer_register(wd->config->scrollSpeed, timerTick, NULL);
    }
}

void back_click_handler(ClickRecognizerRef recognizer, void *context) 
{
    WindowData* wd = (WindowData*)window_get_user_data(window);
    if (wd->currentPage != 1)
    {
        app_timer_cancel(wd->textScrollTimer);
        wd->currentScrollable = 0;
        destroyItemList(wd->items);
        wd->currentPage--;
        menu_layer_reload_data(myMenuLayer);
        MenuIndex mi;
        mi.row = 0;
        mi.section = 0;
        menu_layer_set_selected_index(myMenuLayer, mi, MenuRowAlignCenter, true);
        wd->scrolledNumber = 0;
        wd->textScrollTimer = app_timer_register(wd->config->scrollSpeed, timerTick, NULL);
    }
    else
    {
        window_stack_pop_all(1);
    }    
}