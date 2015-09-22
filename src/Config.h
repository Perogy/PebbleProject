#pragma once

#include <pebble.h>
    
#define CONFIG_KEY 1
#define MESSAGE_KEY 2
    
typedef struct conf
{
    //scrollspeed in milliseconds
    int scrollSpeed;
    GColor backgroundColor;
    GColor foregroundColor;
    GColor altBackgroundColor;
    GColor altForegroundColor;
    GColor highlightBackgroundColor;
    GColor highlightForegroundColor;
} Config;

void initDefaults(Config* config);
void setConfig(char* configString, Config* config);
void loadPersistentConfig(Config* config);
void savePersistentConfig(Config* config);
bool loadMessageShown();
void saveMessageShown();