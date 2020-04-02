#include "folderIO.h"

int createFolder(char * folderPath) {
	char files[SECTOR_SIZE * 2], partPath[SECTOR_SIZE];
	int i, j, unusedFilesIdx, prevOffset, parentIdx;

	// Validate folder folderPath
	clear(partPath, SECTOR_SIZE);
	i = 0; 
	prevOffset = 0; 
	parentIdx = 0xFF;
	while (folderPath[i] != 0x00) {
		if (folderPath[i] == '/') {
			for (j = prevOffset; j < i; j++) {
				partPath[j-prevOffset] = folderPath[j];
			}

			//check if folder exist. If exists, use the existing folder's idx
			if ((parentIdx = findFilename(files, partPath, parentIdx, IS_FOLDER)) == -1) {
				printString_intr("Invalid folder\r\n");
				return W_INVALID_FOLDER;
			}

			clear(partPath, SECTOR_SIZE);
			prevOffset = i + 1;
		}
		i++;
	}

	// Validate folderPath
	if (findFilename(files, folderPath + prevOffset, parentIdx, IS_FOLDER) != -1) {
		printString_intr("Folder already exist\r\n");
		return W_FILE_ALREADY_EXIST;
	}

	// Write folder
	// Get files
	readSector_intr(files, 0x101);
	readSector_intr(files + SECTOR_SIZE, 0x102);

	//check for empty file
	i = 0;
	do {
		if (files[i * 16] == 0x00) break;
		i++;
	} while (i < FILE_MAX_COUNT);

	if (i == FILE_MAX_COUNT) { //NOT FOUND, keluarkan pesan error -2
		printString_intr("Entry full\r\n");
		return W_ENTRY_FULL;
	}
	unusedFilesIdx = i;

	// Write folderName
	i = 0;
	while (folderPath[i] != 0x0 && i < 14) {
		files[unusedFilesIdx * FILES_LINE_SIZE + 2 + i] = folderPath[prevOffset + i];
		i++;
	}

	// Write P and S
	SECTOR(files + unusedFilesIdx * FILES_LINE_SIZE) = 0xFF;
	PARENT(files + unusedFilesIdx * FILES_LINE_SIZE) = parentIdx;

	// Write sector
	writeSector_intr(files, 0x101);
	writeSector_intr(files + SECTOR_SIZE, 0x102);

	return W_SUCCESS;
}

int deleteFolder(char * folderPath) {
	char files[SECTOR_SIZE * 2], partPath[SECTOR_SIZE];
	int i, j, filesIdx,;
	
	// Get files
	readSector_intr(files, 0x101);
	readSector_intr(files + SECTOR_SIZE, 0x102);

	// Find folderName
	i = 0;
	filesIdx = 0xFF;
	while (folderPath[i] != 0x0) {
		clear(partPath, SECTOR_SIZE);
		// Copy part path
		j = 0;
		while(folderPath[i] != 0x0 && folderPath[i] != '/') {
			partPath[j] = folderPath[i];
			i++; j++;
		}

		// Check if file not found
		if ((filesIdx = findFilename(files, partPath, filesIdx, IS_FOLDER)) == -1) {
			printString_intr("Folder not found\r\n");
			return D_FOLDER_NOT_FOUND;
		}
	}

	// Clear files
	clear(files + filesIdx * FILES_LINE_SIZE, FILES_LINE_SIZE);

	// Write back map, files, and sectors
	writeSector_intr(files, 0x101);
	writeSector_intr(files + SECTOR_SIZE, 0x102);

	return D_SUCCESS;
}

int listFolderContent(char * folderPath) {
	char files[SECTOR_SIZE * 2], partPath[SECTOR_SIZE], contentName[20];
	int i, j, filesIdx,;
	
	// Get files
	readSector_intr(files, 0x101);
	readSector_intr(files + SECTOR_SIZE, 0x102);

	// Find folderName
	i = 0;
	filesIdx = 0xFF;
	while (folderPath[i] != 0x0) {
		clear(partPath, SECTOR_SIZE);
		// Copy part path
		j = 0;
		while(folderPath[i] != 0x0 && folderPath[i] != '/') {
			partPath[j] = folderPath[i];
			i++; j++;
		}

		// Check if file not found
		if ((filesIdx = findFilename(files, partPath, filesIdx, IS_FOLDER)) == -1) {
			printString_intr("Folder not found\r\n");
			return L_FOLDER_NOT_FOUND;
		}
	}

	// Print folder content
	i = 0;
	do {
		if (files[i * FILES_LINE_SIZE] == 0x00) break;
		// Print content if parentIdx match
		if (PARENT(files + i * FILES_LINE_SIZE) == filesIdx) {
			if (SECTOR(files + i * FILES_LINE_SIZE) == 0xFF) // folder
				printString_intr("[file]  : ");
			else // file
				printString_intr("[folder]: ");
			j = 0;
			while (files[i * FILES_LINE_SIZE + 2 + j] != 0x0 && j < 14) {
				contentName[j] = files[i * FILES_LINE_SIZE + 2 + j];
				j++;
			}
			contentName[j] = 0x0;
			printString_intr(contentName); printString_intr("\r\n");
		}
		i++;
	} while (i < FILE_MAX_COUNT);

	return L_SUCCESS;
}
