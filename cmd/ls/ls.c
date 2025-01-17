#include "../../lib/appShellLinker/appShellLinker.h"
#include "../../lib/folderIO/folderIO.h"
#include "../../lib/string/string.h"
#include "../../lib/text/text.h"
#include "../../lib/constant.c"

int main(){
    char curDir[SECTOR_SIZE], absPath[SECTOR_SIZE * 2], newAbsPath[SECTOR_SIZE * 2];
    char argv[MAX_ARGC][MAX_ARG_LEN], listFile[LF_MAX_ROW][LF_MAX_COL];
    int i, argc, count;

    // Check args
    getParsedArgs(argv, &argc);

    // Get full dir
    getCurDir(curDir);
    i = 0;
    while (curDir[i] != 0x0) i++;

    if (argc == 0) {
        if (stringCmp(curDir, "~"))
            listFolderContent("", listFile);
        else
            listFolderContent(curDir + 2, listFile);
        i = 0;
        while (listFile[i][0] != 0x0 && i < LF_MAX_ROW) {
            print(listFile[i]);
            print("\r\n");
            i++;
        }
    } else { // argc > 0
        curDir[i] = '/';
        curDir[i + 1] = 0x0;
        for (count = 0; count < argc; count++) {
            stringConcat(absPath, curDir + 2, argv[count]);
            // Parse full dir
            absPathParser(newAbsPath, absPath);
            // ls
            if (argc > 1) {
                print(argv[count]); 
                print(":\r\n"); 
            }
            if (listFolderContent(newAbsPath, listFile) == L_FOLDER_NOT_FOUND) {
                print("ls: cannot access '");
                print(argv[count]);
                print("': No such file or directory\r\n");
            } else {
                i = 0;
                while (listFile[i][0] != 0x0 && i < LF_MAX_ROW) {
                    print(listFile[i]);
                    print("\r\n");
                    i++;
                }
            }
            if (argc > 1 && count < argc - 1)
                print("\r\n");
        }
    }

    backToShell();
}
