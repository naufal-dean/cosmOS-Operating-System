/* Macro define */
/* Byte size */
#define SECTOR_SIZE 512
#define FILES_LINE_SIZE 16
#define FILE_NAME_OFFSET 2
#define FILE_MAX_COUNT 64
#define SECTOR_LINE_SIZE 16
#define DIR_LINE_SIZE 32
/* Parent and Sector byte selector */
#define PARENT(P) (P)[0]
#define SECTOR(P) (P)[1]
/* RW return code */
#define R_SUCCESS 1
#define R_FILE_NOT_FOUND -1
#define W_SUCCESS 1
#define W_FILE_ALREADY_EXIST -1
#define W_ENTRY_FULL -2
#define W_SECTOR_FULL -3
#define W_INVALID_FOLDER -4
/* Folder and file flag */
#define IS_FOLDER 1
#define IS_FILE 0

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
/* Shell */
int cdExec(char * path, char * curDir, char * parentIndex);
void shellLoop();
/* Strings */
int stringCmpUtils(char * buff1, char * buff2, int buffLen);
int stringCmp(char * buff1, char * buff2);
int filenameCmp(char * buff1, char * buff2);
void stringCpy(char * buffOut, char * buffIn);
int stringStartsWith(char * buffFull, char * buffInit);
/* Utils */
void clear(char *buffer, int length);
int div(int a, int b);
int mod(int a, int b);
void intToStr(int number, char * buffer);
int strToInt(char * string);
int findFilename(char * filename, char parentIndex, int isFolder);
char* filenameFromIdx(int idx);
int isFolderExist(char* name, int * fileIdx);
/* Etc */
void printLogo();

/* Main program */
int main() {
  char buffer[20 * 512];
  int * sectors;
  int * success;
  int idx;
  (*sectors) = 1;

  makeInterrupt21();
  printLogo();

  if (findFilename("Folder1", 0xFF, IS_FOLDER) == -1) {
    printString("Populating files\r\n");
    idx = writeFolder("Folder1", success, 0xFF);
    idx = writeFolder("Folder2", success, idx);
    writeFile("Inside file 1", "File1", success, idx);
    printString("Populating files done\r\n");
  }

  // executeProgram("milestone1", 0x2000, success);
  // executeProgram("extern", 0x2000, success);
  
  //loops interface for testing
  // interfaceLoop();
  shellLoop();

  // readFile(buffer, "milestone1", success);
  // printString(buffer);

  // printString("\r\nstart\r\n");
  // writeFile("Testfile123456", "heheFile123456", sectors);
  // printString("fin\r\n");

  // readFile(buffer, "heheFile1234", success);
  // printString(buffer);

  while (1);
}

