#include "../../lib/math/math.h"
#include "../../lib/string/string.h"
#include "../../lib/appShellLinker/appShellLinker.h"
#include "../../lib/constant.c"

int cdExec(char * path, char * curDir, char * parentIndex);
void shellLoop();
int isCommand(char * cmd);
void pushHistory(char * hist);
void popHistory(char * hist);

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
  char command[512], curDir[2 * 512], tempCurDir[2 * 512], files[SECTOR_SIZE * 2], buffer[SECTOR_SIZE * 16], hold[SECTOR_SIZE], cmd[SECTOR_SIZE], args[SECTOR_SIZE];
  int i, j, parentIndex, tempParIdx, result, binIdx, cnt;
  char temp[100], histSector[SECTOR_SIZE];
  // Read sector
  interrupt(0x21, 0x02, files, 0x101, 0);
  interrupt(0x21, 0x02, files + SECTOR_SIZE, 0x102, 0);
  // Init
  getParIdx(temp);
  parentIndex = strToInt(temp);
  getCurDir(curDir);

  while (1) {
    clear(command, 512);
    intToStr(parentIndex, temp);
    interrupt(0x21, 0x00, "[", 0, 0); interrupt(0x21, 0x00, temp, 0, 0); interrupt(0x21, 0x00, "] ", 0, 0);
    interrupt(0x21, 0x00, curDir, 0, 0); interrupt(0x21, 0x00, "$ ", 0, 0);
    interrupt(0x21, 0x01, command, 0, 0);

    // Get cmd
    i = 0;
    clear(cmd, SECTOR_SIZE);
    j = 0;
    while (command[i] != ' ' && command[i] != 0x0) {
      cmd[j] = command[i];
      i++; j++;
    }
    // Ignore whitespace
    while (command[i] == ' ') i++;
    // Get args
    clear(args, SECTOR_SIZE);
    j = 0;
    while (command[i] != 0x0) {
      args[j] = command[i];
      i++; j++;
    }
    args[j] = 0x0;
    setArgs(args);

    // Save curDir and parentIdx
    setCurDir(curDir);
    intToStr(parentIndex, temp);
    setParIdx(temp);

    // Push command history
    interrupt(0x21, 0x00, "Push\r\n", 0, 0);
    pushHistory(command);
    interrupt(0x21, 0x00, "Pushend\r\n", 0, 0);

    // Execute cmd
    if (stringCmp(cmd, "cd")) {
      /*** CD START ***/
      // copy curDir and parentIndex
      stringCpy(tempCurDir, curDir);
      tempParIdx = parentIndex;
      // check params
      if (args[0] == 0x0) { // no params, cd to root
        result = cdExec("~", tempCurDir, &tempParIdx);
      } else { // params available
        result = cdExec(args, tempCurDir, &tempParIdx);
      }
      // result
      if (result) {
        parentIndex = tempParIdx;
        stringCpy(curDir, tempCurDir);
      } else {
        interrupt(0x21, 0x00, "cd: ", 0, 0);
        interrupt(0x21, 0x00, args, 0, 0);
        interrupt(0x21, 0x00, ": No such file or directory\r\n", 0, 0);
      }
      /*** CD END ***/
    } else if (stringStartsWith(cmd, "./")) {
      /*** EXEC START ***/
      binIdx = findFilename(files, "bin", 0xFF, IS_FOLDER);
      if (findFilename(files, cmd + 2, binIdx, IS_FILE) != -1) {
        interrupt(0x21, (binIdx << 8) + 0x06, cmd + 2, 0x2000, &result);
      } else if (findFilename(files, cmd + 2, parentIndex, IS_FILE) != -1) {  // not in PATH
        interrupt(0x21, (parentIndex << 8) + 0x06, cmd + 2, 0x2000, &result);
      } else {
        interrupt(0x21, 0x00, cmd, 0, 0);
        interrupt(0x21, 0x00, ": No such file or directory\r\n", 0, 0);
      }
      /*** EXEC END ***/
    } else if (stringCmp(cmd, "debug")) {
      interrupt(0x21, 0x02, histSector, HISTORY_SECTOR, 0);
      interrupt(0x21, 0x00, "Meta   :", 0, 0); interrupt(0x21, 0x00, histSector, 0, 0); interrupt(0x21, 0x00, "\r\n", 0, 0);
      interrupt(0x21, 0x00, "H1     :", 0, 0); interrupt(0x21, 0x00, histSector + HIST_CONTENT_OFFSET, 0, 0); interrupt(0x21, 0x00, "\r\n", 0, 0);
      interrupt(0x21, 0x00, "H2     :", 0, 0); interrupt(0x21, 0x00, histSector + HIST_CONTENT_OFFSET + HIST_CONTENT_LINE_SIZE, 0, 0); interrupt(0x21, 0x00, "\r\n", 0, 0);
      interrupt(0x21, 0x00, "H3     :", 0, 0); interrupt(0x21, 0x00, histSector + HIST_CONTENT_OFFSET + HIST_CONTENT_LINE_SIZE * 2, 0, 0); interrupt(0x21, 0x00, "\r\n", 0, 0);
    } else {
      binIdx = findFilename(files, "bin", 0xFF, IS_FOLDER);
      if (findFilename(files, cmd, binIdx, IS_FILE) != -1 && isCommand(cmd)) {
        /*** EXEC COMMAND FROM BIN ***/
        interrupt(0x21, (binIdx << 8) + 0x06, cmd, 0x2000, &result);
      } else {
        /*** NOT FOUND ***/
        interrupt(0x21, 0x00, command, 0, 0);
        interrupt(0x21, 0x00, ": command not found\r\n", 0, 0);
      }
    }
  }
}

