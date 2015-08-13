#include "Messaging.h"
#include "Main.h"
#include "WindowData.h"
#include "ErrorWindow.h"

char *translate_error(AppMessageResult result) {
  switch (result) {
    case APP_MSG_OK: return "APP_MSG_OK";
    case APP_MSG_SEND_TIMEOUT: return "APP_MSG_SEND_TIMEOUT";
    case APP_MSG_SEND_REJECTED: return "APP_MSG_SEND_REJECTED";
    case APP_MSG_NOT_CONNECTED: return "APP_MSG_NOT_CONNECTED";
    case APP_MSG_APP_NOT_RUNNING: return "APP_MSG_APP_NOT_RUNNING";
    case APP_MSG_INVALID_ARGS: return "APP_MSG_INVALID_ARGS";
    case APP_MSG_BUSY: return "APP_MSG_BUSY";
    case APP_MSG_BUFFER_OVERFLOW: return "APP_MSG_BUFFER_OVERFLOW";
    case APP_MSG_ALREADY_RELEASED: return "APP_MSG_ALREADY_RELEASED";
    case APP_MSG_CALLBACK_ALREADY_REGISTERED: return "APP_MSG_CALLBACK_ALREADY_REGISTERED";
    case APP_MSG_CALLBACK_NOT_REGISTERED: return "APP_MSG_CALLBACK_NOT_REGISTERED";
    case APP_MSG_OUT_OF_MEMORY: return "APP_MSG_OUT_OF_MEMORY";
    case APP_MSG_CLOSED: return "APP_MSG_CLOSED";
    case APP_MSG_INTERNAL_ERROR: return "APP_MSG_INTERNAL_ERROR";
    default: return "UNKNOWN ERROR";
  }
}
    
