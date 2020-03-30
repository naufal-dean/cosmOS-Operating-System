bcc -ansi -c -o bin/sh/shell.o bin/sh/shell.c
bcc -ansi -c -o lib/math.o lib/math.c
bcc -ansi -c -o lib/string.o lib/string.c
nasm -f as86 lib.asm -o lib.o
ld86 -o bin/sh/shell -d bin/sh/shell.o lib.o lib/math.o lib/string.o
