#pragma once

#include <pebble.h>
#include "CustomFunctions.h"
//The current list of all the items
typedef struct item
{
    char** items;
    char** itemIDs;
    char** itemDates;
    char** itemDueDates;
    char** indentation;
    bool* checked;
    bool* recurringTask;
    int length;
} ItemStruct;

ItemStruct* createEmptyItemList();
ItemStruct* createItemList(char** items, char** itemIDs, char** itemDates, char** itemDueDates, char** itemIndentation, int length);
void destroyItemList(ItemStruct* is);
void unSerializeItemsString(ItemStruct* itemList, char* itemNamesString, char* itemIDsString, char* itemDatesString, char* itemDueDatesString, char* itemIndentationString);
void setChecked(ItemStruct* is, int index);