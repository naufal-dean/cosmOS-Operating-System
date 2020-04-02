#include "appShellLinker.h"

void setArgs(char * args) {
	writeSector_intr(args, 0x104);
}

void getArgs(char * args) {
	readSector_intr(args, 0x104);
}

void backToShell() {
	int success;
	executeProgram_intr("shell", 0x2000, &success, 0x01);
}