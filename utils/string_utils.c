#include "constant_utils.c"

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

char* filenameFromIdx(char * files, int idx){
  int i;
  char buffer[14];

  i = 2;
  while(files + (idx * FILES_LINE_SIZE + i) != "\0" && i < 16){
    buffer[i-2] = files[idx * FILES_LINE_SIZE + i];
    i++;
  }

  return buffer;
}

int isFolderExist(char * files, char* name, int * fileIdx){
  int i;

  for(i = 0; i < 32; i++){
    if(SECTOR(files + i * FILES_LINE_SIZE) == 0xFF && stringCmp(name, filenameFromIdx(i)) == 1){
      fileIdx = i;
      return 1;
    }
  }
  return 0;
}