#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>

#include "merror.h"
#include "LogStream.h"

#define RESET   "\x1B[0m"

void* _stream_handle_create(uint8_t streams)
{
    handle_stream_t* sh = (handle_stream_t *)calloc(1, S_LOG_STREAM_SIZE);
    assert(sh != NULL);

    int level = _INFO1_LEVEL + 1;
    while (--level >= _DEBUG_LEVEL)
    {
        (sh->streams)[level] = streams & (level + 1) ? stderr : stdout;
    }
    return (void *)sh;
}

void write_stream(handle_stream_t* sh, log_level_t level, char* msg)
{
    fprintf((sh->streams)[level], "%s%s%s", (sh->style)[level], msg, RESET);
}

void stream_handle_flush()
{
    fflush(stdout);
}

void stream_handle_destory(handle_stream_t* sh)
{
    stream_handle_flush();
    free(sh);
}