bcc -ansi -c -o bin/sh/shell.o bin/sh/shell.c
bcc -ansi -c -o lib/math/math.o lib/math/math.c
bcc -ansi -c -o lib/string/string.o lib/string/string.c
nasm -f as86 lib.asm -o lib.o
ld86 -o bin/sh/shell -d bin/sh/shell.o lib.o lib/math/math.o lib/string/string.o
