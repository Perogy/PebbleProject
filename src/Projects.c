#include <pebble.h>
#include "Projects.h"

ProjectStruct* createEmptyProjectList()
{
    ProjectStruct* projectList = (ProjectStruct*)malloc(sizeof(ProjectStruct));
    projectList->projects = 0;
    projectList->projectIDs = 0;
    projectList->length = 0;
    return projectList;
}
ProjectStruct* createProjectList(char** projects, char** projectIDs, int length)
{
    ProjectStruct* projectList = (ProjectStruct*)malloc(sizeof(ProjectStruct));
    projectList->projects = projects;
    projectList->projectIDs = projectIDs;
    projectList->length = length;
    return projectList;
}
void destroyProjectList(ProjectStruct* ps)
{
    for(int i=0;i < ps->length;i++)
    {
        free(ps->projects[i]);
        free(ps->projectIDs[i]);
    }
    free(ps->projects);
    free(ps->projectIDs);
    free(ps);
}

void unSerializeProjectsString(ProjectStruct* projectList, char* projectNamesString, char* projectIDsString)
{
    projectList->projects = splitString(projectNamesString, '|', &projectList->length);
    projectList->projectIDs = splitString(projectIDsString, '|', &projectList->length);
    outputArrayContents(projectList->projects, projectList->length);
    outputArrayContents(projectList->projectIDs, projectList->length);
}
