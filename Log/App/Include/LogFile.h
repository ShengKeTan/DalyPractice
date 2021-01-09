#ifndef __LOG_FILE_H__
#define __LOG_FILE_H__

#include "Log.h"

typedef struct handle_file_t
{
    uint8_t*        wkey;
    int             cflag;
    char*           filepath;
    FILE*           flog;
    char*           iobuf;
    size_t          iocap;
    size_t          filesize;
    size_t          curfilesize;
    size_t          backupnum;
    size_t          curbackupnum;
    pthread_mutex_t mutex;
} handle_file_t;

#define S_LOG_FILE_SIZE sizeof(handle_file_t)

#define AES_128    0x10

#define AES_EX(n)  (((n) & (AES_128 - 1)) != 0 ? ((n) | (AES_128 - 1)) + 1 : (n))

#define EX_SINGLE_LOG_SIZE    AES_EX(SINGLE_LOG_SIZE)

void* _file_handle_create(const char* fileName, size_t fileSize, size_t fileBak, size_t ioBufSize, uint8_t cflag, const char* password);

/**
 * write message to file.
 * @param fh - log file name.
 * @param msg - message to write.
 * @param len - len of message.
*/
void write_file(handle_file_t *fh, char *msg, size_t len);

/**
 * flush IO buffer to file. 
 * @param lh - log file andle.
*/
void file_handle_flush(handle_file_t *lh);

/**
 * destory file handle.
 * @param lh - log handle.
*/
void file_handle_destory(handle_file_t *lh);

#endif // __LOG_FILE_H__