bcc -ansi -c -o cmd/mv/mv.o cmd/mv/mv.c
bcc -ansi -c -o lib/appShellLinker/appShellLinker.o lib/appShellLinker/appShellLinker.c
bcc -ansi -c -o lib/math/math.o lib/math/math.c
bcc -ansi -c -o lib/string/string.o lib/string/string.c
bcc -ansi -c -o lib/text/text.o lib/text/text.c
nasm -f as86 lib.asm -o lib.o
ld86 -o cmd/mv/mv -d cmd/mv/mv.o lib.o lib/string/string.o lib/appShellLinker/appShellLinker.o lib/math/math.o lib/text/text.o