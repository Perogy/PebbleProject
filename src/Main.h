#ifndef _MAIN_H_
#define _MAIN_H_
    
#include <pebble.h>
#define PROJECT_NAMES 0
#define PROJECT_IDs 1
#define SELECTED_ITEM 2
#define ITEM_NAMES 3
#define ITEM_IDS 4

Window* window;
MenuLayer* myMenuLayer;

//the current list of all the projects
typedef struct proj
{
    char** projects;
    char** projectIDs;
    int length;
} projectStruct;

//The current list of all the items
typedef struct item
{
    char** items;
    char** itemIDs;
    int length;
} itemStruct;

typedef struct layer
{
    Layer** checkboxLayer;
    GRect** checkboxFrame;
    GRect** checkboxRect;
    int length;
} itemCheckboxLayer;

projectStruct* projectList;
itemStruct* itemList;
itemCheckboxLayer* checkboxList;

void sendProjectIDToPhone(int index);
void draw_row_callback(GContext *ctx, Layer *cell_layer, MenuIndex *cell_index, void *callback_context);
uint16_t num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *callback_context);
void unSerializeProjectsString(char* projectNamesString, char* projectIDsString);
void unSerializeItemsString(char* itemNamesString, char* itemIDsString);
void config_provider(Window *window);
void window_load(Window *window);
void window_unload(Window *window);
void init();
void deinit();

#endif