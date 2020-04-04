#include "lib/constant.c"

/* Function declaration */
/* Main */
void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
void readFile(char *buffer, char *path, int *result, char parentIndex);
void writeFile(char *buffer, char *path, int *result, char parentIndex);
int writeFolder(char * folderName, int *result, char parentIndex);
void executeProgram(char *filename, int segment, int *success, char parentIndex);
/* Etc */
void printLogo();


int main() {
  char buffer[20 * 512];
  int * sectors;
  int * success;
  int idx;
  char files[1024];
  readSector(files, 0x101);
  readSector(files + SECTOR_SIZE, 0x102);
  (*sectors) = 1;

  makeInterrupt21();
  printLogo();

  if (findFilename(files, "Folder1", 0xFF, IS_FOLDER) == -1) {
    printString("Populating files\r\n");
    idx = writeFolder("Folder1", success, 0xFF);
    idx = writeFolder("Folder2", success, idx);
    writeFile("Inside file 1", "File1", success, idx);
    writeFile("Inside file 2", "Folder1/Folder2/File2", success, 0xFF);
    writeFile("Inside file 2 dupl", "Folder1/Folder2/File2", success, 0xFF);
    writeFile("Inside file 3", "Folder2/Folder3/File3", success, 0xFF);
    printString("Populating files done\r\n");
  }

  // Init default curentDirectory and parentIndex for shell
  // CUR_DIR_SECTOR = 0x201
  // PAR_IDX_SECTOR = 0x202
  writeSector("~", 0x201);
  writeSector("255", 0x202);
  // Launch shell
  executeProgram("shell", 0x2000, success, 0x1);

  while (1);
}

void handleInterrupt21 (int AX, int BX, int CX, int DX) {
   char AL, AH;
   AL = (char) (AX);
   AH = (char) (AX >> 8);
   switch (AL) {
      case 0x00:
         printString(BX);
         break;
      case 0x01:
         readString(BX);
         break;
      case 0x02:
         readSector(BX, CX);
         break;
      case 0x03:
         writeSector(BX, CX);
         break;
      case 0x04:
         readFile(BX, CX, DX, AH);
         break;
      case 0x05:
         writeFile(BX, CX, DX, AH);
         break;
      case 0x06:
         executeProgram(BX, CX, DX, AH);
         break;
      default:
         printString("Invalid interrupt");
   }
}

void printString(char *string) {
  int counter = 0;
  while (*(string + counter) != '\0') {
    char ah = *(string + counter);
    char al = 0xe;
    int AX = al * 256 + ah;
    interrupt(0x10, AX, 0, 0, 0);
    counter++;
  }
}

