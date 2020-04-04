#include "viim.h"

int main() {
	char buffer[SECTOR_SIZE * 16], curDir[SECTOR_SIZE], absPath[SECTOR_SIZE * 2], newAbsPath[SECTOR_SIZE * 2];
    char argv[MAX_ARGC][MAX_ARG_LEN];
	int i, argc;

    // Check args
    getParsedArgs(argv, &argc);
    if (argc < 1) {
        print("viim: no input file\r\n");
        backToShell();
    } if (argc > 1) {
    	print("viim: too many arguments\r\n");
        backToShell();
    }

    // Get curDir
    getCurDir(curDir);
    i = 0;
    while (curDir[i] != 0x0) i++;
    curDir[i] = '/';
    curDir[i + 1] = 0x0;

    // Pass to editor
    stringConcat(absPath, curDir + 2, argv[0]);
    absPathParser(newAbsPath, absPath);
    clear(buffer, SECTOR_SIZE * 16);
    if (readFile(newAbsPath, buffer) == R_SUCCESS) {
		editor(buffer);
        // Update file
        deleteFile(newAbsPath);
        writeFile(newAbsPath, buffer);
    } else {
        print("viim: ");
		print(argv[0]);
		print(": file not found\r\n");
    }

	backToShell();
	return 0;
}

void editor(char * buffer) {
    inputEditable(buffer);    
}