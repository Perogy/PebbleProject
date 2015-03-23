#include <pebble.h>
#include "WindowData.h"

WindowData* createWindowData(ProjectStruct* projects)
{
    WindowData* wd = (WindowData*)malloc(sizeof(WindowData));
    wd->projects = projects;
    wd->items = 0;
    wd->currentPage = 1;
    return wd;
}

void destroyWindowData(WindowData* wd)
{
    free(wd->projects);
    free(wd->items);
}