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
int selectedProjectIndex;

GColor backgroundColor;
GColor altBackgroundColor;
GColor highlightColor;

Window* window;
MenuLayer* myMenuLayer;
AppTimer* textScrollTimer;

//page depth 1 = projects, page depth 2 = items for the selected project
int pageDepth;
    

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
    WindowData* wd = (WindowData*)window_get_user_data(window);
    GRect cellBounds = layer_get_bounds(cell_layer);
    GPoint g = GPoint(cellBounds.size.w*.80 + CIRCLE_RADIUS, cellBounds.size.h/2.0);
    #ifdef PBL_COLOR
        graphics_context_set_antialiased(ctx, true);
    #endif
    graphics_draw_circle(ctx, g, CIRCLE_RADIUS);
    
    if (wd->items->checked[index] == 1)
    {
        GPoint g = GPoint(cellBounds.size.w*.80 + CIRCLE_RADIUS, cellBounds.size.h/2.0);
        //needed for aplite
        graphics_context_set_fill_color(ctx, wd->config->foregroundColor);
        #ifdef PBL_COLOR
            MenuIndex currentIndex = menu_layer_get_selected_index(myMenuLayer);
            int currentRow = currentIndex.row;
            if (index == currentRow)
            {
                graphics_context_set_fill_color(ctx, wd->config->highlightForegroundColor);
            }
            else
            {
                graphics_context_set_fill_color(ctx, wd->config->foregroundColor);
            }
        #endif
        graphics_fill_circle(ctx, g, CIRCLE_RADIUS/2.0);
    }
}

//draws a dot inside the checkbox to mark as "checked"
void checkCheckbox(GContext *ctx, Layer *cell_layer, int index)
{
    WindowData* wd = (WindowData*)window_get_user_data(window);
    if (wd->items->checked[index] == 1)
    {   
        GRect cellBounds = layer_get_bounds(cell_layer);
        GPoint g = GPoint(cellBounds.size.w*.80 + CIRCLE_RADIUS, cellBounds.size.h/2.0);
        graphics_fill_circle(ctx, g, CIRCLE_RADIUS/2.0);
    }
}

