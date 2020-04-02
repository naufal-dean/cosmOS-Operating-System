#include "viim.h"

int main() {
	char args[SECTOR_SIZE], buffer[SECTOR_SIZE * 16], filePath[SECTOR_SIZE];
	int i;

	print("Inside viim test\r\n");

	// Get filePath
	getArgs(args);
	i = 0;
	while (args[i] != 0x0 && args[i] != ' ') {
		filePath[i] = args[i];
		i++;
	}
	filePath[i] = 0x0;


	// Pass to editor
	if (readFile(filePath, buffer) > 0) {
		editor(buffer);
	} else {
		print(filePath);
		print(": File not found\r\n");
	}
	// writeFile(filePath, buffer);

	backToShell();
	return 0;
}

void editor(char * buffer) {
	int exit = 0;
	print(buffer);
	// while (!exit) {

	// }
}