#ifndef FOLDER_IO_H
#define FOLDER_IO_H

#define D_FOLDER_SUCCESS 1
#define D_FOLDER_NOT_FOUND -1
#define L_SUCCESS 1
#define L_FOLDER_NOT_FOUND -1
#define LF_MAX_ROW 30
#define LF_MAX_COL 25

#include "../constant.c"
#include "../interrupt_define.c"
#include "../string/string.h"

int createFolder(char * folderName);
int deleteFolder(char * folderName);
int listFolderContent(char * folderPath, char listFile[LF_MAX_ROW][LF_MAX_COL]);

#endif // FOLDER_IO_H
