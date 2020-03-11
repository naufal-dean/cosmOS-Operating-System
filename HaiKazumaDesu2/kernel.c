/* Macro define */
#define SECTOR_SIZE 512
// #define FILES_LINE_SIZE 16
#define SECTOR_LINE_SIZE 16
#define FILE_NAME_OFFSET 2
#define FILE_MAX_COUNT 64
#define DIR_LINE_SIZE 32
#define PARENT(P) (P)[0]
#define SECTOR(P) (P)[1]

#define R_SUCCESS 1
#define R_FILE_NOT_FOUND -1
#define W_SUCCESS 1
#define W_FILE_ALREADY_EXIST -1
#define W_ENTRY_FULL -2
#define W_SECTOR_FULL -3
#define W_INVALID_FOLDER -4

int PATH_FOLDER = 1;
int PATH_FILE = 0;
int FILES_LINE_SIZE = 16;


/* Ini deklarasi fungsi */
void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
int div(int a, int b);
int mod(int a, int b);
void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
int validatePath(char * files, char * path, int * sepCount, char parentIndex, char * outFileIdx);
void readFile(char *buffer, char *path, int *result, char parentIndex);
void clear(char *buffer, int length); //Fungsi untuk mengisi buffer dengan 0
void writeFile(char *buffer, char *path, int *result, char parentIndex);
void executeProgram(char *filename, int segment, int *success);
void printLogo();
void interfaceLoop();
void printMenu();
void stringCpy(char * buffOut, char * buffIn);
int stringStartsWith(char * buffFull, char * buffInit);
int filenameCmp(char * buff1, char * buff2);
int stringCmp(char * a, char * b);




