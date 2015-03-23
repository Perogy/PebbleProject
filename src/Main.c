#include <pebble.h>
#include "Main.h"
#include "CustomFunctions.h"
#include "Items.h"
#include "Projects.h"
#include "CheckboxList.h"
#include "ErrorWindow.h"
#include "WindowData.h"
#include "MainWindowClicks.h"

itemCheckboxLayer* checkboxList = 0;
int scrolledNumber = 0;
bool scrollable = 0;

Window* window;
MenuLayer* myMenuLayer;
AppTimer* textScrollTimer;

//page depth 1 = projects display page depth 2 = items for the project
int pageDepth = 1;
    
//sends the project ID at specified index to the phone to get itemlist
void sendProjectIDToPhone(int index)
{
    WindowData* wd = (WindowData*)window_get_user_data(window);
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    if (!iter) {
      // Error creating outbound message
      return;
    }
  
    dict_write_cstring(iter, SELECTED_PROJECT, wd->projects->projectIDs[index]);
    dict_write_end(iter);
  
    app_message_outbox_send();
}

//sends the item ID at specified index to the phone to set an item as completed
void sendItemIDToPhone(int index)
{
    WindowData* wd = (WindowData*)window_get_user_data(window);
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    if (!iter) {
      // Error creating outbound message
      return;
    }
  
    dict_write_cstring(iter, SELECTED_ITEM, wd->items->itemIDs[index]);
    dict_write_end(iter);
  
    app_message_outbox_send();
}

//draws a checkbox in the given cell layer of the menu
void drawCheckbox(GContext *ctx, Layer *cell_layer, int index)
{
    
    //run this initializing code if checkboxList is set to 0.. maybe this code should be outside of the draw function somewhere
    if (checkboxList->checkboxFrame[index].size.w == 0)
    {
        
        //draw Rect
        /*GRect cellBounds = layer_get_bounds(cell_layer);
        GRect G = GRect(cellBounds.size.w*.80, 0, cellBounds.size.w-(cellBounds.size.w*.85), cellBounds.size.h);
        checkboxList->checkboxFrame[index].size.w = G.size.w;
        checkboxList->checkboxFrame[index].size.h = G.size.h;
        checkboxList->checkboxFrame[index].origin.x = G.origin.x;
        checkboxList->checkboxFrame[index].origin.y = G.origin.y;
        //checkboxList->checkboxLayer[index] = layer_create(checkboxList->checkboxFrame[index]);
        G = GRect(checkboxList->checkboxFrame[index].origin.x, checkboxList->checkboxFrame[index].origin.y+(checkboxList->checkboxFrame[index].size.h*.25), 
                  checkboxList->checkboxFrame[index].size.w*.90, checkboxList->checkboxFrame[index].size.h*.50);
        checkboxList->checkboxRect[index].size.w = G.size.w;
        checkboxList->checkboxRect[index].size.h = G.size.h;
        checkboxList->checkboxRect[index].origin.x = G.origin.x;
        checkboxList->checkboxRect[index].origin.y = G.origin.y;*/
    }
    
    //snprintf(asdf, 100, "\nwhat the derp is going on %d\n", checkboxList->checkboxFrame[index].size.w);
    //displayErrorMessage(asdf);
    //layer_add_child(cell_layer, checkboxList->checkboxLayer[index]);
    
    graphics_context_set_fill_color(ctx, GColorWhite);
    //draw Circle
    GRect cellBounds = layer_get_bounds(cell_layer);
    //make this scalable
    int radius = 8;
    GPoint g = GPoint(cellBounds.size.w*.80 + radius, cellBounds.size.h/2.0);
    graphics_draw_circle(ctx, g, CIRCLE_RADIUS);
    
    //graphics_fill_rect(ctx, checkboxList->checkboxFrame[index], 0, GCornerNone);
    //graphics_draw_rect(ctx, checkboxList->checkboxRect[index]);
}

//draws a dot inside the checkbox to mark as "checked"
void checkCheckbox(GContext *ctx, Layer *cell_layer, int index)
{
    WindowData* wd = (WindowData*)window_get_user_data(window);
    if (wd->items->checked[index] == 1)
    {
        //Check Rect
        /*GRect temp = checkboxList->checkboxRect[index];
        temp.origin.x = temp.origin.x + 5;
        temp.origin.y = temp.origin.y + 5;
        temp.size.w = temp.size.w - 10;
        temp.size.h = temp.size.h - 10;
        graphics_context_set_fill_color(ctx, GColorBlack);
        graphics_fill_rect(ctx, temp, 0, GCornerNone);*/
        
        //Check Circle
        graphics_context_set_fill_color(ctx, GColorBlack);
        GRect cellBounds = layer_get_bounds(cell_layer);
        GPoint g = GPoint(cellBounds.size.w*.80 + CIRCLE_RADIUS, cellBounds.size.h/2.0);
        graphics_fill_circle(ctx, g, CIRCLE_RADIUS-4);
    }
}