void readString(char *string) {
  char histSector[SECTOR_SIZE];
  int count = 0, histIdx = 0, maxHistIdxPlusOne = 0;
  int cKarakter = 0;
  char AH = 0, AL = 0;
  // ascii list
  int cr = 13;
  int lf = 10;
  int backspace = 8;
  int null = 0;
  // arrow AH key
  int upArr = 0x48, downArr = 0x50;
  // get history
  readSector(histSector, HISTORY_SECTOR);
  while (histSector[HIST_CONTENT_OFFSET + maxHistIdxPlusOne * HIST_CONTENT_LINE_SIZE] != 0x0 && maxHistIdxPlusOne < 3) maxHistIdxPlusOne++;
  histIdx = maxHistIdxPlusOne;
  // read input
  while (1) {
    // get char
    cKarakter = interrupt(0x16, 0, 0, 0, 0);
    AH = (char) (cKarakter >> 8);
    AL = (char) cKarakter;

    // check arrow input
    if (AH == upArr || AH == downArr) {
      if (AH == upArr && histIdx - 1 > -1) {
        // decrement idx
        histIdx--;
        // delete active char
        while (count > 0) {
          string[count] = null;
          count--;
          interrupt(0x10, 0xe*256+backspace, 0, 0, 0);
          interrupt(0x10, 0xe*256+null, 0, 0, 0);
          interrupt(0x10, 0xe*256+backspace, 0, 0, 0);
        }
        // add char from history
        while (histSector[HIST_CONTENT_OFFSET + histIdx * HIST_CONTENT_LINE_SIZE + count] != 0X0 && count < HIST_CONTENT_LINE_SIZE) {
          string[count] = histSector[HIST_CONTENT_OFFSET + histIdx * HIST_CONTENT_LINE_SIZE + count];
          interrupt(0x10, 0xe*256+(string[count]), 0, 0, 0);
          count++;
        }
      } else if (AH == downArr && histIdx + 1 < maxHistIdxPlusOne) {
        // increment idx
        histIdx++;
        // delete active char
        while (count > 0) {
          string[count] = null;
          count--;
          interrupt(0x10, 0xe*256+backspace, 0, 0, 0);
          interrupt(0x10, 0xe*256+null, 0, 0, 0);
          interrupt(0x10, 0xe*256+backspace, 0, 0, 0);
        }
        // add char from history
        while (histSector[HIST_CONTENT_OFFSET + histIdx * HIST_CONTENT_LINE_SIZE + count] != 0X0 && count < HIST_CONTENT_LINE_SIZE) {
          string[count] = histSector[HIST_CONTENT_OFFSET + histIdx * HIST_CONTENT_LINE_SIZE + count];
          interrupt(0x10, 0xe*256+(string[count]), 0, 0, 0);
          count++;
        }
      }
      continue;
    }
    // Normal input
    if (AL == cr) {
      string[count] = null;
      string[count + 1] = cr;
      string[count + 2] = lf;
      interrupt(0x10, 0xe*256+cr, 0, 0, 0);
      interrupt(0x10, 0xe*256+lf, 0, 0, 0);
      return;

    //suport backspacing
    } else if (AL == backspace) {
      if (count > 0) {
        string[count] = null;
        count--;
        interrupt(0x10, 0xe*256+backspace, 0, 0, 0);
        interrupt(0x10, 0xe*256+null, 0, 0, 0);
        interrupt(0x10, 0xe*256+backspace, 0, 0, 0);
      }
    } else {
      string[count] = AL;
      count++;
      interrupt(0x10, 0xe*256+AL, 0, 0, 0);
    }
  }
}

