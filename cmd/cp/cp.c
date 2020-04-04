#include "../../lib/appShellLinker/appShellLinker.h"
#include "../../lib/fileIO/fileIO.h"
#include "../../lib/folderIO/folderIO.h"
#include "../../lib/math/math.h"
#include "../../lib/string/string.h"
#include "../../lib/text/text.h"
#include "../../lib/constant.c"

void validateName(char * files, char * name, int pIdx, int isFolder, int * nxtPIdx);

int main(){
    // var
    char files[SECTOR_SIZE * 2], argv[MAX_ARGC][MAX_ARG_LEN], hold[100], buffer[100], rename[100];
    char curDir[SECTOR_SIZE], absPath[SECTOR_SIZE * 2], newAbsPath[SECTOR_SIZE * 2], content[SECTOR_SIZE];
    int i, j, argc, fIdx, idxHold, parIdx, isFolder, statFolder, *result, *nxtPIdx;

    // read args passed by shell
    getFiles(files);
    getParIdx(buffer);
    getParsedArgs(argv, &argc);
    
    // Get full dir
    getCurDir(curDir);
    i = 0;
    while (curDir[i] != 0x0) i++;
    curDir[i] = '/';
    curDir[i + 1] = 0x0;

    // if the argument is none
    if(argc < 1){
        print("cp: missing file operand\r\n");
        // back to shell
        backToShell();
        return 0;
    } else if(argc > 2){
        print("cp: too many operands\r\n");
        // back to shell
        backToShell();
        return 0;
    }

    // parse args: source
    parIdx = strToInt(buffer); // get current 
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
        parIdx = *nxtPIdx; i++;
    }

    i = 0;
    if(isFolder){ // if the src file is a folder, check if it is empty
        while(i < 64){
            if(PARENT(files + i * FILES_LINE_SIZE) == parIdx){  // the src folder is not empty
                print("cp: the source folder is not empty\r\n");
                backToShell();
                return 0;
            }
            i++;
        }
    } else { // if the src is a file, get its contents
        stringConcat(absPath, curDir + 2, argv[0]);
        absPathParser(newAbsPath, absPath);
        if(readFile(newAbsPath, content) != R_SUCCESS){
            print("cp: failed to read file\r\n");
            backToShell();
            return 0;
        }
        clear(absPath, SECTOR_SIZE * 2); clear(newAbsPath, SECTOR_SIZE * 2);
    }

    statFolder = isFolder;

    // if the argument is only one string
    if(argv[1][0] == 0x0){
        print("cp: missing destination file operand\r\n");
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
                isFolder = statFolder;
            }
        }

        if(argv[1][i] == '/'){
            validateName(files, hold, parIdx, isFolder, nxtPIdx);
            parIdx = *nxtPIdx; i++;
        } else {
            stringCpy(rename, hold);
        }
    }

    idxHold = parIdx; // check if the path is subdir of src
    while(idxHold != 0xFF){
        if(idxHold == fIdx){ // dest is subdirectory of the source
            print("cp: destination is a subdirectory of the source\r\n");
            backToShell();
            return 0;
        }
        idxHold = PARENT(files + idxHold * FILES_LINE_SIZE);
    }

    // check if rename's name exists on the dest directory
    if(findFilename(files, rename, parIdx, 1) != -1 || findFilename(files, rename, parIdx, 0) != -1){
        print("cp: filename already exists in destination\r\n");
        backToShell();
        return 0;
    }

    stringConcat(absPath, curDir + 2, argv[1]);
    absPathParser(newAbsPath, absPath);

    // writefile
    if(isFolder){
        if(createFolder(newAbsPath) != W_SUCCESS){
            print("cp: failed to copy folder\r\n");
            backToShell();
            return 0;
        }
    } else {
       if(writeFile(newAbsPath, content) != W_SUCCESS){
            print("cp: failed to copy file\r\n");
            backToShell();
            return 0;
        }
    }

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
        print("cp: "); print(name);
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
            print("cp: folder named '");
        } else {
            print("cp: file/folder named '");
        }
        print(name); print("' does not exist\r\n");
        // back to shell
        backToShell();
        return;
    }
}