bcc -ansi -c -o ext/extern.o ext/extern.c
bcc -ansi -c -o lib/math.o lib/math.c
nasm -f as86 lib.asm -o lib.o
ld86 -o ext/extern -d ext/extern.o lib.o lib/math.o