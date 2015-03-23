#ifndef _CUSTOMFUNCTIONS_H_
#define _CUSTOMFUNCTIONS_H_

char* getSubString(char* str, int startIndex, int endIndex);
void outputArrayContents(char** array, int arrayLength);
char** splitString(char* str, char delimiter, int* length);
void scrollTextByOneLetter(char* text);
void loopingCopy(char* dst, char* src, int startIndex, int length);

#endif