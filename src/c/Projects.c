#include <pebble.h>
#include "Projects.h"

ProjectStruct* createEmptyProjectList()
{
    ProjectStruct* projectList = (ProjectStruct*)malloc(sizeof(ProjectStruct));
    projectList->projects = 0;
    projectList->projectIDs = 0;
    projectList->indentation = 0;
    projectList->length = 0;
    return projectList;
}
ProjectStruct* createProjectList(char** projects, char** projectIDs, char** indentation, int length)
{
    ProjectStruct* projectList = (ProjectStruct*)malloc(sizeof(ProjectStruct));
    projectList->projects = projects;
    projectList->projectIDs = projectIDs;
    projectList->indentation = indentation;
    projectList->length = length;
    return projectList;
}
void destroyProjectList(ProjectStruct* ps)
{
    if (ps==0)
        return;
    
    for(int i=0;i < ps->length;i++)
    {
        free(ps->projects[i]);
        free(ps->projectIDs[i]);
        free(ps->indentation[i]);
    }
    free(ps->projects);
    free(ps->projectIDs);
    free(ps->indentation);
    free(ps);
}

void unSerializeProjectsString(ProjectStruct* projectList, char* projectNamesString, char* projectIDsString, char* projectIndentationString)
{
    projectList->projects = splitString(projectNamesString, '|', &projectList->length);
    projectList->projectIDs = splitString(projectIDsString, '|', &projectList->length);
    projectList->indentation = splitString(projectIndentationString, '|', &projectList->length);
}
