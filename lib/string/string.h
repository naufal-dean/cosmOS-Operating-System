#ifndef STRING_H
#define STRING_H

#include "../constant.c"

void clear(char *buffer, int length);
int stringCmp(char * buff1, char * buff2);
void stringCpy(char * buffOut, char * buffIn);
int stringStartsWith(char * buffFull, char * buffInit);
int filenameCmp(char * buff1, char * buff2);
int findFilename(char * files, char * filename, char parentIndex, int isFolder);

#endif // STRING_H