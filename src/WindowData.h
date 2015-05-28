#ifndef _WINDOWDATA_H_
#define _WINDOWDATA_H_

#include <pebble.h>
#include "Items.h"
#include "Projects.h"
#include "Config.h"

typedef struct mainWindowData
{
    ProjectStruct* projects;
    ItemStruct* items;
    Config* config;
    int currentPage;
} WindowData;

WindowData* createWindowData(Config* config);
void setProjects(WindowData* wd, ProjectStruct* projects);
void destroyWindowData(WindowData* wd);

#endif