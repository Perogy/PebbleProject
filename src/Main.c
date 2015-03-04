#include <pebble.h>
#include "Main.h"
#include "CustomFunctions.h"

//page depth 1 = projects display page depth 2 = items for the project
int pageDepth = 1;

int check = 0;

//sends the project ID at specified index to the phone to get itemlist
void sendProjectIDToPhone(int index)
{
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    if (!iter) {
      // Error creating outbound message
      return;
    }
  
    dict_write_cstring(iter, SELECTED_ITEM, projectList->projectIDs[index]);
    dict_write_end(iter);
  
    app_message_outbox_send();
}

//draws a checkbox in the given cell layer of the menu
void drawCheckbox(GContext *ctx, Layer *cell_layer, int index)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "\nin draw checkbox\n");

    //pointers are dirty as hell in this thing, be careful.
    GRect cellBounds = layer_get_bounds(cell_layer);

    checkboxList->checkboxFrame[index] = &GRect(cellBounds.size.w*.80, 0, cellBounds.size.w-(cellBounds.size.w*.85), cellBounds.size.h);
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_context_set_stroke_color(ctx, GColorBlack);
    graphics_fill_rect(ctx, *(checkboxList->checkboxFrame[index]), 0, GCornerNone);

    checkboxList->checkboxLayer[index] = layer_create(*(checkboxList->checkboxFrame[index]));
    layer_add_child(cell_layer, checkboxList->checkboxLayer[index]);

    checkboxList->checkboxRect[index] = &GRect((*(checkboxList->checkboxFrame[index])).origin.x, (*(checkboxList->checkboxFrame[index])).origin.y+((*(checkboxList->checkboxFrame[index])).size.h*.25), 
                                               (*(checkboxList->checkboxFrame[index])).size.w*.90, (*(checkboxList->checkboxFrame[index])).size.h*.50);
    graphics_draw_rect(ctx, *(checkboxList->checkboxRect[index]));
}

void checkCheckbox(GContext *ctx, int index)
{
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_context_set_stroke_color(ctx, GColorBlack);
    GRect checked = *(checkboxList->checkboxRect[index]);
    graphics_fill_rect(ctx, checked, 0, GCornerNone);
}

//this is executed when row is changed.
void draw_row_callback(GContext *ctx, Layer *cell_layer, MenuIndex *cell_index, void *callback_context)
{
    if (pageDepth == 1)
    {
        if (projectList)
        {
            for(int i=0;i<(projectList->length);i++)
            {
                if (cell_index->row == i)
                {
                    menu_cell_basic_draw(ctx, cell_layer, projectList->projects[i], NULL, NULL);
                }
            }
        }
    }
    if (pageDepth == 2)
    {
        if (itemList)
        {
            for(int i=0;i<(itemList->length);i++)
            {
                if (cell_index->row == i)
                {
                    menu_cell_basic_draw(ctx, cell_layer, itemList->items[i], NULL, NULL);
                    drawCheckbox(ctx, cell_layer, i);
                    if (check == 1)
                    {
                        checkCheckbox(ctx, i);
                        check = 0;
                    }

                }
            }
        }
    }
}



//sets number of rows
uint16_t num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *callback_context)
{
    if (pageDepth == 1)
    {
        if (projectList->length)
        {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "\nnumrows Returning: %d\n", projectList->length);
            return projectList->length;
        }
        else
        {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "\nnumrows Returning: 0\n");
            return 0;
        }
    }
    if (pageDepth == 2)
    {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "\nnumrows Returning: %d\n", itemList->length);
        return itemList->length;
    }
    else
    {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "\nnumrows Returning: 0\n");
        return 0;
    }
}
void unSerializeProjectsString(char* projectNamesString, char* projectIDsString)
{
    projectList = (projectStruct*)malloc(sizeof(projectStruct));
    projectList->projects = 0;
    projectList->length = 0;
    projectList->projectIDs = 0;
    projectList->projects = splitString(projectNamesString, '|', &projectList->length);
    projectList->projectIDs = splitString(projectIDsString, '|', &projectList->length);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "\nprojectNamesString: %s\n", projectNamesString);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "\nproject 5: %s\n", projectList->projects[5]);
}

void unSerializeItemsString(char* itemNamesString, char* itemIDsString)
{
    itemList = (itemStruct*)malloc(sizeof(itemStruct));
    itemList->items = 0;
    itemList->length = 0;
    itemList->itemIDs = 0;
    itemList->items = splitString(itemNamesString, '|', &itemList->length);
    itemList->itemIDs = splitString(itemIDsString, '|', &itemList->length);
    outputArrayContents(itemList->items, itemList->length);
    outputArrayContents(itemList->itemIDs, itemList->length);
}

//called when an item is selected
static void menu_select_callback(ClickRecognizerRef recognizer, void *context)
{
    if (pageDepth == 1)
    {
        MenuIndex currentIndex = menu_layer_get_selected_index(myMenuLayer);
        int currentRow = currentIndex.row;
        sendProjectIDToPhone(currentRow);
    }
    if (pageDepth == 2)
    {
        check = 1;
        menu_layer_reload_data(myMenuLayer);
    }
}

