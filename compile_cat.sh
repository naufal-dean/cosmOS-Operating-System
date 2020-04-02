bcc -ansi -c -o cmd/cat/cat.o cmd/cat/cat.c
bcc -ansi -c -o lib/appShellLinker/appShellLinker.o lib/appShellLinker/appShellLinker.c
bcc -ansi -c -o lib/fileIO/fileIO.o lib/fileIO/fileIO.c
bcc -ansi -c -o lib/math/math.o lib/math/math.c
bcc -ansi -c -o lib/string/string.o lib/string/string.c
bcc -ansi -c -o lib/text/text.o lib/text/text.c
nasm -f as86 lib.asm -o lib.o
ld86 -o cmd/cat/cat -d cmd/cat/cat.o lib.o lib/appShellLinker/appShellLinker.o lib/fileIO/fileIO.o lib/math/math.o lib/string/string.o lib/text/text.o