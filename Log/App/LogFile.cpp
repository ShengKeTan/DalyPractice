#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <cassert>
#include <fcntl.h>
#include <ctime>

#include "LogFile.h"
#include "merror.h"

#define PATH_MAX    4096

static void _update_file(handle_file_t* fh);

typedef struct
{
    uint8_t*            wkey;
    int                 cflag;
    char*               filepath;
    FILE*               flog;
    char*               iobuf;
    size_t              iocap;
    size_t              filesize;
    size_t              curfilesize;
    size_t              backupnum;
    size_t              curbacknum;
    pthread_mutex_t     mutex;
} file_handle_t;

#ifndef S_ISREG
#define S_ISREG(x) (((x) & S_IFMT) == S_IFREG)
#endif

static uint64_t _get_file_fize(const char* filepath)
{
    struct stat sbuf;
    int ret = stat(filepath, &sbuf);
    if (ret || !S_ISREG(sbuf.st_mode))
    {
        return 0;
    }
    return (uint64_t)sbuf.st_size;
}

static void _log_lock(handle_file_t* l)
{
    pthread_mutex_lock(&l->mutex);
}

static void _log_unlock(handle_file_t* l)
{
    pthread_mutex_unlock(&l->mutex);
}

static void _lock_destroy(handle_file_t* l)
{
    pthread_mutex_destroy(&l->mutex);
}

static int _pwd_init(uint8_t* *wkey, const char* pwd)
{
    //TODO: MD5(pwd, key)
    return 0;
}

static void _backupfile(handle_file_t* fh)
{
    char newfile[PATH_MAX] = {0};
    if (fh->cflag & COMPRESS)
    {
        //TODO : COMPRESS
    }
    else
    {
        sprintf(newfile, "%s.bak%lu", fh->filepath, fh->curbackupnum % fh->backupnum);
        rename(fh->filepath, newfile);
    }
}

static int _iobuf_init(handle_file_t *fh, size_t capacity, int flag, const char* pwd)
{
    fh->iocap = capacity;
    fh->iobuf = (char *)calloc(1, fh->iocap);
    if (!fh->iobuf)
    {
        return 1;
    }

    if (flag & ENCRYPT)
    {
        assert(pwd != NULL);
        if (0 != _pwd_init(&fh->wkey, pwd))
        {
            free(fh->iobuf);
            return 1;
        }
    }
    return 0;
}

static int _file_init(handle_file_t* fh, const char* logpath, size_t filesize, size_t backnum)
{
    fh->filesize = filesize;
    fh->filepath = strdup(logpath);
    if (!fh->filepath)
    {
        return 1;
    }

    fh->curfilesize = _get_file_fize(logpath);
    fh->curbackupnum = 0;
    fh->backupnum = backnum;
    return 0;
}

static int _file_handle_request(handle_file_t* fh)
{
    if (fh->flog)
    {
        return 0;
    }

    fh->flog = fopen(fh->filepath, "a+");
    if (!fh->flog)
    {
        return 1;
    }

    if (setvbuf(fh->flog, fh->iobuf, _IOFBF, fh->iocap) != 0)
    {
        return 1;
    }
    return 0;
}

void* _file_handle_create(const char* fileName, size_t fileSize, size_t fileBak, size_t ioBufSize, uint8_t cflag, const char* password)
{
    if (!fileName)
    {
        return NULL;
    }

    handle_file_t* l = (handle_file_t *)calloc(1, S_LOG_FILE_SIZE);
    if (!l)
    {
        exit_throw("failf to calloc!");
    }

    int ret = _file_init(l, fileName, fileSize, fileBak);
    if (ret != 0)
    {
        free(l);
        return NULL;
    }

    ret = _iobuf_init(l, ioBufSize, cflag, password);
    if (ret != 0)
    {
        free(l->filepath);
        free(l);
        return NULL;
    }

    ret = pthread_mutex_init(&l->mutex, NULL);
    assert(ret == 0);

    l->cflag = cflag;
    return (void *)l;
}

void write_file(handle_file_t *fh, char *msg, size_t len)
{
    if (fh->cflag & ENCRYPT)
    {
        //TODO:
    }

    _log_lock(fh);

    if (0 != _file_handle_request(fh))
    {
        info_display("failed to open the file\n");
        perror("Failed to open file\n");
        return;
    }

    //循环写，直到msg完全被写入文件
    size_t wlen = 0;
    while (1)
    {
        int curlen = fwrite(msg + wlen, sizeof(char), len - wlen, fh->flog);
        if (curlen == 0)
        {
            break;
        }
        else if (curlen < 0)
        {
            error_display("Faild to write the file, path[%s], len[%lu]\n", fh->filepath, wlen);
            return;
        }

        wlen += curlen;
    }
    fh->curfilesize += wlen;

    if (fh->curfilesize > fh->filesize)
    {
        _update_file(fh);
    }

    _log_unlock(fh);

    if (fh->cflag & ENCRYPT)
    {
        free(msg);
    }
}

void file_handle_flush(handle_file_t *fh)
{
    if (fh == NULL)
    {
        return;
    }

    _log_lock(fh);

    fclose(fh->flog);
    fh->flog = NULL;

    _log_unlock(fh);
}

void file_handle_destory(handle_file_t *fh)
{
    if (fh == NULL)
    {
        return;
    }

    _log_lock(fh);

    fclose(fh->flog);
    fh->flog = NULL;
    _backupfile(fh);

    free(fh->iobuf);
    free(fh->wkey);


    _log_unlock(fh);
    _lock_destroy(fh);

    free(fh);
    fh = NULL;
}

static void _update_file(handle_file_t* fh)
{
    if (!fh->flog)
    {
        return;
    }

    fclose(fh->flog);
    fh->flog = NULL;

    if (fh->backupnum == 0)
    {
        remove(fh->filepath);
    }
    else
    {
        _backupfile(fh);
    }
    fh->curbackupnum++;
    fh->curfilesize = 0;
}