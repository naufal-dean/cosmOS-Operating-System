bcc -ansi -c -o cmd/mkdir/mkdir.o cmd/mkdir/mkdir.c
bcc -ansi -c -o lib/appShellLinker/appShellLinker.o lib/appShellLinker/appShellLinker.c
bcc -ansi -c -o lib/folderIO/folderIO.o lib/folderIO/folderIO.c
bcc -ansi -c -o lib/string/string.o lib/string/string.c
bcc -ansi -c -o lib/text/text.o lib/text/text.c
nasm -f as86 lib.asm -o lib.o
ld86 -o cmd/mkdir/mkdir -d cmd/mkdir/mkdir.o lib.o lib/appShellLinker/appShellLinker.o lib/folderIO/folderIO.o lib/string/string.o lib/text/text.o
