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
// for debugging
void printSectorString(char *string);


int main() {
  char buffer[512];
  int * sectors;
  int * success;
  (*sectors) = 2;

  // readSector(buffer, 1);  
  // printSectorString(buffer);
  // clear(buffer, 512);
  // printString("\r\nstart\r\n");
  // writeFile("Inside file", "1stFile", sectors);
  // printString("fin\r\n");

  readFile(buffer, "1stFile", success);
  printString("readed\r\n");
  interrupt(0x10, 0xe*256+(*success)+48, 0, 0, 0);
  printString(buffer);


  // printString("Sector: ");
  // interrupt(0x10, 0xe*256+sectors[0]+48, 0, 0, 0);
  // readSector(buffer, 1);
  // printSectorString(buffer);
  // clear(buffer, 512);
  // printString("\r\n");

  // readSector(buffer, 2);
  // printSectorString(buffer);

  // while (1) {
  //   printString("Input: ");
  //   readString(buffer);
  //   printString("Output: ");
  //   printString(buffer);
  //   printString("\r\n");
  // }
  makeInterrupt21();
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

// for debugging
void printSectorString(char *string) {
  int counter = 0;
  while (counter < 512) {
    char ah = *(string + counter);
    char al = 0xe;
    int AX = al * 256 + ah;
    interrupt(0x10, AX, 0, 0, 0);
    counter++;
  }
}

void readString(char *string) {
  int count = 0;
  // ascii list
  int cr = 13;
  int lf = 10;
  int backspace = 8;
  int null = 0;

  while (1) {
    // get char
    int cKarakter = interrupt(0x16, 0, 0, 0, 0);
    if (cKarakter == cr) {
      string[count] = null;
      string[count + 1] = cr;
      string[count + 2] = lf;
      interrupt(0x10, 0xe*256+cr, 0, 0, 0);
      interrupt(0x10, 0xe*256+lf, 0, 0, 0);
      return;
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
  char dir[512], tempBuffer[512];
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
    *success = 0;
    printString("notfound\r\n");
    return;    
  }
  printString("found\r\n");

  // read sector
  sectorIdx = 12;
  while (entry[sectorIdx] != 0x0) {
    readSector(tempBuffer, entry[sectorIdx]);
    // interrupt(0x10, 0xe*256+mod(entry[sectorIdx],10)+48, 0, 0, 0);
    // printString("\r\n");
    // printString(tempBuffer);
    // printString("\r\n");

    for (i = 0; i < 512; ++i) {
      buffer[((sectorIdx - 12) * 512) + i] = tempBuffer[i];
    }
    printString("\r\n");
    printString(buffer);
    printString("\r\n");
    
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
  if (emptyDirLine == -1) // not found
    return;
  printString("emptyDirLine: ");
  interrupt(0x10, 0xe*256+emptyDirLine+48, 0, 0, 0);
  printString("\r\n");

  // get free sector in map
  while ((map[freeSectorMap] != 0x0) && (freeSectorMap < 256))
    freeSectorMap++;
  printString("freeSector: ");
  interrupt(0x10, 0xe*256+div(mod(freeSectorMap,1000),100)+48, 0, 0, 0);
  interrupt(0x10, 0xe*256+div(mod(freeSectorMap,100),10)+48, 0, 0, 0);
  interrupt(0x10, 0xe*256+mod(freeSectorMap,10)+48, 0, 0, 0);
  printString("\r\n");

  if (256 - freeSectorMap < (*sectors)) {
    printString("not enough");
    return;
  } // not enough space
  // if ((*sectors) > 20) // file size too big
  //   return;
  printString(" OK");

  // clean name field in dir
  emptyDirLineAdr = dir + (emptyDirLine * dirLineSize);
  clear(emptyDirLineAdr, dirLineSize);
  // save file name in dir
  i = 0;
  while ((filename[i] != 0x0) && (i < 12)) {
    dir[(emptyDirLine * dirLineSize) + i] = filename[i];
    i++;
  }
  printString(" OK2");

  // write buffer to all sector needed
  for (i = 0; i < (*sectors); ++i) {
    writeSectorAdr = (freeSectorMap + i) * 512;
    clear(writeSectorAdr, 512);
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
    printString(" OK3");  
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

  if (success == 0) return;

  for (i = 0; i < maximum_size; i++) {
    putInMemory(segment, i, buffer[i]);
  }

  launchProgram(segment);
}
