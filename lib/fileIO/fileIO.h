#ifndef FILE_IO_H
#define FILE_IO_H

#include "../constant.c"
#include "../interrupt_define.c"
#include "../string/string.h"

int readFile(char * filename, char * buffer);
int writeFile(char * filename, char * buffer);
int deleteFile(char * filename);

#endif // FILE_IO_H