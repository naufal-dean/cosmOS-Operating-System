dd if=/dev/zero of=system.img bs=512 count=2880
nasm bootloader.asm -o bootloader
dd if=bootloader of=system.img bs=512 count=1 conv=notrunc
dd if=img/map.img of=system.img bs=512 count=1 seek=256 conv=notrunc
dd if=img/files.img of=system.img bs=512 count=2 seek=257 conv=notrunc
dd if=img/sectors.img of=system.img bs=512 count=1 seek=259 conv=notrunc
./compile_shell.sh
./loadFile bin/sh/shell 0x1
./compile_extern.sh
./loadFile ext/extern 0x1