#include "../lib/fileIO/fileIO.h"
#include "../lib/appShellLinker/appShellLinker.h"
#include "../lib/constant.c"

int main() {
	char args[SECTOR_SIZE], buffer[SECTOR_SIZE * 16];

	getArgs(args);
	
	readFile(filePath, buffer);
	editor(buffer);



	return 0;
}

void editor(char * buffer) {

}