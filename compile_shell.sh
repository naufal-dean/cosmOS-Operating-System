bcc -ansi -c -o bin/sh/shell.o bin/sh/shell.c
bcc -ansi -c -o utils/math_utils.o utils/math_utils.c
bcc -ansi -c -o utils/string_utils.o utils/string_utils.c
nasm -f as86 lib.asm -o lib.o
ld86 -o bin/sh/shell -d bin/sh/shell.o lib.o utils/math_utils.o utils/string_utils.o