void draw_header_callback(GContext *ctx, Layer *cell_layer, uint16_t section_index, void *callback_context)
{
    //menu_cell_basic_header_draw(ctx, cell_layer, "Projects");
    if (pageDepth == 1)
    {
        WindowData* wd = (WindowData*)window_get_user_data(window);
        GRect cellBounds = layer_get_bounds(cell_layer);
        graphics_context_set_text_color(ctx, wd->config->foregroundColor);
        graphics_draw_text(ctx, "Projects", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), cellBounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
        graphics_context_set_stroke_color(ctx, wd->config->foregroundColor);
        graphics_draw_line(ctx,GPoint(cellBounds.origin.x,cellBounds.origin.y+cellBounds.size.h-1), GPoint(cellBounds.origin.x+cellBounds.size.w, cellBounds.origin.y+cellBounds.size.h-1));
    }
    else
    {
        WindowData* wd = (WindowData*)window_get_user_data(window);
        GRect cellBounds = layer_get_bounds(cell_layer);
        graphics_context_set_text_color(ctx, wd->config->foregroundColor);
        graphics_draw_text(ctx, wd->projects->projects[selectedProjectIndex], fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), cellBounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
        graphics_context_set_stroke_color(ctx, wd->config->foregroundColor);
        graphics_draw_line(ctx,GPoint(cellBounds.origin.x,cellBounds.origin.y+cellBounds.size.h-1), GPoint(cellBounds.origin.x+cellBounds.size.w, cellBounds.origin.y+cellBounds.size.h-1));
    }
    
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return 30;
}

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return 1;
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
                    
                    MenuIndex currentIndex = menu_layer_get_selected_index(myMenuLayer);
                    int currentRow = currentIndex.row;
                    
                    if (cell_index->row == currentRow)
                    {
                        //have to reverse background/foreground color on aplite as it seems to auto invert
                        //and basalt does not (you set the colors in "normal colors" function).
                        #ifdef PBL_COLOR
                            graphics_context_set_text_color(ctx, wd->config->highlightForegroundColor);
                            graphics_context_set_fill_color(ctx, wd->config->highlightBackgroundColor);
                            graphics_context_set_stroke_color(ctx, wd->config->highlightForegroundColor);
                        #else
                            graphics_context_set_text_color(ctx, wd->config->highlightBackgroundColor);
                            graphics_context_set_fill_color(ctx, wd->config->highlightForegroundColor);
                        #endif
                    }
                    else
                    {
                        if(i%2 == 0)
                        {
                            graphics_context_set_text_color(ctx, wd->config->foregroundColor);
                            graphics_context_set_fill_color(ctx, wd->config->backgroundColor);
                            graphics_context_set_stroke_color(ctx, wd->config->foregroundColor);
                        }  
                        else
                        {
                            graphics_context_set_text_color(ctx, wd->config->altForegroundColor);
                            graphics_context_set_fill_color(ctx, wd->config->altBackgroundColor);
                            graphics_context_set_stroke_color(ctx, wd->config->altForegroundColor);
                        }
                    }
                    graphics_fill_rect(ctx, cellBounds, 0, GCornerNone);
                    
                    
                    //if selected
                    if (cell_index->row == currentRow)
                    {
                        //increase cellbounds size so we can tell if the text will "overflow". if you dont do this the content size will stop at the end of the bounds
                        cellBounds.size.w = cellBounds.size.w*2.0;
                        GSize textSize = graphics_text_layout_get_content_size(wd->projects->projects[i], fonts_get_system_font(FONT_KEY_GOTHIC_24), cellBounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft);
                        cellBounds.size.w = cellBounds.size.w/2.0;
                        //snprintf(asdf, 100, "%d", textSize.w);
                        //displayErrorMessage(asdf);
                        if (textSize.w > cellBounds.size.w)
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
                    
                    MenuIndex currentIndex = menu_layer_get_selected_index(myMenuLayer);
                    int currentRow = currentIndex.row;
                    
                    if (cell_index->row == currentRow)
                    {
                        //have to reverse background/foreground color on aplite as it seems to auto invert
                        //and basalt does not (you set the colors in "normal colors" function).
                        #ifdef PBL_COLOR
                            graphics_context_set_text_color(ctx, wd->config->highlightForegroundColor);
                            graphics_context_set_fill_color(ctx, wd->config->highlightBackgroundColor);
                            graphics_context_set_stroke_color(ctx, wd->config->highlightForegroundColor);
                        #else
                            graphics_context_set_text_color(ctx, wd->config->highlightBackgroundColor);
                            graphics_context_set_fill_color(ctx, wd->config->highlightForegroundColor);
                        #endif
                    }
                    else
                    {
                        if(i%2 == 0)
                        {
                            graphics_context_set_text_color(ctx, wd->config->foregroundColor);
                            graphics_context_set_fill_color(ctx, wd->config->backgroundColor);
                            graphics_context_set_stroke_color(ctx, wd->config->foregroundColor);
                        }  
                        else
                        {
                            graphics_context_set_text_color(ctx, wd->config->altForegroundColor);
                            graphics_context_set_fill_color(ctx, wd->config->altBackgroundColor);
                            graphics_context_set_stroke_color(ctx, wd->config->altForegroundColor);
                        }
                    }
                    graphics_fill_rect(ctx, cellBounds, 0, GCornerNone);

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
                    //checkCheckbox(ctx, cell_layer, i);
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
    textScrollTimer = app_timer_register(wd->config->scrollSpeed, timerTick, NULL);
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
        textScrollTimer = app_timer_register(wd->config->scrollSpeed, timerTick, NULL);
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
        
        pageDepth = 2;
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
        textScrollTimer = app_timer_register(wd->config->scrollSpeed, timerTick, NULL);
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


static void inbox_dropped_callback(AppMessageResult reason, void *context) 
{
  APP_LOG(APP_LOG_LEVEL_ERROR, "ERROR: Message dropped. Reason: %d - %s", reason, translate_error(reason));
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) 
{
  APP_LOG(APP_LOG_LEVEL_ERROR, "ERROR: Appmessage send failed. Reason: %d - %s", reason, translate_error(reason));
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) 
{
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
    int screenHeight = 168;
    
    //status bar shows up by default on aplite, have to subtract it from our total space
    #ifndef PBL_COLOR
        screenHeight = screenHeight - 16;
    #endif
    myMenuLayer = menu_layer_create(GRect(0, 0, 144, screenHeight));
    //removed this as we need a custom back button function
    //menu_layer_set_click_config_onto_window(myMenuLayer, window);
    
    //remove that ugly shadow
    scroll_layer_set_shadow_hidden(menu_layer_get_scroll_layer(myMenuLayer), 1);
    
    Config* config = (Config*)malloc(sizeof(Config));
    loadPersistentConfig(config);
    
    WindowData* wd = createWindowData(config);
    window_set_user_data(window, wd);
    window_set_background_color(window, config->backgroundColor);
    #ifdef PBL_COLOR
        menu_layer_set_normal_colors(myMenuLayer, config->backgroundColor, config->foregroundColor);
    #endif
    pageDepth = 1;
    MenuLayerCallbacks callbacks = 
    {
        .draw_row = (MenuLayerDrawRowCallback) draw_row_callback,
        .draw_header = (MenuLayerDrawHeaderCallback) draw_header_callback,
        .get_num_sections = (MenuLayerGetNumberOfSectionsCallback)menu_get_num_sections_callback,
        .get_num_rows = (MenuLayerGetNumberOfRowsInSectionsCallback) num_rows_callback,
        .get_cell_height = (MenuLayerGetCellHeightCallback) cell_height_callback,
        .get_header_height = (MenuLayerGetHeaderHeightCallback) menu_get_header_height_callback
    };
    menu_layer_set_callbacks(myMenuLayer, NULL, callbacks);

    window_set_click_config_provider(window, (ClickConfigProvider) config_provider);
    
    layer_add_child(window_get_root_layer(window), menu_layer_get_layer(myMenuLayer));
    

}
 
void window_unload(Window *window)
{
    WindowData* wd = (WindowData*)window_get_user_data(window);
    menu_layer_destroy(myMenuLayer);
    if (wd != 0)
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
    WindowData* wd = (WindowData*)window_get_user_data(window);
    if (wd->config)
        savePersistentConfig(wd->config);
    window_destroy(window);
}

int main(void)
{
    init();
    app_event_loop();
    deinit();
}