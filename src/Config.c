#include <pebble.h>
#include "Config.h"
#include "CustomFunctions.h"

void initDefaults(Config* config)
{
    config->scrollSpeed = 150;
    #ifdef PBL_COLOR
        config->backgroundColor = GColorLightGray;
        config->foregroundColor = GColorBlack;
        config->altBackgroundColor = GColorDarkGray;
        config->altForegroundColor = GColorWhite;
        config->highlightBackgroundColor = GColorRed;
        config->highlightForegroundColor = GColorWhite;
    #else
        config->backgroundColor = GColorWhite;
        config->foregroundColor = GColorBlack;
        config->altBackgroundColor = GColorWhite;
        config->altForegroundColor = GColorBlack;
        config->highlightBackgroundColor = GColorBlack;
        config->highlightForegroundColor = GColorWhite;
    #endif
}

void setConfig(char* configString, Config* config)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "\nIn setConfig");
    int* len = (int*)malloc(sizeof(int));
    char** configArray = splitString(configString, '|', len);
    
    outputArrayContents(configArray, *len);
        
    config->scrollSpeed = atoi(configArray[0]);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "\nscrollspeed set success");

    #ifdef PBL_COLOR
        APP_LOG(APP_LOG_LEVEL_DEBUG, "\nbackground color string is: %s", configArray[1]);
        config->backgroundColor = GColorFromHEX(HexStringToUInt(configArray[1]));
        config->foregroundColor = GColorFromHEX(HexStringToUInt(configArray[2]));
        config->altBackgroundColor = GColorFromHEX(HexStringToUInt(configArray[3]));
        config->altForegroundColor = GColorFromHEX(HexStringToUInt(configArray[4]));
        config->highlightBackgroundColor = GColorFromHEX(HexStringToUInt(configArray[5]));
        config->highlightForegroundColor = GColorFromHEX(HexStringToUInt(configArray[6]));
    #else
        config->backgroundColor = GColorWhite;
        config->foregroundColor = GColorBlack;
        config->altBackgroundColor = GColorWhite;
        config->altForegroundColor = GColorBlack;
        config->highlightBackgroundColor = GColorBlack;
        config->highlightForegroundColor = GColorWhite;
    #endif
    APP_LOG(APP_LOG_LEVEL_DEBUG, "\nset colors");
}

void savePersistentConfig(Config* config)
{
    persist_write_data(CONFIG_KEY, config, sizeof(*config));
}

void loadPersistentConfig(Config* config)
{
    persist_delete(CONFIG_KEY);
    if (persist_exists(CONFIG_KEY))
    {
        persist_read_data(CONFIG_KEY, config, sizeof(*config));
    }
    else
    {
        initDefaults(config);
    }
}