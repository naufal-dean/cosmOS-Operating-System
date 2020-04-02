#ifndef FILE_IO_H
#define FILE_IO_H

#define D_SUCCESS 1
#define D_FILE_NOT_FOUND -1

#include "../constant.c"
#include "../interrupt_define.c"
#include "../string/string.h"

int readFile(char * filePath, char * buffer);
int writeFile(char * filePath, char * buffer);
int deleteFile(char * filePath);

#endif // FILE_IO_H
