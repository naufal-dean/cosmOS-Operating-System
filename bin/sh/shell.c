#include "../../lib/constant.c"

int cdExec(char * path, char * curDir, char * parentIndex);
void shellLoop();

int main() {
  shellLoop();
}

int cdExec(char * path, char * curDir, char * parentIndex) {
  char files[SECTOR_SIZE * 2];
  int i, j, k, lastSlashIdx, filesIdx, end;
  char temp[100];
  interrupt(0x21, 0x02, files, 0x101, 0);
  interrupt(0x21, 0x02, files + SECTOR_SIZE, 0x102, 0);

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
    filesIdx = findFilename(files, path, *parentIndex, IS_FOLDER);
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
  char command[512], curDir[2 * 512], tempCurDir[2 * 512], files[SECTOR_SIZE * 2], buffer[SECTOR_SIZE * 16], hold[SECTOR_SIZE];
  char * tempParIdx; char * resultPointer;
  int i, j, parentIndex, result, binIdx, cnt;
  char temp[100];
  // read sector
  interrupt(0x21, 0x02, files, 0x101, 0);
  interrupt(0x21, 0x02, files + SECTOR_SIZE, 0x102, 0);
  // init
  stringCpy(curDir, "~");
  parentIndex = 0xFF;
  while (1) {
    // read sector
    interrupt(0x21, 0x02, files, 0x101, 0);
    interrupt(0x21, 0x02, files + SECTOR_SIZE, 0x102, 0);

    clear(command, 512);
    intToStr(parentIndex, temp);
    interrupt(0x21, 0x00, "[", 0, 0); interrupt(0x21, 0x00, temp, 0, 0); interrupt(0x21, 0x00, "] ", 0, 0);
    interrupt(0x21, 0x00, curDir, 0, 0); interrupt(0x21, 0x00, "$ ", 0, 0);
    interrupt(0x21, 0x01, command, 0, 0);
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
        interrupt(0x21, 0x00, "No such file or directory\r\n", 0, 0);
      }
      /*** CD END ***/
    } else if (stringStartsWith(command, "./")) {
      /*** EXEC START ***/
      binIdx = findFilename(files, "bin", 0xFF, IS_FOLDER);
      if (findFilename(files, command + 2, binIdx, IS_FILE) != -1) {
        interrupt(0x21, (binIdx << 8) + 0x06, command + 2, 0x2000, resultPointer);
      } else {  // not in PATH
        interrupt(0x21, (parentIndex << 8) + 0x06, command + 2, 0x2000, resultPointer);
      }
      /*** EXEC END ***/
    } else if (stringStartsWith(command, "cat ")) {
      /*** CAT START ***/
      i = 4;
      while (command[i] == ' ' && command[i] != 0x0)
        i++; // ignore whitespace
      if (command[i] == 0x0) {
        interrupt(0x21, 0x00, "No input file\r\n", 0, 0);
        continue;
      }
      clear(buffer, SECTOR_SIZE * 16);
      interrupt(0x21, (parentIndex << 8) + 0x04, buffer, command + i, resultPointer);
      if (*resultPointer == R_SUCCESS) {
        interrupt(0x21, 0x00, buffer, 0, 0);
        interrupt(0x21, 0x00, "\r\n", 0, 0);
      }
      /*** CAT END ***/
    } else if (stringStartsWith(command, "mv ")) {
      /*** MV START ***/
      clear(hold, SECTOR_SIZE);
      i = 3;

      cnt = 0;
      while (command[i] != 0x0){
        hold[cnt] = command[i];
        i++; cnt++;
      }

      interrupt(0x21, 0x03, hold, 0x400, 0); // write args to sector 0x400
      binIdx = findFilename(files, "bin", 0xFF, IS_FOLDER);

      if (findFilename(files, "mv", binIdx, IS_FILE) != -1) { // executing mv from bin
        interrupt(0x21, (binIdx << 8) + 0x06, "mv", 0x2000, resultPointer);
      }
      /*** MV END ***/
    } else {
      /*** NOT FOUND ***/
      interrupt(0x21, 0x00, command, 0, 0); interrupt(0x21, 0x00, ": command not found\r\n", 0, 0);
    }
  }
}
