#include "fileIO.h"

int readFile(char * filePath, char * buffer) {
	int result;
	readFile_intr(buffer, filePath, &result, 0xFF);
	return result;
}

int writeFile(char * filePath, char * buffer) {
	int result;
	writeFile_intr(buffer, filePath, &result, 0xFF);
	return result;
}

int deleteFile(char * filePath) {
	char map[SECTOR_SIZE], files[SECTOR_SIZE * 2], sectors[SECTOR_SIZE], partPath[SECTOR_SIZE], cleanBuffer[SECTOR_SIZE];
	int i, j, filesIdx, sectorsIdx, targetSector, isFolder;
	
	// Get map, files, and sectors
	readSector_intr(map, 0x100);
	readSector_intr(files, 0x101);
	readSector_intr(files + SECTOR_SIZE, 0x102);
	readSector_intr(sectors, 0x103);

	// Find filename
	i = 0;
	filesIdx = 0xFF;
	while (filePath[i] != 0x0) {
		clear(partPath, SECTOR_SIZE);
		// Copy part path
		j = 0;
		while(filePath[i] != 0x0 && filePath[i] != '/') {
			partPath[j] = filePath[i];
			i++; j++;
		}

		// Is folder or file
		if (filePath[i] == '/') {
		  	isFolder = 1; i++;
		} else // filePath[i] == 0x0
		  	isFolder = 0;

		// Check if file not found
		if ((filesIdx = findFilename(files, partPath, filesIdx, isFolder)) == -1) {
			printString_intr("File not found\r\n");
			return D_FILE_NOT_FOUND;
		}
	}

	// Clear map and sectors
	clear(cleanBuffer, SECTOR_SIZE);
  	sectorsIdx = SECTOR(files + filesIdx * FILES_LINE_SIZE);
	i = 0;
	while (sectors[sectorsIdx * SECTOR_LINE_SIZE + i] != 0x0 && i < SECTOR_LINE_SIZE) {
		targetSector = sectors[sectorsIdx * SECTOR_LINE_SIZE + i];
		// Clear map
		map[targetSector] = 0x0;
		// Write clean buffer to sector
		writeSector_intr(cleanBuffer, targetSector);
		i++;
	}
	// Clear sectors
	clear(sectors + sectorsIdx * SECTOR_LINE_SIZE, SECTOR_LINE_SIZE);

	// Clear files
	clear(files + filesIdx * FILES_LINE_SIZE, FILES_LINE_SIZE);

	// Write back map, files, and sectors
	writeSector_intr(map, 0x100);
	writeSector_intr(files, 0x101);
	writeSector_intr(files + SECTOR_SIZE, 0x102);
	writeSector_intr(sectors, 0x103);

	return D_SUCCESS;
}

