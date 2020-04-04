#include "../../lib/appShellLinker/appShellLinker.h"
#include "../../lib/math/math.h"
#include "../../lib/string/string.h"
#include "../../lib/text/text.h"
#include "../../lib/constant.c"

void validateName(char * files, char * name, int pIdx, int isFolder, int * nxtPIdx);

int main(){
    // var
    char files[SECTOR_SIZE * 2], argv[MAX_ARGC][MAX_ARG_LEN], name[14], hold[100], rename[14], buffer[100];
    int i, j, argc, idxHold, fIdx, parIdx, bParIdx, isFolder, *result, *nxtPIdx;

    // read args passed by shell
    getFiles(files);
    getParIdx(buffer); // get current directory's index
    getParsedArgs(argv, &argc);

    // if the argument is none
    if(argc < 0){
        print("mv: missing file operand\r\n");
        // back to shell
        backToShell();
        return 0;
    } else if(argc > 2){
        print("mv: too many operands\r\n");
        // back to shell
        backToShell();
        return 0;
    }


    // parse args: source
    parIdx = strToInt(buffer); // get current folder idx
    isFolder = 0; i = 0;
    while(argv[0][i] != ' ' && argv[0][i] != 0x0){
        j = 0; clear(hold, 100);
        while(argv[0][i] != '/' && argv[0][i] != ' ' & argv[0][i] != 0x0){
            hold[j] = argv[0][i];
            i++; j++;
        }

        if(argv[0][i] == '/'){
            isFolder = 1;
        } else {
            if(findFilename(files, hold, parIdx, 1) != -1){
                isFolder = 1;
            } else {
                isFolder = 0;
            }
        }

        validateName(files, hold, parIdx, isFolder, nxtPIdx);
        if(argv[0][i] == '/'){
            parIdx = *nxtPIdx; i++;
        }
    }

    bParIdx = parIdx;
    stringCpy(name, hold); clear(hold, 100);

    // if the argument is only one string
    if(argv[1][0] == 0x0){
        print("mv: missing destination file operand\r\n");
        // back to shell
        backToShell();
        return 0;
    }
    
    // parse args: destination
    parIdx = strToInt(buffer);
    isFolder = 0; i = 0;
    while(argv[1][i] != ' ' && argv[1][i] != 0x0){
        j = 0; clear(hold, 100);
        while(argv[1][i] != '/' && argv[1][i] != ' ' && argv[1][i] != 0x0){
            hold[j] = argv[1][i];
            i++; j++;
        }

        if(argv[1][i] == '/'){
            isFolder = 1; 
        } else {
            if(findFilename(files, hold, parIdx, 1) != -1){
                isFolder = 1;
            } else {
                isFolder = 0;
            }
        }

        if(argv[1][i] == '/'){
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

    idxHold = parIdx; // check if the path is subdir of src
    while(idxHold != 0xFF){
        if(idxHold == fIdx){ // dest is subdirectory of the source
            print("mv: destination is a subdirectory of the source\r\n");
            backToShell();
            return 0;
        }
        idxHold = PARENT(files + idxHold * FILES_LINE_SIZE);
    }

    // check if rename's name exists on the dest directory
    if(findFilename(files, rename, parIdx, 1) != -1 || findFilename(files, rename, parIdx, 0) != -1){
        print("mv: filename already exists in destination\r\n");
        backToShell();
        return 0;
    }

    // update parent of the source to dest    
    PARENT(files + fIdx * FILES_LINE_SIZE) = parIdx;

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
    setFiles(files);

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
        print("mv: "); print(name);
        print(" does not exist. (Must be 14 characters or lower)\r\n");
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
            print("mv: folder named '");
        } else {
            print("mv: file/folder named '");
        }
        print(name); print("' does not exist\r\n");
        // back to shell
        backToShell();
        return;
    }
}