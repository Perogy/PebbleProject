#pragma once

#include <pebble.h>
#include "CustomFunctions.h"
//The current list of all the items
typedef struct item
{
    char** items;
    char** itemIDs;
    bool* checked;
    int length;
} ItemStruct;

ItemStruct* createEmptyItemList();
ItemStruct* createItemList(char** items, char** itemIDs, int length);
void destroyItemList(ItemStruct* is);
void unSerializeItemsString(ItemStruct* itemList, char* itemNamesString, char* itemIDsString);
void setChecked(ItemStruct* is, int index);