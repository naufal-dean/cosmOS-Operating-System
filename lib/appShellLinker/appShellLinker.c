#include "appShellLinker.h"

void setArgs(char * args) {
	writeSector_intr(args, ARGS_SECTOR);
}

void getArgs(char * args) {
	readSector_intr(args, ARGS_SECTOR);
}

void getParsedArgs(char argv[MAX_ARGC][MAX_ARG_LEN], int * argc) {
	char args[512];
	int i, j;
	readSector_intr(args, ARGS_SECTOR);
	(*argc) = 0; i = 0;
	while (args[i] != 0x0) {
		while (args[i] == ' ') i++;
		j = 0;
		while (args[i] != ' ' && args[i] != 0x0) {
			argv[*argc][j] = args[i];
			i++; j++;
		}
		argv[*argc][j] = 0x0;
		if (j > 0) (*argc)++; 
	}
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

void absPathParser(char * newAbsPath, char * absPath) {
    char temp[50][30], partPath[50];
    int i, j, k, count, strLen;
    
    // Parse absPath
    strLen = stringLen(absPath);
    count = 0;
    i = 0;
    while (i < strLen) {
        clear(partPath, 50); 
        
        while (absPath[i] == '/') i++;
        
        j = 0;
        while (absPath[i] != '/' && i < strLen) {
            partPath[j] = absPath[i];
            i++; j++;
        }
        partPath[j] == 0x0;
        
        if (stringCmp(partPath, "..")) {
            count = (count > 0) ? (count - 1) : (0);
    	} else if (partPath[0] != 0x0 && !stringCmp(partPath, ".")) {
            stringCpy(temp[count], partPath);
            count++;
        }
        
        i++;
    } 
    // Combine parse result
    i = 0; j = 0;
    while (i < count) {
        k = 0;
        while (temp[i][k] != 0x0) {
            newAbsPath[j] = temp[i][k];
            j++; k++;
        }
        if (i < count - 1) newAbsPath[j++] = '/';
        i++;
    }
    newAbsPath[j] = 0x0;
}

void backToShell() {
	int success;
	executeProgram_intr("shell", 0x2000, &success, 0x01);
}
