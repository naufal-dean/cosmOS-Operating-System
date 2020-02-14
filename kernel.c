/* Ini deklarasi fungsi */
void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
int div(int a, int b);
int mod(int a, int b);
void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
void readFile(char *buffer, char *filename, int *success);
void clear(char *buffer, int length); //Fungsi untuk mengisi buffer dengan 0
void writeFile(char *buffer, char *filename, int *sectors);
void executeProgram(char *filename, int segment, int *success);
void printLogo();
void interfaceLoop();
void printMenu();


int main() {
  char buffer[2000];
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
  // writeFile("Test file", "heheFile", sectors);
  // printString("fin\r\n");

  // readFile(buffer, "key.txt", success);
  // printString(buffer);

  while (1);
}

void handleInterrupt21 (int AX, int BX, int CX, int DX){
  switch (AX) {
    case 0x0:
      printString(BX);
      break;
    case 0x1:
      readString(BX);
      break;
    case 0x2:
      readSector(BX, CX);
      break;
    case 0x3:
      writeSector(BX, CX);
      break;
    case 0x4:
      readFile(BX, CX, DX);
      break;
    case 0x5:
      writeFile(BX, CX, DX);
      break;
    case 0x6:
      executeProgram(BX, CX, DX);
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

void readFile(char *buffer, char *filename, int *success) {
  char dir[512];
  int i, found, sectorIdx;
  char * entry;
  // get dir sector
  readSector(dir, 2);

  // get entry
  entry = dir;

  // search filename
  while ((*entry) != 0x0) {
    found = 1;
    i = 0;
    while (entry[i] != 0x0 && i < 12) {
      if (entry[i] != filename[i]) {
        found = 0;
        break;
      }
      i++;
    }
    if (found) break;
    entry = entry + 32;
  }
  if (!found) { // not found
    printString("File not found!\r\n");
    *success = 0;
    return;    
  }

  // read sector
  sectorIdx = 12;
  i = 0;
  while (entry[sectorIdx] != 0x0 && sectorIdx < 32) {
    readSector(buffer + (i * 512), entry[sectorIdx]);
    i++;
    sectorIdx++;
  }
  *success = 1;
}

void clear(char *buffer, int length) {
  int i;
  for (i = 0; i < length; i++) {
    buffer[i] = 0x0;
  }
} //Fungsi untuk mengisi buffer dengan 0

void writeFile(char *buffer, char *filename, int *sectors) {
  char map[512], dir[512];
  char tempBuffer[512];
  int i, j;
  int dirLineSize = 32, dirLineCount = 16, emptyDirLine = -1, emptyDirLineAdr;
  int freeSectorMap = 0;
  int writeSectorAdr;
  
  // get map and dir sector
  readSector(map, 1);
  readSector(dir, 2);

  // search for empty dir
  i = 0;
  while (i < dirLineCount) {
    if (dir[i * dirLineSize] == 0x0) {
      emptyDirLine = i;
      break;
    }
    i++;
  }
  if (emptyDirLine == -1) { // not found
    printString("No empty dir!\r\n");
    return;
  }

  // get free sector in map
  while ((map[freeSectorMap] != 0x0) && (freeSectorMap < 256))
    freeSectorMap++;

  if (256 - freeSectorMap < (*sectors)) { // not enough space
    printString("Not enough space!\r\n");
    return;
  } 
  if ((*sectors) > 20) { // file size too big
    printString("File size too big!\r\n");
    return;
  }

  // clean name field in dir
  emptyDirLineAdr = dir + (emptyDirLine * dirLineSize);
  clear(emptyDirLineAdr, dirLineSize);
  // save file name in dir
  i = 0;
  while ((filename[i] != 0x0) && (i < 12)) {
    dir[(emptyDirLine * dirLineSize) + i] = filename[i];
    i++;
  }

  // write buffer to all sector needed
  for (i = 0; i < (*sectors); ++i) {
    writeSectorAdr = (freeSectorMap + i) * 512;
    clear(writeSectorAdr, 512);
    clear(tempBuffer, 512);
    // write per byte to sector
    j = 0;
    while ((j < 512) && (buffer[j] != 0x0)) {
      tempBuffer[j] = buffer[(i * 512) + j];
      j++;
      writeSector(tempBuffer, (freeSectorMap + i));
    }
    // write sector position to map, dir, and sectors
    map[freeSectorMap + i] = 0xff;
    dir[(emptyDirLine * dirLineSize) + 12 + i] = freeSectorMap + i;
  }

  // finalize changes
  writeSector(map, 1);
  writeSector(dir, 2);
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
  interrupt(0x10, 0x7, 0, 0);
}

void printMenu(){
  printString("Masukkan nomor dari service yang ingin digunakan:\r\n");
  printString("1. Read string, print ke layar\r\n");
  printString("2. Input file\r\n");
  printString("3. Read file\r\n");
  printString("4. Execute program\r\n\r\n");
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
  char buffer[999], choice[999];
  printMenu();
  readString(choice);

  //loops while choice is not zero
  while(strToInt(choice) != 0){
    switch(strToInt(choice)){
      case 1:
        readString(buffer);
        printString(buffer);
        readString(choice);
      default:
        printString("Not supported yet!\r\n");
    }
  }
  printString("Thank you for using cosmOS\r\n");
}