int main() {
  char buffer[20 * 512];
  int * sectors;
  int * success;
  (*sectors) = 1;

  makeInterrupt21();
  printLogo();

  // executeProgram("milestone1", 0x2000, success);
  // executeProgram("extern", 0x2000, success);
  
  //loops interface for testing
  interfaceLoop();

  // readFile(buffer, "milestone1", success);
  // printString(buffer);

  // printString("\r\nstart\r\n");
  // writeFile("Testfile123456", "heheFile123456", sectors);
  // printString("fin\r\n");

  // readFile(buffer, "heheFile1234", success);
  // printString(buffer);

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

void readSector(char *buffer, int sector) {
  interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void writeSector(char *buffer, int sector) {
  interrupt(0x13, 0x301, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

int validatePath(char * files, char * path, int * sepCount, char parentIndex, char * outFileIdx, int isFolder) {
  int filesIdx = 0, i;
  if (*sepCount == 0) { // base
    do {
      if (isFolder) {
        if (SECTOR(files + filesIdx * FILES_LINE_SIZE) == 0xFF && PARENT(files + filesIdx * FILES_LINE_SIZE) == parentIndex && filenameCmp(files + filesIdx * FILES_LINE_SIZE, path)) {
          *outFileIdx = filesIdx;
          return 1;
        }
      } else { // file
        if (SECTOR(files + filesIdx * FILES_LINE_SIZE) != 0xFF && PARENT(files + filesIdx * FILES_LINE_SIZE) == parentIndex && filenameCmp(files + filesIdx * FILES_LINE_SIZE, path)) {
          *outFileIdx = filesIdx;
          return 1;
        }
      }
      filesIdx++;
    } while (filesIdx < FILE_MAX_COUNT);
    return 0;
  } else { // reccurens, still folder name
    // get next path component offset
    i = 0;
    while (path[i] != 0x0)
      i++;
    i++;
    // check folder name
    (*sepCount)--;
    do {
      if (SECTOR(files + filesIdx * FILES_LINE_SIZE) == 0xFF && PARENT(files + filesIdx * FILES_LINE_SIZE) == parentIndex && filenameCmp(files + filesIdx * FILES_LINE_SIZE, path))
        return validatePath(files, path + i, sepCount, filesIdx, outFileIdx);
      filesIdx++;
    } while (filesIdx < FILE_MAX_COUNT);
    return 0;
  }
}

void readFile(char *buffer, char *path, int *result, char parentIndex) {
  char files[SECTOR_SIZE * 2], sectors[SECTOR_SIZE], locPath[SECTOR_SIZE];
  int * sepCount; int * fileIdx;
  int i, sectorIdx;
  // load
  readSector(files, 0x101);
  readSector(files + SECTOR_SIZE, 0x102);
  readSector(sectors, 0x103);
  // check if path valid
  // count '/'
  i = 0; (*sepCount) = 0;
  while (path[i] != 0x0) {
    locPath[i] = path[i];
    if (path[i] == '/') {
      locPath[i] = 0x0;
      (*sepCount)++;
    }
    i++;
  }
  locPath[i] = 0x0;
  *fileIdx = 0;
  if (validatePath(files, locPath, sepCount, parentIndex, fileIdx, PATH_FILE) == 0) {
    *result = R_FILE_NOT_FOUND;
    return;
  }
  // read sector
  sectorIdx = SECTOR(files + (*fileIdx) * FILES_LINE_SIZE);
  i = 0;
  while (sectors[sectorIdx * SECTOR_LINE_SIZE + i] != 0x0) {
    readSector(buffer + i * SECTOR_SIZE, sectors[sectorIdx * SECTOR_LINE_SIZE + i]);
    i++;
  }
  *result = R_SUCCESS;
}

void clear(char *buffer, int length) {
  int i;
  for (i = 0; i < length; i++) {
    buffer[i] = 0x0;
  }
} //Fungsi untuk mengisi buffer dengan 0

void writeFile(char *buffer, char *path, int *result, char parentIndex) {
  char map[SECTOR_SIZE], files[2 * SECTOR_SIZE], sectors[SECTOR_SIZE];
  int i, j, filenameOffset;
  int unusedSector, unusedFile, sectorsIdx;
  

  // get map, files, and sectors
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
    return;
  }

  unusedFile = i;


  // Empty Sectors
  i = 0;
  while (i < 32) {
    if (sectors[i * 16] == '\0') {
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
  while (path[filenameOffset + i] != 0x0) {
    files[unusedFile * FILES_LINE_SIZE + 2 + i] = path[filenameOffset + i];
    i++;
  }

  // Write P and S
  SECTOR(files + unusedFile * FILES_LINE_SIZE) = sectorsIdx;
  PARENT(files + unusedFile * FILES_LINE_SIZE) = 0xFF;


  i = 0;
  while (buffer[i * SECTOR_SIZE] != '\0') {
    writeSector(buffer[i * SECTOR_SIZE], unusedSector);
    sectors[unusedFile * 16 + i] = unusedSector;
    map[unusedSector] = 0xFF;
    unusedSector++;
    i++;
  }

  writeSector(map, 0x100);
  writeSector(files, 0x101);
  writeSector(files + SECTOR_SIZE, 0x102);
  writeSector(sectors, 0x103);
  *result = 1;

  //

}

void executeProgram(char *filename, int segment, int *success) {
  int maximum_size = 20 * 512;
  char buffer[20 * 512];
  int i;

  readFile(buffer, filename, success);

  if (*success == 0) {
    printString("Program terminated!\r\n");
    return;
  }  

  for (i = 0; i < maximum_size; i++) {
    putInMemory(segment, i, buffer[i]);
  }

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

void printMenu(){
  printString("Masukkan nomor dari service yang ingin digunakan:\r\n");
  printString("1. Read string, print ke layar\r\n");
  printString("2. Input file\r\n");
  printString("3. Read file\r\n");
  printString("4. Execute program\r\n");
  printString("0. Exit\r\n");
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

void interfaceLoop(){
  char buffer[20 * 512], buffer2[20 * 512], choice[20 * 512];
  int buffLen;
  int * sectors;
  int * success;

  printMenu();
  printString("Menu: ");
  readString(choice);

  //loops while choice is not zero
  while(strToInt(choice) != 0){
    clear(buffer, 20 * 512);
    clear(buffer2, 20 * 512);
    switch(strToInt(choice)){
      case 1: // I/O
          printString("Input  : ");
          readString(buffer);
          printString("Output : ");
          printString(buffer);
          printString("\r\n");
        break;
      case 2: // Input file
          printString("Filename : ");
          readString(buffer);
          printString("Content  :\r\n");
          readString(buffer2);
          // get buff len and sectors needed
          buffLen = 0;
          while (buffer2[buffLen] != 0x0)
            buffLen++;
          *sectors = div(buffLen, 512);
          if (mod(buffLen, 512) != 0) *sectors += 1;
          printString("BEFORE\r\n");
          writeFile(buffer2, buffer, sectors, 0xFF);
          printString("AFTER\r\n");
        break;
      case 3: // Read file
          printString("File to read : ");
          readString(buffer);
          readFile(buffer2, buffer, success, 0xFF);
          if (*success) {
            printString("Content      :\r\n");
            printString(buffer2);
            printString("\r\n");
          } else {
            printString("Read file failed...\r\n");
          }
        break;
      case 4: // Execute program
          printString("Program to be executed : ");
          readString(buffer);
          executeProgram(buffer, 0x2000, success);
        break;
      default:
        printString("Not supported yet!\r\n");
    }
    printString("\r\n");
    printMenu();
    printString("Menu: ");
    readString(choice);
  }
  printString("Thank you for using cosmOS\r\n");
}

int stringCmp(char * buff1, char * buff2) {
  int i = 0;
  while (1) {
    if (buff1[i] != buff2[i])
      return 0;
    if (buff1[i] == 0x0)
      break;
    i++;
  }
  return 1;
}

void stringCpy(char * buffOut, char * buffIn) {
  int i = 0;
  do {
    buffOut[i] = buffIn[i];
    i++;
  } while (buffIn[i] != 0x0);
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

int filenameCmp(char * buff1, char * buff2) {
  int i = 0;
  while (i < 14) {
    if (buff1[i] != buff2[i])
      return 0;
    if (buff1[i] == 0x0)
      break;
    i++;
  }
  return 1;
}