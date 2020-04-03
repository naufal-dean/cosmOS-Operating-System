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
	int i, j, filesIdx, lastFilesIdx, sectorsIdx, targetSector, lastSectorsIdx, isFolder;
	
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
			// printString_intr("File not found\r\n");
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
	// Replace cleaned sectors line with latest sectors line if any
	lastSectorsIdx = sectorsIdx;
	while (sectors[(lastSectorsIdx + 1) * SECTOR_LINE_SIZE] != 0x0 && (lastSectorsIdx + 1) < SECTOR_MAX_COUNT) lastSectorsIdx++;
	if (lastSectorsIdx > sectorsIdx) { // Found sectors line below
		// Swap line
		for (j = 0; j < SECTOR_LINE_SIZE; j++) {
			sectors[sectorsIdx * SECTOR_LINE_SIZE + j] = sectors[lastSectorsIdx * SECTOR_LINE_SIZE + j];
		}
		clear(sectors + lastSectorsIdx * SECTOR_LINE_SIZE, SECTOR_LINE_SIZE);
		// Update other file's sector index that refer to the swapped sectors line
		i = 0;
		while (files[i * FILES_LINE_SIZE] != 0x0 && i < SECTOR_MAX_COUNT) {
			if (SECTOR(files + i * FILES_LINE_SIZE) == lastSectorsIdx)
				SECTOR(files + i * FILES_LINE_SIZE) = sectorsIdx;
			i++;
		}
	} else { // Sector line to be cleaned is latest
		clear(sectors + sectorsIdx * SECTOR_LINE_SIZE, SECTOR_LINE_SIZE);
	}

	// Clear files
	// Replace cleaned files line with latest files line if any
	lastFilesIdx = filesIdx;
	while (files[(lastFilesIdx + 1) * FILES_LINE_SIZE] != 0x0 && (lastFilesIdx + 1) < FILE_MAX_COUNT) lastFilesIdx++;
	if (lastFilesIdx > filesIdx) { // Found files line below
		// Swap line
		for (j = 0; j < FILES_LINE_SIZE; j++) {
			files[filesIdx * FILES_LINE_SIZE + j] = files[lastFilesIdx * FILES_LINE_SIZE + j];
		}
		clear(files + lastFilesIdx * FILES_LINE_SIZE, FILES_LINE_SIZE);
		// Update other file's parent index that refer to the swapped files line
		i = 0;
		while (files[i * FILES_LINE_SIZE] != 0x0 && i < FILE_MAX_COUNT) {
			if (PARENT(files + i * FILES_LINE_SIZE) == lastFilesIdx)
				PARENT(files + i * FILES_LINE_SIZE) = filesIdx;
			i++;
		}
	} else { // Files line to be cleaned is latest
		clear(files + filesIdx * FILES_LINE_SIZE, FILES_LINE_SIZE);
	}

	// Write back map, files, and sectors
	writeSector_intr(map, 0x100);
	writeSector_intr(files, 0x101);
	writeSector_intr(files + SECTOR_SIZE, 0x102);
	writeSector_intr(sectors, 0x103);

	return D_FILE_SUCCESS;
}

