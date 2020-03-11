# K3-OS-5

# TUGAS BESAR IF2230 - SISTEM OPERASI

## The Awakening - Milestone 01

Kelompok 5 / K03:
1. Muhammad Hasan 		13518012
2. Naufal Dean Anugrah  	13518123
3. M. Fauzan Rafi Sidiq W.	13518147

## Available shell script
1. `compile.sh`: Compile and link kernel.c and kernel.asm, then write to system.img
2. `run.sh`: Call compile.sh, then run bochs + input c in bochs
3. `resetSysImg.sh`: Make system.img anew
4. `externCompile.sh`: Compile and link extern.c and lib.asm

## How to run OS?
1. Just call `run.sh` in terminal

## How to run external program?
1. Compile loadFile.c: `gcc loadFile.c -o loadFile`.
2. Compile extern program by calling `externCompile.sh` in terminal.
3. Load to system.img: `./loadFile extern`.
4. Run OS.
5. Choose menu 4 (Execute program).
6. Input `extern` for the program filename.
7. And you're done.