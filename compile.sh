bcc -ansi -c -o kernel.o kernel.c
bcc -ansi -c -o lib/math/math.o lib/math/math.c
bcc -ansi -c -o lib/string/string.o lib/string/string.c
nasm -f as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel.o kernel_asm.o lib/math/math.o lib/string/string.o
dd if=kernel of=system.img bs=512 conv=notrunc seek=3