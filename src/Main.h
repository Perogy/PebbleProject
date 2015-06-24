#ifndef _MAIN_H_
#define _MAIN_H_
    
#include <pebble.h>

#define PROJECT_NAMES 0
#define PROJECT_IDs 1
#define SELECTED_PROJECT 2
#define ITEM_NAMES 3
#define ITEM_IDS 4
#define SELECTED_ITEM 5 
//informs us that we are waiting for javascript to complete something (generally means to show a "loading" window).
#define WAITING 6
//an error happened in javascript
#define ERROR 7
#define CONFIG 8
#define TIMELINE_JSON 9
#define ERRORMSG 10
    
#define TEXT_SCROLL_INTERVAL 150

#define CIRCLE_RADIUS 8

extern Window* window;
extern MenuLayer* myMenuLayer;

extern int scrolledNumber;
extern bool scrollable;
extern int pageDepth;
extern int selectedProjectIndex;

extern AppTimer* textScrollTimer;

void sendProjectIDToPhone(int index);
void cleanupCheckboxList();
void sendItemIDToPhone(int index);
void draw_row_callback(GContext *ctx, Layer *cell_layer, MenuIndex *cell_index, void *callback_context);
uint16_t num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *callback_context);
void checkCheckbox(GContext *ctx, Layer* cell_layer, int index);
void config_provider(Window *window);
void timerTick(void* data);
void window_load(Window *window);
void window_unload(Window *window);
void init();
void deinit();
void scrollTextBackToStart();

#endif