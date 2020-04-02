#include "string.h"

void clear(char *buffer, int length) {
  int i;
  for (i = 0; i < length; i++) {
    buffer[i] = 0x0;
  }
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
  buffOut[i] = 0x0;
}

void stringConcat(char * buffOut, char * buffIn1, char * buffIn2) {
  int i = 0;
  stringCpy(buffOut, buffIn1);
  while (buffOut[i] != 0x0) i++;
  stringCpy(buffOut + i, buffIn2);
}

int stringLen(char * buffer) {
  int i = 0;
  while (buffer[i] != 0x0) i++;
  return i;
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

int findFilename(char * files, char * filename, char parentIndex, int isFolder) {
  int filesIdx;
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
