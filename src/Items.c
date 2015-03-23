#include <pebble.h>
#include "Items.h"

ItemStruct* createEmptyItemList()
{
    ItemStruct* itemList = (ItemStruct*)malloc(sizeof(ItemStruct));
    itemList->items = 0;
    itemList->itemIDs = 0;
    itemList->length = 0;
    return itemList;
}
ItemStruct* createItemList(char** items, char** itemIDs, int length)
{
    ItemStruct* itemList = (ItemStruct*)malloc(sizeof(ItemStruct));
    itemList->items = items;
    itemList->itemIDs = itemIDs;
    itemList->checked = (int*)calloc(sizeof(int),length);
    itemList->length = length;
    return itemList;
}
void destroyItemList(ItemStruct* is)
{
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

void setChecked(ItemStruct* is, int index)
{
    is->checked[index] = 1;
}

void unSerializeItemsString(ItemStruct* itemList, char* itemNamesString, char* itemIDsString)
{
    itemList->items = splitString(itemNamesString, '|', &itemList->length);
    itemList->itemIDs = splitString(itemIDsString, '|', &itemList->length);
    outputArrayContents(itemList->items, itemList->length);
    outputArrayContents(itemList->itemIDs, itemList->length);
}