#include <pebble.h>
#include "Main.h"
#include "ErrorMessage.h"
#include "WindowData.h"
#include "MainWindowClicks.h"

//called when an item is selected
void menu_select_callback(ClickRecognizerRef recognizer, void *context)
{
    if (pageDepth == 1)
    {
        scrollTextBackToStart();
        layer_mark_dirty(menu_layer_get_layer(myMenuLayer));
        MenuIndex currentIndex = menu_layer_get_selected_index(myMenuLayer);
        int currentRow = currentIndex.row;
        sendProjectIDToPhone(currentRow);
        displayMessage("Loading...", 102);
    }
    if (pageDepth == 2)
    {
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
        //scroll the text back to the beginning when unselecting
        
        //this will not work when it STARTS on a scrollable text field, needs to be fixed
        scrollTextBackToStart();
        currentIndex.row--;
        menu_layer_set_selected_index(myMenuLayer, currentIndex, MenuRowAlignCenter, true);
    }
}

void down_click_handler(ClickRecognizerRef recognizer, void *context) 
{
    WindowData* wd = (WindowData*)window_get_user_data(window);
    MenuIndex currentIndex = menu_layer_get_selected_index(myMenuLayer);
    if (currentIndex.row < wd->projects->length)
    {
        //scroll the text back to the beginning when unselecting
        scrollTextBackToStart();
        currentIndex.row++;
        menu_layer_set_selected_index(myMenuLayer, currentIndex, MenuRowAlignCenter, true);
    }
}

void back_click_handler(ClickRecognizerRef recognizer, void *context) 
{
    WindowData* wd = (WindowData*)window_get_user_data(window);
    if (pageDepth != 1)
    {
        destroyItemList(wd->items);
        cleanupCheckboxList();
        pageDepth--;
        menu_layer_reload_data(myMenuLayer);
        scrolledNumber = 0;
    }
    else
    {
        window_stack_pop_all(1);
    }    
}