/*
 * Main
 */
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
  int count = 0;
  int cKarakter = 0;
  // ascii list
  int cr = 13;
  int lf = 10;
  int backspace = 8;
  int null = 0;
  while (1) {
    // get char
    cKarakter = interrupt(0x16, 0, 0, 0, 0);
    if (cKarakter == cr) {
      string[count] = null;
      string[count + 1] = cr;
      string[count + 2] = lf;
      interrupt(0x10, 0xe*256+cr, 0, 0, 0);
      interrupt(0x10, 0xe*256+lf, 0, 0, 0);
      return;

    //suport backspacing
    } else if (cKarakter == backspace) {
      if (count > 0) {
        string[count] = null;
        count--;
        interrupt(0x10, 0xe*256+backspace, 0, 0, 0);
        interrupt(0x10, 0xe*256+null, 0, 0, 0);
        interrupt(0x10, 0xe*256+backspace, 0, 0, 0);
      }
    } else {
      string[count] = cKarakter;
      count++;
      interrupt(0x10, 0xe*256+cKarakter, 0, 0, 0);
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
  char files[SECTOR_SIZE * 2], sectors[SECTOR_SIZE], partPath[SECTOR_SIZE * 2];
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
    clear(partPath, SECTOR_SIZE * 2);
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
    filesIdx = findFilename(partPath, locParIndex, isFolder);
    // check if not found
    if (filesIdx == -1) {
      printString("File not found\r\n");
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

  clear(temp, 100);
  i = 0; 
  prevOffset = 0; 
  folderIdx = parentIndex;
  while(path[i] != 0x00){
    if(path[i] == '/'){
      for(j = prevOffset; j < i; j++){
        temp[j-prevOffset] = path[j];
      }

      //check if folder exist. If exists, use the existing folder's idx
      if(isFolderExist(temp, fIdx)){
        folderIdx = *fIdx;
      } else { //write folder if doesn't exist
        folderIdx = writeFolder(temp, success, folderIdx);
      }
      clear(temp, 100);
      prevOffset = i+1;
    }
    i++;
  }

  //read all sector after being written for the folders
  readSector(map, 0x100);
  readSector(files, 0x101);
  readSector(files + SECTOR_SIZE, 0x102);
  readSector(sectors, 0x103);

  //check unused sector from map
  for (i = 0; i < SECTOR_SIZE; i++) {
    if (map[i] == 0x00) {
      break;
    }
  }

  if (i == SECTOR_SIZE) { //NOT FOUND, keluarkan pesan error -3
    (*result) = W_SECTOR_FULL;
    printString("ret secfull\r\n");
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
    printString("ret etrfull\r\n");
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
    unusedSector++;
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
    printString("ret etrfull\r\n");
    return -1;
  }
  unusedFile = i;

  // Write folderName
  i = 0;
  while (folderName[i] != 0x0 && i < 14) {/* By*/
    files[unusedFile * FILES_LINE_SIZE + 2 + i] = folderName[i];
    i++;
  }

  // Write P and S
  // SECTO* Parent and Sector byte selector */files + unusedFile * FILES_LINE_SIZE) = 0xFF;
  PARENT(files + unusedFile * FILES_LINE_SIZE) = parentIndex;

  // Write sector
  writeSector(files, 0x101);
  writeSector(files + SECTOR_SIZE, 0x102);

  *result = W_SUCCESS;
  return unusedFile;
}

void executeProgram(char *filename, int segment, int *success, char parentIndex) {
  int maximum_size = 16 * 512;
  char buffer[16 * 512];
  int i;

  readFile(buffer, filename, success, parentIndex);
  if (*success < 0) {
    printString("Program terminated!\r\n");
    return;
  }  

  for (i = 0; i < maximum_size; i++) {
    putInMemory(segment, i, buffer[i]);
  }

  launchProgram(segment);
}

/*
 * Shell
 */
int cdExec(char * path, char * curDir, char * parentIndex) {
  char files[SECTOR_SIZE * 2];
  int i, j, k, lastSlashIdx, filesIdx, end;
  char temp[100];
  readSector(files, 0x101);
  readSector(files + SECTOR_SIZE, 0x102);

  // isolate first part of path
  i = 0;
  while(path[i] != 0x0 && path[i] != '/')
    i++;
  end = path[i] == 0x0;
  path[i] = 0x0;
  // execute cd partPath
  if (stringCmp(path, "..")) {
    if (*parentIndex != 0xFF) {
      // update parentIndex
      *parentIndex = PARENT(files + (*parentIndex) * FILES_LINE_SIZE);
      // update curDir
      j = 0;
      while (curDir[j] != 0x0) {
        if (curDir[j] == '/') lastSlashIdx = j;
        j++;
      }
      curDir[lastSlashIdx] = 0x0;
    }
  } else if (stringCmp(path, "~")) {
    // update parentIndex
    *parentIndex = 0xFF;
    // update curDir
    stringCpy(curDir, "~");
  } else {
    filesIdx = findFilename(path, *parentIndex, IS_FOLDER);
    if (path[0] != 0x0) { 
      if (filesIdx != -1) { // found
        // update parentIndex
        *parentIndex = filesIdx;
        // update curDir
        j = 0;
        while (curDir[j] != 0x0) j++;
        curDir[j] = '/'; j++;
        // copy new path
        k = 0;
        while (path[k] != 0x0) {
          curDir[j] = path[k];
          j++; k++;
        }
        curDir[j] = 0x0;
      } else {
        return 0;
      }
    }
  }

  // reccursive
  if (end) {
    return 1;
  } else {
    cdExec(path + i + 1, curDir, parentIndex);
  }
}

void shellLoop() {
  char command[512], curDir[2 * 512], tempCurDir[2 * 512], files[SECTOR_SIZE * 2], buffer[SECTOR_SIZE * 16];
  char * tempParIdx; char * resultPointer;
  int i, j, parentIndex, result, binIdx;
  char temp[100];
  // read sector
  readSector(files, 0x101);
  readSector(files + SECTOR_SIZE, 0x102);
  // init
  stringCpy(curDir, "~");
  parentIndex = 0xFF;
  while (1) {
    clear(command, 512);
    intToStr(parentIndex, temp);
    printString("["); printString(temp); printString("] ");
    printString(curDir); printString("$ ");
    readString(command);
    // execute command
    if (stringStartsWith(command, "cd")) {
      /*** CD START ***/
      i = 2;
      while (command[i] == ' ' && command[i] != 0x0)
        i++; // ignore whitespace
      // copy curDir
      stringCpy(tempCurDir, curDir);
      // check params
      *tempParIdx = parentIndex;
      if (command[i] == 0x0) { // no params, cd to root
        result = cdExec("~", tempCurDir, tempParIdx);
      } else { // params available
        result = cdExec(command + i, tempCurDir, tempParIdx);
      }
      // result
      if (result) {
        parentIndex = *tempParIdx;
        stringCpy(curDir, tempCurDir);
      } else {
        printString("No such file or directory\r\n");
      }
      /*** CD END ***/
    } else if (stringStartsWith(command, "./")) {
      /*** EXEC START ***/
      binIdx = findFilename("bin", 0xFF, IS_FOLDER);
      if (findFilename(command + 2, binIdx, IS_FILE) != -1) {
        executeProgram(command + 2, 0x2000, resultPointer, binIdx);
      } else {  // not in PATH
        executeProgram(command + 2, 0x2000, resultPointer, parentIndex);
      }
      /*** EXEC END ***/
    } else if (stringStartsWith(command, "cat")) {
      /*** CAT START ***/
      i = 3;
      while (command[i] == ' ' && command[i] != 0x0)
        i++; // ignore whitespace
      if (command[i] == 0x0) {
        printString("No input file\r\n");
        continue;
      }
      clear(buffer, SECTOR_SIZE * 16);
      readFile(buffer, command + i, resultPointer, parentIndex);
      if (*resultPointer == R_SUCCESS) {
        printString(buffer);
        printString("\r\n");
      }
      /*** CAT END ***/
    } else {
      /*** NOT FOUND ***/
      printString(command); printString(": command not found\r\n");
    }
  }
}

/*
 * Strings
 */
int stringCmpUtils(char * buff1, char * buff2, int buffLen) {
  int i = 0;
  while (i < buffLen) {
    if (buff1[i] != buff2[i])
      return 0;
    if (buff1[i] == 0x0)
      break;
    i++;
  }
  return 1;
}

int stringCmp(char * buff1, char * buff2) {
  return stringCmpUtils(buff1, buff2, 1e6); // assume buffer length not exceeds 1e6
}

int filenameCmp(char * buff1, char * buff2) {
  return stringCmpUtils(buff1, buff2, 14);
}

void stringCpy(char * buffOut, char * buffIn) {
  int i = 0;
  do {
    buffOut[i] = buffIn[i];
    i++;
  } while (buffIn[i] != 0x0);
  buffOut[i] = 0x0;
}

int stringStartsWith(char * buffFull, char * buffInit) {
  int i = 0;
  while (1) {
    if (buffInit[i] == 0x0)
      break;
    if (buffFull[i] != buffInit[i])
      return 0;
    i++;
  }
  return 1;
}

/*
 * Utils
 */
void clear(char *buffer, int length) {
  int i;
  for (i = 0; i < length; i++) {
    buffer[i] = 0x0;
  }
} //Fungsi untuk mengisi buffer dengan 0

int div(int a, int b) {
  int l = 0, r = a, ret = 0;

  while (l <= r) {
    int mid = (l + r) >> 1;
    if (a >= mid * b) {
      l = mid + 1;
      ret = mid;
    } else {
      r = mid - 1;
    }
  }
  return ret;
}

int mod(int a, int b) {
  return a - b * div(a, b);
}

void intToStr(int number, char * buffer) {
  int tempNum, i, j, neg;

  if (number == 0) {
    buffer[0] = '0';
    buffer[1] = 0x0;
  } else {
        neg = number < 0;
        if (neg) number *= -1;
        tempNum = number;
        
    i = 0;
    while (tempNum != 0) {
            buffer[i] = mod(tempNum, 10) + 48;
            tempNum = div(tempNum, 10);
            i++;
    }
        if (neg) buffer[i++] = '-';
        
        // swap
        for (j = 0; j < div(i, 2); ++j) {
            buffer[j] ^= buffer[i-j-1];
            buffer[i-j-1] ^= buffer[j];
            buffer[j] ^= buffer[i-j-1];
        }
        buffer[i] = 0x0;
  }
}

int strToInt(char * string) {
  int val = 0, i = 0, neg;

  neg = string[0] == '-';
  if (neg) i++;
  while (string[i] != 0x0) {
    val = val * 10 + (string[i] - 48);
    i++;
  }
  if (neg) val *= -1;
  return val;
}

int findFilename(char * filename, char parentIndex, int isFolder) {
  char files[SECTOR_SIZE * 2];
  int filesIdx;

  readSector(files, 0x101);
  readSector(files + SECTOR_SIZE, 0x102);
  // search
  filesIdx = 0;
  do {
    if (PARENT(files + filesIdx * FILES_LINE_SIZE) == parentIndex) {
      if (isFolder && SECTOR(files + filesIdx * FILES_LINE_SIZE) == 0xFF) {
        if (filenameCmp(filename, files + filesIdx * FILES_LINE_SIZE + 2))
          break;
      } else if (!isFolder && SECTOR(files + filesIdx * FILES_LINE_SIZE) != 0xFF) {
        if (filenameCmp(filename, files + filesIdx * FILES_LINE_SIZE + 2))
          break;
      }
    }
    filesIdx++;
  } while (filesIdx < FILE_MAX_COUNT);
  // return
  if (filesIdx == FILE_MAX_COUNT) { // not found
    return -1;
  } else {
    return filesIdx;
  }
}

char* filenameFromIdx(int idx) {
  int i;
  char buffer[14]; char files[1024];

  readSector(files, 0x101);
  readSector(files + SECTOR_SIZE, 0x102);

  i = 2;
  while(files + (idx * FILES_LINE_SIZE + i) != "\0" && i < 16){
    buffer[i-2] = files[idx * FILES_LINE_SIZE + i];
    i++;
  }

  return buffer;
}

int isFolderExist(char* name, int * fileIdx) {
  int i; char files[1024];

  readSector(files, 0x101);
  readSector(files + SECTOR_SIZE, 0x102);

  for(i = 0; i < 32; i++){
    if(SECTOR(files + i * FILES_LINE_SIZE) == 0xFF && stringCmp(name, filenameFromIdx(i)) == 1){
      fileIdx = i;
      return 1;
    }
  }
  return 0;
}

void printLogo() {
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