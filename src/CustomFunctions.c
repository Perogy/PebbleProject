#include <pebble.h>

char* getSubString(char* str, int startIndex, int endIndex)
{
    char* newStr = (char*)malloc(endIndex - startIndex + 1);
    int count = 0;
    for(int i = startIndex; i<endIndex; i++)
    {
        newStr[count] = str[i];
        count++;
    }
    newStr[count+1] = '\0';
    return newStr;
}

void outputArrayContents(char** array, int arrayLength)
{
    for(int i=0; i<arrayLength; i++)
    {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "\nArray Element %d: %s\n", i, array[i]);
    }
}
//return an array of items which have been split by delimiter
char** splitString(char* str, char delimiter, int* length)
{
    //this 1000 needs to be changed to be more dynamic.
    char** splitArray = (char**)malloc(1000);
    int previousDelimiterIndex = 0;
    int tokenCount = 0;
            
    for(int i=0; i<(int)strlen(str); i++)
    {
        //if we hit the delimiter or end of string execute below code
        if ((str[i] == delimiter) || (i == ((int)strlen(str)-1)))
        {
            tokenCount++;
            //splitArray = (char**)realloc(splitArray, sizeof(char*)*(tokenCount+1));
            splitArray[tokenCount-1] = (char*)malloc((i-previousDelimiterIndex) + 1);
            //memset(splitArray[tokenCount-1], '\0', (i-previousDelimiterIndex) + 1);
            //(i-previousDelimiterIndex) + 1
            char* derp = getSubString(str, previousDelimiterIndex, i);
            splitArray[tokenCount-1] = strcpy(splitArray[tokenCount-1], derp);
            outputArrayContents(splitArray, tokenCount);
            previousDelimiterIndex = i+1;
        }
    }
    *length = tokenCount;
    
    return splitArray;
}

void scrollTextByOneLetter(char* text)
{
    int length = strlen(text);
    for(int i=0;i < length;i++)
    {
        char tmp = text[i];
        text[i] = text[i+1];
        text[length-1] = tmp;
    }
}