# K3-OS-5 (TUGAS BESAR IF2230 - SISTEM OPERASI)

## The Extent Of Bonds - Milestone 03

Kelompok 5 / K03:
1. Muhammad Hasan 		13518012
2. Naufal Dean Anugrah  	13518123
3. M. Fauzan Rafi Sidiq W.	13518147

<!-- ## Available shell script
1. `compile.sh`: Compile and link kernel.c and kernel.asm, then write to system.img
2. `run.sh`: Call compile.sh, then run bochs + input c in bochs
3. `resetSysImg.sh`: Make system.img anew
4. `externCompile.sh`: Compile and link extern.c and lib.asm -->

## How to run OS?
1. Just call `run.sh` in terminal

## How to run external program?
1. Compile loadFile.c: `gcc loadFile.c -o loadFile`.
2. Compile extern program by calling `compile_extern.sh` in terminal.
3. Load to system.img: `./loadFile extern`.
4. Run OS.
5. Choose menu 4 (Execute program).
6. Input `extern` for the program filename.
7. And you're done.

## Commands
These are the commands supported for the CosmOS's shell

### cat
Print file's contents on standard output <br>
usage: <br>
```cat [FILE]``` 

### cd
Change the shell working directory to target directory <br>
usage: <br>
```cd [TARGET DIRECTORY]``` 

### cp
Copy files and directories to target path <br>
usage: <br>
```cp [FILE] [TARGET PATH]``` 

### ls
Lists information about the `FILE`s (in the current directory by default). <br>
usage: <br>
```ls [FILE]```

### mkdir
Create folder in the specified path <br>
usage: <br>
```mkdir [TARGET PATH]```

### mv
Rename SOURCE to DEST, or move SOURCE(s) to DIRECTORY <br>
usage: <br>
```mv [SOURCE] [DEST]```

### rm
Removes each specified file/directory. It does not remove non-empty directories <br>
usage: <br>
```rm [FILE]```