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
./compile_cat.sh
./loadFile cmd/cat/cat 0x1
./compile_cp.sh
./loadFile cmd/cp/cp 0x1
./compile_ls.sh
./loadFile cmd/ls/ls 0x1
./compile_mkdir.sh
./loadFile cmd/mkdir/mkdir 0x1
./compile_mv.sh
./loadFile cmd/mv/mv 0x1
./compile_rm.sh
./loadFile cmd/rm/rm 0x1