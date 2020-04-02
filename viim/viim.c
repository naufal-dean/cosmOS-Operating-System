#include "viim.h"

int main() {
	char args[SECTOR_SIZE], buffer[SECTOR_SIZE * 16];

	getArgs(args);
	print("test");


	// readFile(filePath, buffer);
	// editor(buffer);
	// writeFile(filePath, buffer);

	backToShell();
	return 0;
}

void editor(char * buffer) {
	int exit = 0;
	while (!exit) {

	}
}