//called when receive message
void inbox_received_callback(DictionaryIterator *iterator, void *context) 
{
    // Read first item
    Tuple *t = dict_read_first(iterator);
    
    char* projectNamesStr = 0;
    char* projectIDsStr = 0;
    char* itemNamesStr = 0;
    char* itemIDsStr = 0;
    char* strTimeline = 0;
    char* configStr = 0;
    WindowData* wd = (WindowData*)window_get_user_data(window);
    
    // For all items
    while(t != NULL) 
    {
        // Which key was received?
        switch(t->key) 
        {
            case PROJECT_NAMES:
                projectNamesStr = (char*)calloc(t->length, sizeof(char));
                strcpy(projectNamesStr, t->value->cstring);
            break;
            case PROJECT_IDs:
                projectIDsStr = (char*)calloc(t->length, sizeof(char));
                strcpy(projectIDsStr, t->value->cstring);
            break;
            case ITEM_NAMES:
                itemNamesStr = (char*)calloc(t->length, sizeof(char));
                strcpy(itemNamesStr, t->value->cstring);
            break;
            case ITEM_IDS:
                itemIDsStr = (char*)calloc(t->length, sizeof(char));
                strcpy(itemIDsStr, t->value->cstring);
            break;
            case TIMELINE_JSON:
                strTimeline = (char*)calloc(t->length, sizeof(char));
                strcpy(strTimeline, t->value->cstring);
            break;
            case SELECTED_ITEM:
                
                if (strcmp(t->value->cstring,"1") == 0)
                {
                    MenuIndex currentIndex = menu_layer_get_selected_index(myMenuLayer);
                    int currentRow = currentIndex.row;
                    wd->items->checked[currentRow] = 1;
                    window_set_click_config_provider(window, (ClickConfigProvider) config_provider);
                    layer_mark_dirty(menu_layer_get_layer(myMenuLayer));
                }
                else
                {
                    displayMessage("Failed to mark item as completed. This may be due to a connection issue.", 101);
                }
            break;
            case WAITING:
                if (t->value->int32 == 1)
                {
                    displayMessage("No account info found. Waiting for login info from phone.", 101);
                }
                if (t->value->int32 == 2)
                {
                    displayMessage("Loading Projects...", 102);
                }
            break;
            case ERROR:
                if (t->value->int32 == 1)
                {
                    displayMessage("Login info was incorrect. Popping up the config window so you can try again.", 101);
                }
                if (t->value->int32 == 2)
                {
                    displayMessage("Error retrieving projects.", 100);
                }
                if (t->value->int32 == 3)
                {
                    displayMessage("Project Not Found.", 100);
                }
                return;
            break;
            case ERRORMSG:
                while (window_stack_get_top_window() != window)
                {
                    window_stack_pop(1);
                }
                displayMessage(t->value->cstring, 100);
                return;
            break;
            case CONFIG:
                configStr = (char*)calloc(t->length, sizeof(char));
                strcpy(configStr, t->value->cstring);
                WindowData* wd = window_get_user_data(window);
                setConfig(configStr, wd->config);
                #ifdef PBL_COLOR
                    window_set_background_color(window, wd->config->backgroundColor);
                    menu_layer_set_normal_colors(myMenuLayer, wd->config->backgroundColor, wd->config->foregroundColor);
                #endif
                layer_mark_dirty(menu_layer_get_layer(myMenuLayer));
            break;
            default:
                displayMessage("Error: Unrecognized key in received data.", 101);
            break;
        }

        // Look for next item
        t = dict_read_next(iterator);
    }
    if (projectNamesStr)
    {
        ProjectStruct* projectList = createEmptyProjectList();
        unSerializeProjectsString(projectList, projectNamesStr, projectIDsStr);
        setProjects(wd, projectList);
        menu_layer_reload_data(myMenuLayer);
        MenuIndex mi;
        mi.row = 0;
        mi.section = 0;
        menu_layer_set_selected_index(myMenuLayer, mi, MenuRowAlignCenter, true);
        
        //start scrolling the text of the menu item
        wd->textScrollTimer = app_timer_register(wd->config->scrollSpeed, timerTick, NULL);
        free(projectNamesStr);
        free(projectIDsStr);
        
        //pop the loading screen (or screens) off the window stack
        while (window_stack_get_top_window() != window)
        {
            window_stack_pop(1);
        }

    }
    if (itemNamesStr)
    {
        ItemStruct* itemList = createEmptyItemList();
        unSerializeItemsString(itemList, itemNamesStr, itemIDsStr);
        wd->items = itemList;
        
        
        if (itemList->length == 0)
        {
            displayMessage("There are no items in this project.", NON_FATAL);
            free(itemNamesStr);
            free(itemIDsStr);
            return;
        }
        
        wd->currentPage = 2;
        menu_layer_reload_data(myMenuLayer);
        MenuIndex mi;
        mi.row = 0;
        mi.section = 0;
        menu_layer_set_selected_index(myMenuLayer, mi, MenuRowAlignCenter, true);
        free(itemNamesStr);
        free(itemIDsStr);
        //pop the loading screen (or screens) off the window stack
        while (window_stack_get_top_window() != window)
        {
            window_stack_pop(1);
        }
        wd->textScrollTimer = app_timer_register(wd->config->scrollSpeed, timerTick, NULL);
    }
    
    //probably dont actually need this
    /*if (itemNamesStrTimeline)
    {
        ItemStruct* itemList = createEmptyItemList();
        unSerializeItemsString(itemList, itemNamesStrTimeline, itemIDsStrTimeline);
        wd->timelineItems = itemList;
        
        if (itemList->length == 0)
        {
            free(itemNamesStrTimeline);
            free(itemIDsStrTimeline);
            return;
        }
        
        //pin to timeline
        #ifdef PBL_COLOR
            
        #endif
        
        free(itemNamesStr);
        free(itemIDsStr);
    }*/
    
}

void inbox_dropped_callback(AppMessageResult reason, void *context) 
{
  APP_LOG(APP_LOG_LEVEL_ERROR, "ERROR: Message dropped. Reason: %d - %s", reason, translate_error(reason));
}

void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) 
{
  APP_LOG(APP_LOG_LEVEL_ERROR, "ERROR: Appmessage send failed. Reason: %d - %s", reason, translate_error(reason));
}

void outbox_sent_callback(DictionaryIterator *iterator, void *context) 
{
}