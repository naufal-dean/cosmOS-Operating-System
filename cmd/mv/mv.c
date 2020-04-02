#include "../../lib/string/string.h"
#include "../../lib/math/math.h"
#include "../../lib/appShellLinker/appShellLinker.h"
#include "../../lib/constant.c"

void validateName(char * files, char * name, int pIdx, int isFolder, int * nxtPIdx);

int main(){
    // var
    char files[SECTOR_SIZE * 2], args[SECTOR_SIZE], name[14], hold[100], rename[14], test[10], buf[3];
    int i, j, srcLength, fIdx, parIdx, bParIdx, isFolder, *result, *nxtPIdx;

    // read args passed by shell
    readSector_intr(files, 0x101);
	readSector_intr(files + SECTOR_SIZE, 0x102);
    getArgs(args);

    i = 0;
    // ignore all whitespaces
    while(args[i] == ' '){
        i++;
    }

    // if the argument is none
    if(args[i] == 0x0){
        interrupt(0x21, 0, "mv: missing file operand\r\n", 0, 0);
        // back to shell
        backToShell();
        return 0;
    }

    // parse args: source
    parIdx = 0xFF; // get current 
    isFolder = 0;
    while(args[i] != ' ' && args[i] != 0x0){
        j = 0; clear(hold, 100);
        while(args[i] != '/' && args[i] != ' ' & args[i] != 0x0){
            hold[j] = args[i];
            i++; j++;
        }

        if(args[i] == '/'){
            isFolder = 1;
        } else {
            if(findFilename(files, hold, parIdx, 1) != -1){
                isFolder = 1;
            } else {
                isFolder = 0;
            }
        }

        validateName(files, hold, parIdx, isFolder, nxtPIdx);
        if(args[i] == '/'){
            parIdx = *nxtPIdx; i++;
        }
    }

    bParIdx = parIdx;
    stringCpy(name, hold); clear(hold, 100);

    // ignore all whitespaces
    while(args[i] == ' '){
        i++;
    }

    // if the argument is only one string
    if(args[i] == 0x0){
        interrupt(0x21, 0, "mv: missing destination file operand\r\n", 0, 0);
        // back to shell
        backToShell();
        return 0;
    }
    
    // parse args: destination
    parIdx = 0xFF;
    isFolder = 0;
    while(args[i] != ' ' && args[i] != 0x0){
        j = 0; clear(hold, 100);
        while(args[i] != '/' && args[i] != ' ' && args[i] != 0x0){
            hold[j] = args[i];
            i++; j++;
        }

        if(args[i] == '/'){
            isFolder = 1; 
        } else {
            if(findFilename(files, hold, parIdx, 1) != -1){
                isFolder = 1;
            } else {
                isFolder = 0;
            }
        }

        if(args[i] == '/'){
            validateName(files, hold, parIdx, isFolder, nxtPIdx);
            parIdx = *nxtPIdx; i++;
        } else {
            stringCpy(rename, hold);
        }
    }

    // finding idx of the file target
    if(findFilename(files, name, bParIdx, 0) != -1){
        fIdx = findFilename(files, name, bParIdx, 0);
    } else {
        fIdx = findFilename(files, name, bParIdx, 1);
    }

    // update parent of the source to dest    
    PARENT(files + fIdx * FILES_LINE_SIZE) = parIdx;
 
    // check if rename's name exists on the dest directory
    if(findFilename(files, rename, parIdx, 1) != -1 || findFilename(files, rename, parIdx, 0) != -1){

    }

    // update name
    i = 2;
    while(i <= 13){ // clear name
        (files + fIdx * FILES_LINE_SIZE)[i] = 0x0;
        i++;
    }

    i = 2;
    while(rename[i-2] != 0x0){ // set name
        (files + fIdx * FILES_LINE_SIZE)[i] = rename[i-2];
        i++;
    }

    // write to image
    writeSector_intr(files, 0x101);
	writeSector_intr(files + SECTOR_SIZE, 0x102);

    // back to shell
    backToShell();
    return 0;
}

void validateName(char * files, char * name, int pIdx, int isFolder, int * nxtPIdx){
    int cnt, *result;

    cnt = 0;
    while(name[cnt] != 0x0){
        cnt++;
    }

    if(cnt > 14){ // file/folder name error: exceeding 14 chars
        interrupt(0x21, 0, "mv: ", 0, 0);
        interrupt(0x21, 0, name, 0, 0);
        interrupt(0x21, 0, " does not exist. (Must be 14 characters or lower)\r\n", 0, 0);
        return;
    }

    if(stringCmp(name, "..")){ // handles relative path: goes up to the path tree
        if(pIdx == 0xFF){ // if already in root, stays in root
            *nxtPIdx = 0xFF;
        } else {
            *nxtPIdx = PARENT(files + pIdx * FILES_LINE_SIZE);
        }
        return;
    }

    if(findFilename(files, name, pIdx, isFolder) != -1){ // handles regular path: find the file/folder name in the files
        if(isFolder){ // if folder: set parent index for next iteration
            *nxtPIdx = findFilename(files, name, pIdx, isFolder);
        }
    } else { // file/folder doesn't exist
        if(isFolder){
            interrupt(0x21, 0, "mv: folder named '", 0, 0);
        } else {
            interrupt(0x21, 0, "mv: file/folder named '", 0, 0);
        }
        interrupt(0x21, 0, name, 0, 0);
        interrupt(0x21, 0, "' does not exist\r\n");
        // back to shell
        backToShell();
        return;
    }
}