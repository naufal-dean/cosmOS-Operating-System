#ifndef STRING_H
#define STRING_H

void clear(char *buffer, int length);
int stringCmp(char * buff1, char * buff2);
void stringCpy(char * buffOut, char * buffIn);
int stringStartsWith(char * buffFull, char * buffInit);
int filenameCmp(char * buff1, char * buff2);
int findFilename(char * files, char * filename, char parentIndex, int isFolder);
char* filenameFromIdx(char * files, int idx);
int isFolderExist(char * files, char* name, int * fileIdx);

#endif // STRING_H