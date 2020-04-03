#include "../../lib/appShellLinker/appShellLinker.h"
#include "../../lib/folderIO/folderIO.h"
#include "../../lib/string/string.h"
#include "../../lib/text/text.h"
#include "../../lib/constant.c"

int main(){
    char curDir[SECTOR_SIZE], absPath[SECTOR_SIZE * 2], newAbsPath[SECTOR_SIZE * 2];
    char argv[MAX_ARGC][MAX_ARG_LEN];
    int i, argc, count, result;

    // Check args
    getParsedArgs(argv, &argc);
    if (argc < 1) {
        print("mkdir: missing operand\r\n");
        backToShell();
    }

    // Get full dir
    getCurDir(curDir);
    i = 0;
    while (curDir[i] != 0x0) i++;
    curDir[i] = '/';
    curDir[i + 1] = 0x0;

    // mkdir
    for (count = 0; count < argc; count++) {
        stringConcat(absPath, curDir + 2, argv[count]);

        // Parse full dir
        absPathParser(newAbsPath, absPath);

        // Create folder
        result = createFolder(newAbsPath);
        if (result != W_SUCCESS) {
            print("mkdir: cannot create directory '");            
            print(argv[count]);
            if (result == W_FILE_ALREADY_EXIST)
                print("': File exists\r\n");
            else if (result == W_INVALID_FOLDER)
                print("': No such file or directory\r\n");
            else if (result == W_ENTRY_FULL)
                print("': File entry full\r\n");
        }
    }

    backToShell();
}