//this is executed when row is changed.
void draw_row_callback(GContext *ctx, Layer *cell_layer, MenuIndex *cell_index, void *callback_context)
{
    WindowData* wd = (WindowData*)window_get_user_data(window);
    if (!wd)
        return;
    if (pageDepth == 1)
    {
        if (wd->projects)
        {
            for(int i=0;i<(wd->projects->length);i++)
            {
                //remember this is the row it is CURRENTLY drawing not the SELECTED row, it will draw all rows on an update by calling the function a bunch of different times
                if (cell_index->row == i)
                {
                    GRect cellBounds = layer_get_bounds(cell_layer);
                    graphics_context_set_text_color(ctx, GColorBlack);
                    graphics_context_set_fill_color(ctx, GColorWhite);
                    graphics_fill_rect(ctx, cellBounds, 0, GCornerNone);
                    
                    MenuIndex currentIndex = menu_layer_get_selected_index(myMenuLayer);
                    int currentRow = currentIndex.row;
                    //if selected
                    if (cell_index->row == currentRow)
                    {
                        GSize textSize = graphics_text_layout_get_content_size(wd->projects->projects[i], fonts_get_system_font(FONT_KEY_GOTHIC_24), cellBounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft);
                        //snprintf(asdf, 100, "%d", textSize.w);
                        //displayErrorMessage(asdf);
                        if (textSize.w > (cellBounds.size.w*.90))
                        {
                            scrollable = 1;
                        }
                        else
                        {
                            scrollable = 0; 
                        }
                    }
                    graphics_draw_text(ctx, wd->projects->projects[i], fonts_get_system_font(FONT_KEY_GOTHIC_24), cellBounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
                }
            }
        }
    }
    if (pageDepth == 2)
    {
        if (wd->items && (wd->items->length != 0))
        {
            for(int i=0;i<(wd->items->length);i++)
            {
                if (cell_index->row == i)
                {
                    GRect cellBounds = layer_get_bounds(cell_layer);
                    
                    //text takes up 80% of the layer size due to the checkboxes
                    GRect textBounds = GRect(cellBounds.origin.x, cellBounds.origin.y, cellBounds.size.w*.80, cellBounds.size.h);
                    //GRect newBounds = GRect(cellBounds.origin.x, cellBounds.origin.y, cellBounds.size.w*.80, cellBounds.size.h);
                    //layer_set_bounds(cell_layer, newBounds);
                    graphics_context_set_text_color(ctx, GColorBlack);
                    graphics_context_set_fill_color(ctx, GColorWhite);
                    graphics_fill_rect(ctx, cellBounds, 0, GCornerNone);
                    
                    MenuIndex currentIndex = menu_layer_get_selected_index(myMenuLayer);
                    int currentRow = currentIndex.row;
                    //if selected
                    if (cell_index->row == currentRow)
                    {
                        GSize textSize = graphics_text_layout_get_content_size(wd->items->items[i], fonts_get_system_font(FONT_KEY_GOTHIC_24), cellBounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft);
                        //snprintf(asdf, 100, "%d", textSize.w);
                        //displayErrorMessage(asdf);
                        if (textSize.w >= (textBounds.size.w))
                        {
                            scrollable = 1;
                        }
                        else
                        {
                            scrollable = 0; 
                        }
                    }
                    graphics_draw_text(ctx, wd->items->items[i], fonts_get_system_font(FONT_KEY_GOTHIC_24), textBounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
                    drawCheckbox(ctx, cell_layer, i);
                    checkCheckbox(ctx, cell_layer, i);
                }
            }
        }
    }
}

//sets height of cells
uint16_t cell_height_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
{
    //make this scalable
    return 30;
}

//sets number of rows
uint16_t num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *callback_context)
{
    WindowData* wd = (WindowData*)window_get_user_data(window);
    if(!wd)
        return 0;
    if (pageDepth == 1)
    {
        if (wd->projects)
        {
            return wd->projects->length;
        }
        else
        {
            return 0;
        }
    }
    if (pageDepth == 2)
    {
        if (wd->items)
        {
            return wd->items->length;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

void timerTick(void* data)
{
    WindowData* wd = (WindowData*)window_get_user_data(window);
    MenuIndex currentIndex = menu_layer_get_selected_index(myMenuLayer);
    int currentRow = currentIndex.row;
    if (pageDepth == 1 && scrollable)
    {
        scrollTextByOneLetter(wd->projects->projects[currentRow]);
        if (scrolledNumber == (int)strlen(wd->projects->projects[currentRow]) - 1)
            scrolledNumber = 0;
        else
            scrolledNumber++;
        layer_mark_dirty(menu_layer_get_layer(myMenuLayer));
    }
    if (pageDepth == 2 && scrollable)
    {
        scrollTextByOneLetter(wd->items->items[currentRow]);
        if (scrolledNumber == (int)strlen(wd->items->items[currentRow]) - 1)
            scrolledNumber = 0;
        else
            scrolledNumber++;
        layer_mark_dirty(menu_layer_get_layer(myMenuLayer));
    }
    textScrollTimer = app_timer_register(TEXT_SCROLL_INTERVAL, timerTick, NULL);
}



void cleanupCheckboxList()
{
    free(checkboxList->checkboxLayer);
    free(checkboxList->checkboxFrame);
    free(checkboxList->checkboxRect);
    free(checkboxList->checked);
    free(checkboxList);
}

void scrollTextBackToStart()
{
    MenuIndex currentIndex = menu_layer_get_selected_index(myMenuLayer);
    WindowData* wd = (WindowData*)window_get_user_data(window);
    while (scrolledNumber > 0)
    {
        if (pageDepth == 1)
        {
            scrollTextByOneLetter(wd->projects->projects[currentIndex.row]);
            if (scrolledNumber == (int)strlen(wd->projects->projects[currentIndex.row]) - 1)
                scrolledNumber = 0;
            else
                scrolledNumber++;
        }
        if (pageDepth == 2)
        {
            scrollTextByOneLetter(wd->items->items[currentIndex.row]);
            if (scrolledNumber == (int)strlen(wd->items->items[currentIndex.row]) - 1)
                scrolledNumber = 0;
            else
                scrolledNumber++;
        }
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
            case SELECTED_ITEM:
                
                if (strcmp(t->value->cstring,"1") == 0)
                {
                    MenuIndex currentIndex = menu_layer_get_selected_index(myMenuLayer);
                    int currentRow = currentIndex.row;
                    wd->items->checked[currentRow] = 1;
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
        WindowData* createdWD = createWindowData(projectList);
        window_set_user_data(window, createdWD);
        menu_layer_reload_data(myMenuLayer);
        MenuIndex mi;
        mi.row = 0;
        mi.section = 0;
        menu_layer_set_selected_index(myMenuLayer, mi, MenuRowAlignCenter, true);
        
        //start scrolling the text of the menu item
        textScrollTimer = app_timer_register(TEXT_SCROLL_INTERVAL, timerTick, NULL);
        free(projectNamesStr);
        free(projectIDsStr);
        //pop the loading screen off the window stack
        if (window_stack_contains_window(errorWindow))
            window_stack_pop(1);
    }
    if (itemNamesStr)
    {
        ItemStruct* itemList = createEmptyItemList();
        unSerializeItemsString(itemList, itemNamesStr, itemIDsStr);

        if (wd->items)
            destroyItemList(wd->items);
        wd->items = itemList;
        
        if (itemList->length == 0)
        {
            displayMessage("There are no items in this project.", NON_FATAL);
            free(itemNamesStr);
            free(itemIDsStr);
            return;
        }
        //initialize item checkboxes
        checkboxList = (itemCheckboxLayer*)malloc(sizeof(itemCheckboxLayer));
        checkboxList->checkboxLayer = (Layer**)calloc(sizeof(Layer*),itemList->length);
        checkboxList->checkboxFrame = (GRect*)calloc(sizeof(GRect),itemList->length);
        checkboxList->checkboxRect = (GRect*)calloc(sizeof(GRect),itemList->length);
        checkboxList->checked = (int*)calloc(sizeof(int),itemList->length);
        checkboxList->length = itemList->length;
        MenuIndex mi;
        mi.row = 0;
        mi.section = 0;
        menu_layer_set_selected_index(myMenuLayer, mi, MenuRowAlignCenter, true);
        pageDepth = 2;
        menu_layer_reload_data(myMenuLayer);
        free(itemNamesStr);
        free(itemIDsStr);
        //pop the loading screen off the window stack
        if (window_stack_contains_window(errorWindow))
            window_stack_pop(1);
    }
    
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
    window_set_user_data(window, 0);
    MenuLayerCallbacks callbacks = 
    {
        .draw_row = (MenuLayerDrawRowCallback) draw_row_callback,
        .get_num_rows = (MenuLayerGetNumberOfRowsInSectionsCallback) num_rows_callback,
        .get_cell_height = (MenuLayerGetCellHeightCallback) cell_height_callback
    };
    menu_layer_set_callbacks(myMenuLayer, NULL, callbacks);

    window_set_click_config_provider(window, (ClickConfigProvider) config_provider);
    
    layer_add_child(window_get_root_layer(window), menu_layer_get_layer(myMenuLayer));
    

}
 
void window_unload(Window *window)
{
    WindowData* wd = (WindowData*)window_get_user_data(window);
    menu_layer_destroy(myMenuLayer);
    destroyProjectList(wd->projects);
}

void init()
{
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
    window_destroy(window);
}

int main(void)
{
    init();
    app_event_loop();
    deinit();
}