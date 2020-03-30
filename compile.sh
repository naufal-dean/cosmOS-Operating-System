bcc -ansi -c -o kernel.o kernel.c
bcc -ansi -c -o utils/math_utils.o utils/math_utils.c
bcc -ansi -c -o utils/string_utils.o utils/string_utils.c
nasm -f as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel.o kernel_asm.o utils/math_utils.o utils/string_utils.o
dd if=kernel of=system.img bs=512 conv=notrunc seek=3