#ifndef _WINDOWDATA_H_
#define _WINDOWDATA_H_

#include <pebble.h>
#include "Items.h"
#include "Projects.h"

typedef struct mainWindowData
{
    ProjectStruct* projects;
    ItemStruct* items;
    int currentPage;
} WindowData;

WindowData* createWindowData(ProjectStruct* projects);
void destroyWindowData(WindowData* wd);

#endif