int isCommand(char * cmd) {
  // add new command here
  if (stringCmp(cmd, "cat")) {
    return 1;
  } else if (stringCmp(cmd, "cp")){
    return 1;
  } else if (stringCmp(cmd, "ls")) {
    return 1;
  } else if (stringCmp(cmd, "mkdir")) {
    return 1;
  } else if (stringCmp(cmd, "mv")) {
    return 1;
  } else if (stringCmp(cmd, "rm")){
    return 1;
  } else {
    return 0;
  }
}

void pushHistory(char * newHist) {
  int i, j, count;
  char histSector[SECTOR_SIZE];

  // Read history sector
  interrupt(0x21, 0x02, histSector, HISTORY_SECTOR, 0);
  
  // Write history
  count = 0;
  while (histSector[HIST_CONTENT_OFFSET + count * HIST_CONTENT_LINE_SIZE] != 0x0 && count < 3) count++;
  if (count == 3) { // already exist 3 history, replacing history 1 and 2
    for (i = 0; i < 2; i++) {
      j = 0;
      while (histSector[HIST_CONTENT_OFFSET + (i + 1) * HIST_CONTENT_LINE_SIZE + j] != 0x0 && j < HIST_CONTENT_LINE_SIZE - 1) {
        histSector[HIST_CONTENT_OFFSET + i * HIST_CONTENT_LINE_SIZE + j] = histSector[HIST_CONTENT_OFFSET + (i + 1) * HIST_CONTENT_LINE_SIZE + j];
        j++;
        histSector[HIST_CONTENT_OFFSET + i * HIST_CONTENT_LINE_SIZE + j] = 0x0;
      }
    }
    count = 2;
  }
  // Write new history
  j = 0;
  while (newHist[j] != 0x0 && j < HIST_CONTENT_LINE_SIZE - 1) {
    histSector[HIST_CONTENT_OFFSET + count * HIST_CONTENT_LINE_SIZE + j] = newHist[j];
    j++;
  }
  histSector[HIST_CONTENT_OFFSET + count * HIST_CONTENT_LINE_SIZE + j] = 0x0;
  
  // Write history sector
  interrupt(0x21, 0x03, histSector, HISTORY_SECTOR, 0);
}