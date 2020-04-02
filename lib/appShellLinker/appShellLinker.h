#ifndef APP_SHELL_LINKER_H
#define APP_SHELL_LINKER_H

#define ARGS_SECTOR 0x200
#define CUR_DIR_SECTOR 0x201
#define PAR_IDX_SECTOR 0x202

#include "../interrupt_define.c"

void setArgs(char * args);
void getArgs(char * args);
void setCurDir(char * curDir);
void getCurDir(char * curDir);
void setParIdx(char * parIdx);
void getParIdx(char * parIdx);
void absPathParser(char * newAbsPath, char * absPath);
void backToShell();

#endif // APP_SHELL_LINKER_H
