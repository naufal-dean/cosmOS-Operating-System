#ifndef CONSTANT
#define CONSTANT

/* Byte size */
#define SECTOR_SIZE 512
#define FILES_LINE_SIZE 16
#define FILE_NAME_OFFSET 2
#define FILE_MAX_COUNT 64
#define SECTOR_LINE_SIZE 16
#define SECTOR_MAX_COUNT 32
#define DIR_LINE_SIZE 32
/* Parent and Sector byte selector */
#define PARENT(P) (P)[0]
#define SECTOR(P) (P)[1]
/* RW return code */
#define R_SUCCESS 1
#define R_FILE_NOT_FOUND -1
#define W_SUCCESS 1
#define W_FILE_ALREADY_EXIST -1
#define W_ENTRY_FULL -2
#define W_SECTOR_FULL -3
#define W_INVALID_FOLDER -4
/* Folder and file flag */
#define IS_FOLDER 1
#define IS_FILE 0
/* History handler */
#define HISTORY_SECTOR 0x203
#define HIST_METADATA_OFFSET 0
#define HIST_CONTENT_OFFSET 100
#define HIST_CONTENT_LINE_SIZE 125
/* Special sector */
#define FILES_SECTOR_1 0x101
#define FILES_SECTOR_2 0x102
#define ARGS_SECTOR 0x200
#define CUR_DIR_SECTOR 0x201
#define PAR_IDX_SECTOR 0x202

#endif // CONSTANT
