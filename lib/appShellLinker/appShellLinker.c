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

void setParIdx(char * parIdx) {
	writeSector_intr(parIdx, PAR_IDX_SECTOR);
}

void getParIdx(char * parIdx) {
	readSector_intr(parIdx, PAR_IDX_SECTOR);
}

void backToShell() {
	int success;
	executeProgram_intr("shell", 0x2000, &success, 0x01);
}
