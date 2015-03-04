#ifndef _CUSTOMFUNCTIONS_H_
#define _CUSTOMFUNCTIONS_H_

#include <pebble.h>

char* getSubString(char* str, int startIndex, int endIndex);
void outputArrayContents(char** array, int arrayLength);
char** splitString(char* str, char delimiter, int* length);
void scrollTextByOneLetter(char* text);

#endif