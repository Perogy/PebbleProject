#pragma once

#include <pebble.h>
#include "CustomFunctions.h"
#include <wchar.h>
//the current list of all the projects
typedef struct proj
{
    wchar_t** projects;
    char** projectIDs;
    char** indentation;
    int length;
} ProjectStruct;

ProjectStruct* createEmptyProjectList();
ProjectStruct* createProjectList(wchar_t** projects, char** projectIDs, char** indentation, int length);
void destroyProjectList(ProjectStruct* ps);
void unSerializeProjectsString(ProjectStruct* projectList, wchar_t* projectNamesString, char* projectIDsString, char* projectIndentationString);