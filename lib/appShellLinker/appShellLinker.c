#include "appShellLinker.h"

void setArgs(char * args) {
	writeSector_intr(args, ARGS_SECTOR);
}

void getArgs(char * args) {
	readSector_intr(args, ARGS_SECTOR);
}

void setCurDir(char * curDir) {
	writeSector_intr(curDir, CUR_DIR_SECTOR);
}

void getCurDir(char * curDir) {
	readSector_intr(curDir, CUR_DIR_SECTOR);
}

void setParIdx(int * parIdx) {
	char temp[100];
	intToStr(*parIdx, temp);
	writeSector_intr(temp, PAR_IDX_SECTOR);
}

void getParIdx(int * parIdx) {
	char temp[100];
	readSector_intr(temp, PAR_IDX_SECTOR);
	*parIdx = strToInt(temp);
}

void backToShell() {
	int success;
	executeProgram_intr("shell", 0x2000, &success, 0x01);
}