void readSector(char *buffer, int sector) {
  interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void writeSector(char *buffer, int sector) {
  interrupt(0x13, 0x301, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void readFile(char *buffer, char *path, int *result, char parentIndex) {
  char files[SECTOR_SIZE * 2], sectors[SECTOR_SIZE], partPath[SECTOR_SIZE];
  char locParIndex;
  int i, j, filesIdx, sectorsIdx, isFolder;
  char temp[100];
  // load
  readSector(files, 0x101);
  readSector(files + SECTOR_SIZE, 0x102);
  readSector(sectors, 0x103);
  // validate path
  i = 0;
  locParIndex = parentIndex;
  while (path[i] != 0x0) {
    clear(partPath, SECTOR_SIZE);
    // copy part path
    j = 0;
    while(path[i] != 0x0 && path[i] != '/') {
      partPath[j] = path[i];
      i++; j++;
    }

    partPath[j] = 0x0;
    if (path[i] == '/') {
      isFolder = 1; i++;
    } else // path[i] == 0x0
      isFolder = 0;
    // check files
    filesIdx = findFilename(files, partPath, locParIndex, isFolder);
    // check if not found
    if (filesIdx == -1) {
      // printString("File not found\r\n");
      (*result) = R_FILE_NOT_FOUND;
      return;
    }
    // update locParIndex
    locParIndex = filesIdx;
  }
  // read sector
  sectorsIdx = SECTOR(files + filesIdx * FILES_LINE_SIZE);
  i = 0;
  while (sectors[sectorsIdx * SECTOR_LINE_SIZE + i] != 0x0 && i < SECTOR_LINE_SIZE) {
    readSector(buffer + i * SECTOR_SIZE, sectors[sectorsIdx * SECTOR_LINE_SIZE + i]);
    i++;
  }
  *result = R_SUCCESS;
}

void writeFile(char *buffer, char *path, int *result, char parentIndex) {
  char map[SECTOR_SIZE], files[2 * SECTOR_SIZE], sectors[SECTOR_SIZE], temp[100];
  int i, j, filenameOffset, prevOffset, folderIdx; int *success; int *fIdx;
  int unusedSector, unusedFile, sectorsIdx;
  

  // get map, files, and sectors
  readSector(map, 0x100);
  readSector(files, 0x101);
  readSector(files + SECTOR_SIZE, 0x102);
  readSector(sectors, 0x103);

  // validate folder
  clear(temp, 100);
  i = 0; 
  prevOffset = 0; 
  folderIdx = parentIndex;
  while (path[i] != 0x00) {
  	if (path[i] == '/') {
  		for (j = prevOffset; j < i; j++) {
  			temp[j-prevOffset] = path[j];
  		}

  		//check if folder exist. If exists, use the existing folder's idx
      if ((folderIdx = findFilename(files, temp, folderIdx, IS_FOLDER)) == -1) {
        (*result) = W_INVALID_FOLDER;
        // printString("Invalid folder\r\n");
        return; 
      }

  		clear(temp, 100);
  		prevOffset = i+1;
  	}
  	i++;
  }

  // validate filename
  if (findFilename(files, path + prevOffset, folderIdx, IS_FILE) != -1 ||
      findFilename(files, path + prevOffset, folderIdx, IS_FOLDER) != -1) {
    (*result) = W_FILE_ALREADY_EXIST;
    // printString("File already exist\r\n");
    return; 
  }

  //check unused sector from map
  for (i = 0; i < SECTOR_SIZE; i++) {
    if (map[i] == 0x00) {
      break;
    }
  }

  if (i == SECTOR_SIZE) { //NOT FOUND, keluarkan pesan error -3
    (*result) = W_SECTOR_FULL;
    // printString("Sector full\r\n");
    return;
  }

  unusedSector = i;

  //check for empty file
  i = 0;
  while (i < 64) {
    if (files[i * 16] == '\0') {
      break;
    }
    i++;
  }

  if (i == 64) { //NOT FOUND, keluarkan pesan error -2
    (*result) = W_ENTRY_FULL;
    // printString("Entry full\r\n");
    return;
  }

  unusedFile = i;

  // Empty Sectors
  i = 0;
  while (i < 32) {
    if (sectors[i * 16] == 0x0) {
      break;
    }
    i++;
  }

  if (i == 32) { //NOT FOUND, keluarkan pesan error -3
    (*result) = W_SECTOR_FULL;
    // printString("Sector full\r\n");
    return;
  }

  sectorsIdx = i;

  // Write filename
  i = 0;
  while (path[i] != 0x0) {
    if (path[i] == '/') filenameOffset = i + 1;
    i++;
  }

  i = 0;
  while (path[filenameOffset + i] != 0x0 && i < 14) {
    files[unusedFile * FILES_LINE_SIZE + 2 + i] = path[filenameOffset + i];
    i++;
  }

  // Write P and S
  SECTOR(files + unusedFile * FILES_LINE_SIZE) = sectorsIdx;
  PARENT(files + unusedFile * FILES_LINE_SIZE) = folderIdx;

  // Write sectors
  i = 0;
  while (buffer[i * SECTOR_SIZE] != '\0') {
    writeSector(buffer + i * SECTOR_SIZE , unusedSector);
    sectors[sectorsIdx * 16 + i] = unusedSector;
    map[unusedSector] = 0xFF;
    // Get next unusedSector
    do {
      unusedSector++;
    } while (map[unusedSector] == 0xFF);
    i++;
  }

  writeSector(map, 0x100);
  writeSector(files, 0x101);
  writeSector(files + SECTOR_SIZE, 0x102);
  writeSector(sectors, 0x103);
  *result = W_SUCCESS;
}

int writeFolder(char * folderName, int *result, char parentIndex) { // return filesIdx used
  char files[2 * SECTOR_SIZE], temp[100];
  int i, j, filenameOffset;
  int unusedFile;

  // get map, files, and sectors
  readSector(files, 0x101);
  readSector(files + SECTOR_SIZE, 0x102);

  //check for empty file
  i = 0;
  do {
    if (files[i * 16] == 0x00) {
      break;
    }
    i++;
  } while (i < FILE_MAX_COUNT);

  if (i == FILE_MAX_COUNT) { //NOT FOUND, keluarkan pesan error -2
    (*result) = W_ENTRY_FULL;
    // printString("ret etrfull\r\n");
    return -1;
  }
  unusedFile = i;

  // Write folderName
  i = 0;
  while (folderName[i] != 0x0 && i < 14) {
    files[unusedFile * FILES_LINE_SIZE + 2 + i] = folderName[i];
    i++;
  }

  // Write P and S
  SECTOR(files + unusedFile * FILES_LINE_SIZE) = 0xFF;
  PARENT(files + unusedFile * FILES_LINE_SIZE) = parentIndex;

  // Write sector
  writeSector(files, 0x101);
  writeSector(files + SECTOR_SIZE, 0x102);

  *result = W_SUCCESS;
  return unusedFile;
}

void executeProgram(char *filename, int segment, int *success, char parentIndex) {
  int maximum_size = 16 * SECTOR_SIZE;
  char buffer[16 * SECTOR_SIZE], histSector[SECTOR_SIZE];
  int i;

  readFile(buffer, filename, success, parentIndex);
  if (*success < 0) {
    printString("Program terminated!\r\n");
    return;
  }  

  for (i = 0; i < maximum_size; i++) {
    putInMemory(segment, i, buffer[i]);
  }

  // Write program name as history metadata
  readSector(histSector, HISTORY_SECTOR);
  stringCpy(histSector + HIST_METADATA_OFFSET, filename);
  writeSector(histSector, HISTORY_SECTOR);

  // Launch program
  launchProgram(segment);
}

void printLogo(){
  int i, j;
  for(i = 0; i <= 50; i++){
    for(j = 0; j<= 80; j++){
      putInMemory(0xB000, 0x8000 + (80*j+i)*2, ' ');
      putInMemory(0xB000, 0x8001 + (80*j+i)*2, 0xD);
    }
  }
  printString("\r\n\r\n");
  printString("                                                         _.oo.\r\n");
  printString("                                 _.u[[/;:,.         .odMMMMMM'\r\n");
  printString("                              .o888UU[[[/;:-.  .o@P^    MMM^\r\n");
  printString("                             oN88888UU[[[/;::-.        dP^\r\n");
  printString("                            dNMMNN888UU[[[/;:--.   .o@P^\r\n");
  printString("                           ,MMMMMMN888UU[[/;::-. o@^\r\n");
  printString("                           NNMMMNN888UU[[[/~.o@P^\r\n");
  printString("                           888888888UU[[[/o@^-..\r\n");
  printString("                          oI8888UU[[[/o@P^:--..\r\n");
  printString("                       .@^  YUU[[[/o@^;::---..\r\n");
  printString("                     oMP     ^/o@P^;:::---..\r\n");
  printString("                  .dMMM    .o@^ ^;::---...\r\n");
  printString("                 dMMMMMMM@^`       `^^^^\r\n");
  printString("                YMMMUP^\r\n");
  printString("                 ^^\r\n");
  printString("                                                 ____  _____\r\n");
  printString("                      _________  _________ ___  / __ |/ ___/\r\n");
  printString("                     / ___/ __ |/ ___/ __ `__ |/ / / /|__ | \r\n");
  printString("                    / /__/ /_/ (__  ) / / / / / /_/ /___/ / \r\n");
  printString("                    |___/|____/____/_/ /_/ /_/|____//____/  \r\n");
  printString("\r\n\r\n");
  interrupt(0x15, 0x8600, 0x8480, 0x1e);
  interrupt(0x10, 0x2, 0, 0, 0);  
}