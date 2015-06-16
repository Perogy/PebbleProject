#include <pebble.h>
#include "WindowData.h"

WindowData* createWindowData(Config* config)
{
    WindowData* wd = (WindowData*)malloc(sizeof(WindowData));
    wd->projects = 0;
    wd->items = 0;
    wd->timelineItems = 0;
    wd->config = config;
    wd->currentPage = 1;
    return wd;
}

void setProjects(WindowData* wd, ProjectStruct* projects)
{
    wd->projects = projects;
}

void destroyWindowData(WindowData* wd)
{
    free(wd->projects);
    free(wd->items);
    free(wd->timelineItems);
    free(wd->config);
}