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
        
    //converts the number from 1-10 to its millisecond value, 10 is fastest 1 is slowest,
    //therefore 10 is 50 milliseconds, 1 is 500 milliseconds
    config->scrollSpeed = (1.0/atoi(configArray[0]))*500;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "\nscrollspeed set success, it is: %d", config->scrollSpeed);

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
    if (persist_exists(CONFIG_KEY))
    {
        persist_read_data(CONFIG_KEY, config, sizeof(*config));
    }
    else
    {
        initDefaults(config);
    }
}