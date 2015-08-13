#pragma once
    
#include <pebble.h>

#define TEXT_SCROLL_INTERVAL 150

#define CIRCLE_RADIUS 8

extern Window* window;
extern MenuLayer* myMenuLayer;

extern int scrolledNumber;
extern bool scrollable;
extern int selectedProjectIndex;

extern AppTimer* textScrollTimer;

void cleanupCheckboxList();

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