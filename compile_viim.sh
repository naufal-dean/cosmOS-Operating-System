bcc -ansi -c -o viim/viim.o viim/viim.c
bcc -ansi -c -o lib/appShellLinker/appShellLinker.o lib/appShellLinker/appShellLinker.c
bcc -ansi -c -o lib/fileIO/fileIO.o lib/fileIO/fileIO.c
bcc -ansi -c -o lib/math/math.o lib/math/math.c
bcc -ansi -c -o lib/string/string.o lib/string/string.c
bcc -ansi -c -o lib/text/text.o lib/text/text.c
nasm -f as86 lib.asm -o lib.o
ld86 -o viim/viim -d viim/viim.o lib.o lib/appShellLinker/appShellLinker.o lib/fileIO/fileIO.o lib/string/string.o lib/text/text.o lib/math/math.o
./loadFile viim/viim 0x1