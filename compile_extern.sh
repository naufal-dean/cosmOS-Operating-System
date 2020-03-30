bcc -ansi -c -o ext/extern.o ext/extern.c
bcc -ansi -c -o utils/math_utils.o utils/math_utils.c
nasm -f as86 lib.asm -o lib.o
ld86 -o ext/extern -d ext/extern.o lib.o utils/math_utils.o