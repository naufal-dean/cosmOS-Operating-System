#include "folderIO.h"

int createFolder(char * folderPath) {
	char files[SECTOR_SIZE * 2], partPath[SECTOR_SIZE];
	int i, j, unusedFilesIdx, prevOffset, parentIdx;

	// Get files
	readSector_intr(files, 0x101);
	readSector_intr(files + SECTOR_SIZE, 0x102);

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

			// Check if folder exist
			if ((parentIdx = findFilename(files, partPath, parentIdx, IS_FOLDER)) == -1) {
				// printString_intr("Invalid folder\r\n");
				return W_INVALID_FOLDER;
			}

			clear(partPath, SECTOR_SIZE);
			prevOffset = i + 1;
		}
		i++;
	}

	// Validate folderPath
	if (findFilename(files, folderPath + prevOffset, parentIdx, IS_FILE) != -1 || 
		findFilename(files, folderPath + prevOffset, parentIdx, IS_FOLDER) != -1) {
		// printString_intr("Folder already exist\r\n");
		return W_FILE_ALREADY_EXIST;
	}

	//check for empty file
	i = 0;
	do {
		if (files[i * 16] == 0x00) break;
		i++;
	} while (i < FILE_MAX_COUNT);

	if (i == FILE_MAX_COUNT) {
		// printString_intr("Entry full\r\n");
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
	int i, j, filesIdx, lastFilesIdx;
	
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
		if (folderPath[i] == '/') i++;

		// Check if file not found
		if ((filesIdx = findFilename(files, partPath, filesIdx, IS_FOLDER)) == -1) {
			// printString_intr("Folder not found\r\n");
			return D_FOLDER_NOT_FOUND;
		}
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
	writeSector_intr(files, 0x101);
	writeSector_intr(files + SECTOR_SIZE, 0x102);

	return D_FOLDER_SUCCESS;
}

int listFolderContent(char * folderPath, char listFile[LF_MAX_ROW][LF_MAX_COL]) {
	char files[SECTOR_SIZE * 2], partPath[SECTOR_SIZE], contentName[20];
	int i, j, filesIdx, count;
	
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
		if (folderPath[i] == '/') i++;
		// Check if folder not found
		if ((filesIdx = findFilename(files, partPath, filesIdx, IS_FOLDER)) == -1) {
			return L_FOLDER_NOT_FOUND;
		}
	}

	// Print folder content
	i = 0; count = 0;
	do {
		if (files[i * FILES_LINE_SIZE] == 0x00) break;
		// Print content if parentIdx match
		if (PARENT(files + i * FILES_LINE_SIZE) == filesIdx) {
			j = 0;
			while (files[i * FILES_LINE_SIZE + 2 + j] != 0x0 && j < 14) {
				contentName[j] = files[i * FILES_LINE_SIZE + 2 + j];
				j++;
			}
			contentName[j] = 0x0;
			if (SECTOR(files + i * FILES_LINE_SIZE) != 0xFF) // file
				stringConcat(listFile[count], "[file]  : ", contentName);
			else // file
				stringConcat(listFile[count], "[folder]: ", contentName);
			count++;
		}
		i++;
	} while (i < FILE_MAX_COUNT);
	clear(listFile[count], LF_MAX_COL); // end of list

	return L_SUCCESS;
}
