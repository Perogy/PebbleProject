#include <pebble.h>
#include "Items.h"

ItemStruct* createEmptyItemList()
{
    ItemStruct* itemList = (ItemStruct*)malloc(sizeof(ItemStruct));
    itemList->items = 0;
    itemList->itemIDs = 0;
    itemList->length = 0;
    itemList->checked = (bool*)malloc(sizeof(bool));
    return itemList;
}
ItemStruct* createItemList(char** items, char** itemIDs, int length)
{
    ItemStruct* itemList = (ItemStruct*)malloc(sizeof(ItemStruct));
    itemList->items = items;
    itemList->itemIDs = itemIDs;
    itemList->checked = (bool*)calloc(sizeof(bool),length);
    itemList->length = length;
    return itemList;
}
void destroyItemList(ItemStruct* is)
{
    if (is == 0)
        return;
    
    for(int i=0;i < is->length;i++)
    {
        free(is->items[i]);
        free(is->itemIDs[i]);
    }
    free(is->items);
    free(is->itemIDs);
    free(is->checked);
    free(is);
}

void unSerializeItemsString(ItemStruct* itemList, char* itemNamesString, char* itemIDsString)
{
    itemList->items = splitString(itemNamesString, '|', &itemList->length);
    itemList->itemIDs = splitString(itemIDsString, '|', &itemList->length);
    itemList->checked = (bool*)realloc(itemList->checked, sizeof(bool)*itemList->length);
    for (int i=0;i < itemList->length;i++)
    {
        itemList->checked[i] = 0;    
    }
}