#include "../../lib/appShellLinker/appShellLinker.h"
#include "../../lib/fileIO/fileIO.h"
#include "../../lib/math/math.h"
#include "../../lib/string/string.h"
#include "../../lib/text/text.h"
#include "../../lib/constant.c"

int main(){
    char buffer[SECTOR_SIZE * 16], args[SECTOR_SIZE], curDir[SECTOR_SIZE], absPath[SECTOR_SIZE * 2];
    int i, parentIndex, result;

    // Check args
    getArgs(args);
    if (args[0] == 0x0) {
        print("cat: No input file\r\n");
        backToShell();
    }

    // Get absolute dir
    getCurDir(curDir);
    i = 0;
    while (curDir[i] != 0x0) i++;
    curDir[i] = '/';
    curDir[i + 1] = 0x0;

    stringConcat(absPath, curDir + 2, args);

    // Read and print
    clear(buffer, SECTOR_SIZE * 16);
    if (readFile(absPath, buffer) == R_SUCCESS) {
        print(buffer);
        print("\r\n");    
    } else {
        print("cat: ");
        print(args);
        print(": No such file or directory\r\n");
    }

    backToShell();
}
