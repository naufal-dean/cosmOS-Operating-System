bcc -ansi -c -o cmd/cp/cp.o cmd/cp/cp.c
bcc -ansi -c -o lib/appShellLinker/appShellLinker.o lib/appShellLinker/appShellLinker.c
bcc -ansi -c -o lib/fileIO/fileIO.o lib/fileIO/fileIO.c
bcc -ansi -c -o lib/folderIO/folderIO.o lib/folderIO/folderIO.c
bcc -ansi -c -o lib/math/math.o lib/math/math.c
bcc -ansi -c -o lib/string/string.o lib/string/string.c
bcc -ansi -c -o lib/text/text.o lib/text/text.c
nasm -f as86 lib.asm -o lib.o
ld86 -o cmd/cp/cp -d cmd/cp/cp.o lib.o lib/string/string.o lib/appShellLinker/appShellLinker.o lib/math/math.o lib/text/text.o lib/fileIO/fileIO.o lib/folderIO/folderIO.o