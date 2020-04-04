#ifndef APP_SHELL_LINKER_H
#define APP_SHELL_LINKER_H

#define FILES_SECTOR_1 0x101
#define FILES_SECTOR_2 0x102
#define ARGS_SECTOR 0x200
#define CUR_DIR_SECTOR 0x201
#define PAR_IDX_SECTOR 0x202

#define MAX_ARGC 20
#define MAX_ARG_LEN 50

#include "../interrupt_define.c"

void setArgs(char * args);
void getArgs(char * args);
void getParsedArgs(char argv[MAX_ARGC][MAX_ARGV_LEN], int * argc);
void setCurDir(char * curDir);
void getCurDir(char * curDir);
void setFiles(char * files);
void getFiles(char * files);
void setParIdx(char * parIdx);
void getParIdx(char * parIdx);
void absPathParser(char * newAbsPath, char * absPath);
void backToShell();

#endif // APP_SHELL_LINKER_H
