#include <pebble.h>
#include "Main.h"
#include "ErrorMessage.h"
#include "ErrorWindow.h"
#include "WindowData.h"
#include "MainWindowClicks.h"
#include "Messaging.h"

#ifdef PBL_COLOR
    static DictationSession *s_dictation_session;

    char *translate_dictation_error(int status) {
      switch (status) {
        case 1: return "User rejected transcription and dismissed the dictation UI.";
        case 2: return "User exited the dictation UI after a transcription error.";
        case 3: return "Too many errors occurred during transcription and the dictation UI exited.";
        case 4: return "No speech was detected and the dictation UI exited.";
        case 5: return "No Bluetooth or Internet connection available.";
        case 6: return "Voice transcription disabled for this user. This can occur if the user has disabled sending 'Usage logs' in the Pebble mobile app.";
        case 7: return "Voice transcription failed due to an internal error.";
        case 8: return "Cloud recognizer failed to transcribe speech (only possible if error dialogs are disabled).";
        default: return "UNKNOWN ERROR";
      }
    }

    static void dictation_session_callback(DictationSession *session, DictationSessionStatus status, char *transcription, void *context) 
    {
        if(status == DictationSessionStatusSuccess) 
        {
            char* text;
            text = (char*)malloc(512);
            // Display the dictated text
            snprintf(text, 512, "%s", transcription);
            addItem(text);
            free(text);
        } 
        else 
        {
            // Display the reason for any error
            static char s_failed_buff[512];
            snprintf(s_failed_buff, sizeof(s_failed_buff), "Transcription failed.\n\nReason:\n%s", translate_dictation_error(status));
            
            //status 1 and 2 mean user exited so we don't want to show an error.
            if (status > 2)
                displayMessage(s_failed_buff, 101);
        }
        dictation_session_destroy(s_dictation_session);
    }
#endif

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
        MenuIndex currentIndex = menu_layer_get_selected_index(myMenuLayer);
        int currentRow = currentIndex.row;
        
        #ifdef PBL_COLOR
        
            WindowData* wd = (WindowData*)window_get_user_data(window);
            //if item is "Add New" (item id will be zero if it's add new)
            if (strcmp(wd->items->itemIDs[currentRow],"0") == 0)
            {
                app_timer_cancel(wd->textScrollTimer);
                scrollTextBackToStart();
                layer_mark_dirty(menu_layer_get_layer(myMenuLayer));
                s_dictation_session = dictation_session_create(512, dictation_session_callback, NULL);
                dictation_session_start(s_dictation_session);
                return;
            }
        
        #endif
        
        //turn off input while saving
        window_set_click_config_provider(window, (ClickConfigProvider)0);
        
        
        
        
        
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