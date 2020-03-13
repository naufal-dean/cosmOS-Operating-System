// loadFile.c
// Michael Black, 2007
//
// Loads a file into the file system
// This should be compiled with gcc and run outside of the OS

#include <stdio.h>
#include <stdlib.h>

void main(int argc, char* argv[]) {
  int i;

  if (argc < 2) {
    printf("Specify file name to load\n");
    return;
  }

  // open the source file
  FILE* loadFil;
  loadFil = fopen(argv[1], "r");
  if (loadFil == 0) {
    printf("File not found\n");
    return;
  }

  // open the floppy image
  FILE* floppy;
  floppy = fopen("system.img", "r+");
  if (floppy == 0) {
    printf("system.img not found\n");
    return;
  }

  // load the disk map
  char map[512];
  fseek(floppy, 512 * 0x100, SEEK_SET);
  for (i = 0; i < 512; i++) map[i] = fgetc(floppy);

  // load the directory
  char files[512 * 2];
  fseek(floppy, 512 * 0x101, SEEK_SET);
  for (i = 0; i < 512 * 2; i++) files[i] = fgetc(floppy);

  // load the sector
  char sector[512];
  fseek(floppy, 512 * 0x103, SEEK_SET);
  for (i = 0; i < 512; i++) sector[i] = fgetc(floppy);

  // find a free entry in the files
  for (i = 0; i < 512; i = i + 0x10)
    if (files[i] == 0) break;
  if (i == 512) {
    printf("Not enough room in files\n");
    return;
  }
  int dirindex = i;

  // fill the name field with 00s first
  for (i = 0; i < 16; i++) files[dirindex + i] = 0x00;
  // fill the P tag
  if (argc == 2) {
    files[dirindex] = 0xFF;
    printf("Trying to load with parent = 0xff\n");
  } else {
    files[dirindex] = (char)strtol(argv[2], NULL, 16);
    printf("Trying to load with parent = 0x%x\n", (char)strtol(argv[2], NULL, 16) & 0xFF);
  }
  // copy the name over
  for (i = 0; i < 14; i++) {
    if (argv[1][i] == 0) break;
    files[dirindex + 2 + i] = argv[1][i];
  }

  // find a free entry in the sector
  for (i = 0; i < 512; i = i + 0x10)
    if (sector[i] == 0) break;
  if (i == 512) {
    printf("Not enough room in sector\n");
    return;
  }
  int secindex = i;
  // fill the S tag
  files[dirindex + 1] = secindex >> 4;

  // find free sectors and add them to the file
  int sectcount = 0;
  while (!feof(loadFil)) {
    if (sectcount == 16) {
      printf("Not enough space in directory entry for file\n");
      return;
    }

    // find a free map entry
    for (i = 0; i < 256; i++)
      if (map[i] == 0) break;
    if (i == 256) {
      printf("Not enough room for file\n");
      return;
    }

    // mark the map entry as taken
    map[i] = 0xFF;

    // mark the sector in the directory entry
    sector[secindex] = i;
    secindex++;
    sectcount++;

    printf("Loaded %s to sector %d\n", argv[1], i);

    // move to the sector and write to it
    fseek(floppy, i * 512, SEEK_SET);
    for (i = 0; i < 512; i++) {
      if (feof(loadFil)) {
        fputc(0x0, floppy);
        break;
      } else {
        char c = fgetc(loadFil);
        fputc(c, floppy);
      }
    }
  }

  // write the map, files, and sector back to the floppy image
  fseek(floppy, 512 * 0x100, SEEK_SET);
  for (i = 0; i < 512; i++) fputc(map[i], floppy);

  fseek(floppy, 512 * 0x101, SEEK_SET);
  for (i = 0; i < 512 * 2; i++) fputc(files[i], floppy);

  fseek(floppy, 512 * 0x103, SEEK_SET);
  for (i = 0; i < 512; i++) fputc(sector[i], floppy);

  fclose(floppy);
  fclose(loadFil);
}
