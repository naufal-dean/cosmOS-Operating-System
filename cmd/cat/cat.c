#include "../../lib/appShellLinker/appShellLinker.h"
#include "../../lib/fileIO/fileIO.h"
#include "../../lib/string/string.h"
#include "../../lib/text/text.h"
#include "../../lib/constant.c"

int main(){
    char buffer[SECTOR_SIZE * 16], curDir[SECTOR_SIZE], absPath[SECTOR_SIZE * 2], newAbsPath[SECTOR_SIZE * 2];
    char argv[MAX_ARGC][MAX_ARG_LEN];
    int i, argc, count;

    // Check args
    getParsedArgs(argv, &argc);
    if (argc < 1) {
        print("cat: No input file\r\n");
        backToShell();
    }

    // Get full dir
    getCurDir(curDir);
    i = 0;
    while (curDir[i] != 0x0) i++;
    curDir[i] = '/';
    curDir[i + 1] = 0x0;

    for (count = 0; count < argc; count++) {
        stringConcat(absPath, curDir + 2, argv[count]);

        // Parse full dir
        absPathParser(newAbsPath, absPath);

        // Read and print
        clear(buffer, SECTOR_SIZE * 16);
        if (readFile(newAbsPath, buffer) == R_SUCCESS) {
            print(buffer);
            print("\r\n");    
        } else {
            print("cat: ");
            print(argv[count]);
            print(": No such file or directory\r\n");
        }
    }

    backToShell();
}
