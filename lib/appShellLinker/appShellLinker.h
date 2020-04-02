#ifndef APP_SHELL_LINKER_H
#define APP_SHELL_LINKER_H

#define ARGS_SECTOR 0x200
#define CUR_DIR_SECTOR 0x201
#define PAR_IDX_SECTOR 0x202

#include "../interrupt_define.c"
#include "../math/math.h"

void setArgs(char * args);
void getArgs(char * args);
void setCurDir(char * curDir);
void getCurDir(char * curDir);
void setParIdx(int * parIdx);
void getParIdx(int * parIdx);
void backToShell();

#endif // APP_SHELL_LINKER_H
