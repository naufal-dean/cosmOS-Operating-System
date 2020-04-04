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

## How to reset `system.img` then compile and load all utility?
1. Just call `reset_sys_img.sh` in terminal

## How to run OS?
1. Just call `run.sh` in terminal (make sure that `system.img` is ready, you can use `reset_sys_img.sh` to prepare `system.img`)

## How to run external program?
1. Compile loadFile.c: `gcc loadFile.c -o loadFile`.
2. Compile extern program by calling `compile_extern.sh` in terminal.
3. Load to system.img: `./loadFile ext/extern`.
4. Run OS.
5. Input `./extern` to launch program.
6. And you're done.

## Commands
These are the commands supported for the CosmOS's shell

### cat
Print file's contents on standard output
```
$ cat [FILE 1] [FILE 2] [FILE 3] ... 
``` 

### cd
Change the shell working directory to target directory (cd to root if no `[TARGET DIRECTORY]` specified)
```
$ cd
``` 
```
$ cd [TARGET DIRECTORY]
``` 

### cp
Copy files and directories to target path
```
$ cp [FILE] [TARGET PATH]
``` 

### ls
Lists information about the `FILE`s (in the current directory if no `[FILE]` specified).
```
$ ls
```
```
$ ls [FILE 1] [FILE 2] [FILE 3] ...
```

### mkdir
Create folder in the specified path
```
$ mkdir [TARGET PATH 1] [TARGET PATH 2] [TARGET PATH 3] ...
```

### mv
Rename SOURCE to DEST, or move SOURCE(s) to DIRECTORY
```
$ mv [SOURCE] [DEST]
```

### rm
Removes each specified file/directory. It does not remove non-empty directories
```
$ rm [FILE 1] [FILE 2] [FILE 3] ...
```