static void back_click_handler(ClickRecognizerRef recognizer, void *context) 
{
    if (pageDepth != 1)
    {
        pageDepth--;
        menu_layer_reload_data(myMenuLayer);
        checkboxList = 0;
    }
    else
    {
        window_stack_pop_all(1);
    }    
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) 
{
    MenuIndex currentIndex = menu_layer_get_selected_index(myMenuLayer);
    if (currentIndex.row > 0)
    {
        currentIndex.row--;
        menu_layer_set_selected_index(myMenuLayer, currentIndex, MenuRowAlignCenter, true);
    }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) 
{
    MenuIndex currentIndex = menu_layer_get_selected_index(myMenuLayer);
    if (currentIndex.row < projectList->length)
    {
        currentIndex.row++;
        menu_layer_set_selected_index(myMenuLayer, currentIndex, MenuRowAlignCenter, true);
    }
}

//called when receive message
static void inbox_received_callback(DictionaryIterator *iterator, void *context) 
{
    // Read first item
    Tuple *t = dict_read_first(iterator);

    char* projectNamesStr = 0;
    char* projectIDsStr = 0;
    char* itemNamesStr = 0;
    char* itemIDsStr = 0;
    checkboxList = 0;
    // For all items
    while(t != NULL) 
    {
        // Which key was received?
        switch(t->key) 
        {
            case PROJECT_NAMES:
            projectNamesStr = (char*)calloc(t->length, sizeof(char));
            strcpy(projectNamesStr, t->value->cstring);
            APP_LOG(APP_LOG_LEVEL_DEBUG, "\nBeatProjectNames");
            break;
            case PROJECT_IDs:
            projectIDsStr = (char*)calloc(t->length, sizeof(char));
            strcpy(projectIDsStr, t->value->cstring);
            APP_LOG(APP_LOG_LEVEL_DEBUG, "\nBeatProjectID");
            break;
            case ITEM_NAMES:
            itemNamesStr = (char*)calloc(t->length, sizeof(char));
            strcpy(itemNamesStr, t->value->cstring);
            break;
            case ITEM_IDS:
            itemIDsStr = (char*)calloc(t->length, sizeof(char));
            strcpy(itemIDsStr, t->value->cstring);
            default:
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Key %d not recognized!", (int)t->key);
            break;
        }

        // Look for next item
        t = dict_read_next(iterator);
    }
    if (projectNamesStr)
    {
        unSerializeProjectsString(projectNamesStr, projectIDsStr);
    }
    if (itemNamesStr)
    {
        unSerializeItemsString(itemNamesStr, itemIDsStr);
        //initialize item checkboxes
        checkboxList = (itemCheckboxLayer*)malloc(sizeof(itemCheckboxLayer));
        checkboxList->checkboxLayer = (Layer**)malloc(sizeof(Layer*)*itemList->length);
        checkboxList->checkboxFrame = (GRect**)malloc(sizeof(GRect*)*itemList->length);
        checkboxList->checkboxRect = (GRect**)malloc(sizeof(GRect*)*itemList->length);
        checkboxList->length = itemList->length;
        pageDepth = 2;
        
    }
    menu_layer_reload_data(myMenuLayer);
}


static void inbox_dropped_callback(AppMessageResult reason, void *context) 
{
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message Dropped! Reason: %d", reason);
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) 
{
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed! Reason: %d", reason);
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) 
{
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}


//custom button click handling as we need to change the function of the back button
void config_provider(Window *window) 
{
    window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler);
    window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
    window_single_click_subscribe(BUTTON_ID_SELECT, menu_select_callback);
}

void window_load(Window *window)
{
    myMenuLayer = menu_layer_create(GRect(0, 0, 144, 168 - 16));
    //removed this as we need a custom back button function
    //menu_layer_set_click_config_onto_window(myMenuLayer, window);

    MenuLayerCallbacks callbacks = 
    {
        .draw_row = (MenuLayerDrawRowCallback) draw_row_callback,
        .get_num_rows = (MenuLayerGetNumberOfRowsInSectionsCallback) num_rows_callback
    };
    menu_layer_set_callbacks(myMenuLayer, NULL, callbacks);
    
    window_set_click_config_provider(window, (ClickConfigProvider) config_provider);
    
    layer_add_child(window_get_root_layer(window), menu_layer_get_layer(myMenuLayer));

}
 
void window_unload(Window *window)
{
  //We will safely destroy the Window's elements here!
}

void init()
{
    //initialize projectList members to 0
    projectList = (projectStruct*)malloc(sizeof(projectStruct));
    projectList->projects = 0;
    projectList->length = 0;
    projectList->projectIDs = 0;
    
    //register networking callbacks
    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);
    
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
    
    window = window_create();
    window_set_window_handlers(window, (WindowHandlers) 
    {
        .load = window_load,
        .unload = window_unload,
    });
    window_stack_push(window, true);
}
 
void deinit()
{
    menu_layer_destroy(myMenuLayer);
    window_destroy(window);
}

int main(void)
{
    init();
    app_event_loop();
    deinit();
}