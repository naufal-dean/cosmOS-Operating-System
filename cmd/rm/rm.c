#include "../../lib/appShellLinker/appShellLinker.h"
#include "../../lib/fileIO/fileIO.h"
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
    if (argc < 1) {
        print("rm: missing operand\r\n");
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

        // Get file list, delete file/folder
        if (listFolderContent(newAbsPath, listFile) == L_SUCCESS) { // rm folder
            if (listFile[0][0] == 0x0) { // empty folder
                if (deleteFolder(newAbsPath) != D_FOLDER_SUCCESS) {
                    print("rm: cannot remove '");
                    print(argv[count]);
                    print("': No such file or directory\r\n");
                }
            } else { // non empty folder
                print("rm: cannot remove '");
                print(argv[count]);
                print("': Folder is not empty\r\n");
            }
        } else { // rm file
            if (deleteFile(newAbsPath) != D_FILE_SUCCESS) {
                print("rm: cannot remove '");
                print(argv[count]);
                print("': No such file or directory\r\n");
            }
        }
    }

    backToShell();
}
