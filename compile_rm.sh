bcc -ansi -c -o cmd/rm/rm.o cmd/rm/rm.c
bcc -ansi -c -o lib/appShellLinker/appShellLinker.o lib/appShellLinker/appShellLinker.c
bcc -ansi -c -o lib/fileIO/fileIO.o lib/fileIO/fileIO.c
bcc -ansi -c -o lib/folderIO/folderIO.o lib/folderIO/folderIO.c
bcc -ansi -c -o lib/string/string.o lib/string/string.c
bcc -ansi -c -o lib/text/text.o lib/text/text.c
nasm -f as86 lib.asm -o lib.o
ld86 -o cmd/rm/rm -d cmd/rm/rm.o lib.o lib/appShellLinker/appShellLinker.o lib/fileIO/fileIO.o lib/folderIO/folderIO.o lib/string/string.o lib/text/text.o
