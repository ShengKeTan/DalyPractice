#ifndef __LOG_H__
#define __LOG_H__

#include "list.h"

//typedef list<log_handle_t *>::iterator log_t;

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include <cstdio>
#include <cstdint>

/* log level */
#define LOG_DEBUG 	0    //print all level msg
#define LOG_ERR		1    //print all except log_debug
#define LOG_WARN	2    //print warn, info, info1
#define LOG_INFO	3    //info, info1 can be print
#define LOG_INFO1	4    //only print loginfo1
#define LOG_CLOSE	9    //nothing to print

#define LOG_DEBUG_MESSAGE	1    //print debug message(time, lineNo, filePath, threadId)
	                             //1: on    0:off

#define LOG_FILE_LEVEL		LOG_DEBUG      //file log level
#define LOG_STREAM_LEVEL	LOG_DEBUG      //stream log level

#define SINGLE_LOG_SIZE		(10 * 1024)    //max log file size

#define UNUSED		((void)0)

#define F_MODE		0x01    //file handle
#define S_MODE		0x10    //stream handle
#define F_S_MODE	0x11    //file and stream
#define GET_MODE(fmode, smode, dmode)	(fmode < dmode ? (smode < dmode ? F_S_MODE : F_MODE) : S_MODE)

#define _STR(x) _VAL(x)
#define _VAL(x) #x

#if (LOG_DEBUG_MESSAGE != 0)
#define write_log(mode, lhs, level, format, ...)	_log_write(mode, lhs, level, format, _STR(__LINE__), __FILE__, ##__VA_ARGS__)
#else
#define write_log(mode, lhs, level, format, ...)	_log_write(mode, lhs, lh, level, format, ##__VA_ARGS__)
#endif

#if (LOG_FILE_LEVEL >= LOG_ERR && LOG_STREAM_LEVEL >= LOG_ERR)
#define _log_debug(lhs, format, ...) UNUSED
#else
#define _log_debug(lhs, format, ...) write_log(GET_MODE(LOG_FILE_LEVEL, LOG_STREAM_LEVEL, LOG_DEBUG), lhs, (log_level_t)LOG_DEBUG, format, ##__VA_ARGS__)
#endif

#if (LOG_FILE_LEVEL >= LOG_WARN && LOG_STREAM_LEVEL >= LOG_WARN)
#define _log_err(lhs, format, ...) UNUSED
#else
#define _log_err(lhs, format, ...) write_log(GET_MODE(LOG_FILE_LEVEL, LOG_STREAM_LEVEL, LOG_ERR), lhs, (log_level_t)LOG_ERR, format, ##__VA_ARGS__)
#endif

#if (LOG_FILE_LEVEL >= LOG_INFO && LOG_STREAM_LEVEL >= LOG_INFO)
#define _log_warn(lhs, format, ...) UNUSED
#else
#define _log_warn(lhs, format, ...) write_log(GET_MODE(LOG_FILE_LEVEL, LOG_STREAM_LEVEL, LOG_WARN), lhs, (log_level_t)LOG_WARN, format, ##__VA_ARGS__)
#endif

#if (LOG_FILE_LEVEL >= LOG_INFO1 && LOG_STREAM_LEVEL >= LOG_INFO1)
#define _log_info(lhs, format, ...) UNUSED
#else
#define _log_info(lhs, format, ...) write_log(GET_MODE(LOG_FILE_LEVEL, LOG_STREAM_LEVEL, LOG_INFO), lhs, (log_level_t)LOG_INFO, format, ##__VA_ARGS__)
#endif

#if (LOG_FILE_LEVEL == LOG_CLOSE && LOG_STREAM_LEVEL == LOG_CLOSE)
#define _log_info1(lhs, format, ...) UNUSED
#else
#define _log_info1(lhs, format, ...) write_log(GET_MODE(LOG_FILE_LEVEL, LOG_STREAM_LEVEL, LOG_INFO1), lhs, (log_level_t)LOG_INFO1, format, ##__VA_ARGS__)
#endif

typedef enum log_level_t
{
	_DEBUG_LEVEL = LOG_DEBUG,
	_ERR_LEVEL = LOG_ERR,
	_WARN_LEVEL = LOG_WARN,
	_INFO_LEVEL = LOG_INFO,
	_INFO1_LEVEL = LOG_INFO1,
} log_level_t;

#define S_LOG_LEVEL	sizeof(log_level_t)

typedef struct log_handle_t
{
	uint32_t	tag;
	void*		hld;
} log_handle_t;

#define S_LOG_HANDLE_T	sizeof(log_handle_t)

#ifdef __cplusplus
}
#endif

//--------------------------------------------------------------------------------------------------------

//typedef list<log_handle_t *>::iterator log_t;

/**
 * add file or stream handle into handle list
 * @param hlist - log handle list
 * @param hld - log handle(file or stream)
 */
void add_to_handle_list(list<log_handle_t *> &hlist, log_handle_t* hld);

void _log_write(uint32_t mode, list<log_handle_t *> &hlist, log_level_t level, const char* format, ...);

/**
 * flush IO buffer to file.
 * @param hlist - log handle list
 */
void log_flush(list<log_handle_t *> &hlist);

/**
 * destory log handle list.
 * @param hlist - log handle list
 */
void log_destory(list<log_handle_t *> &hlist);

//-------------------------------------------------------------------------------------------------------
/**
 * decipher log file, not remove source file.
 * @param in_filename - source filename.
 * @param out_filename - destination filename
 * @param password - password
 */
int log_file_decipher(const char* in_filename, const char* out_filename, const char* password);

/**
 * uncompress log file, not remove source file.
 * @param src_filename - source filename.
 * @param dst_filename - destination filename
 */
int log_file_uncompress(const char* src_filename, const char* dst_filename);

/**
 * md5 file.
 * @param filename - source filename.
 * @param digest - if digest is NULL, there will be memory allocation.
 * if digest is not NULL, digest size must greater than (32+1('\0')) byte.
 * @return digest, a 32-character fixed-length string.
 */
char* log_file_md5(const char* filename, char* digest);

#ifdef __cplusplus
extern "C" {
#endif

#define NORMALIZE	0    //normal
#define ENCRYPT		1    //log file encryption
#define COMPRESS	2    //log file compress

/**
 * create file log handle.
 * @param fileName - path to save log file.
 * @param fileSize - max size of every log files.
 * @param fileBak - the maxinum number of backup log files.
 * backup file's name is the fileName add backup number at the end. 
 * @param ioBufSize - log IO buffer size, 0 is allowed(no buffer).
 * @param cflag - option, NORMAKUZE, ENCRYPE, COMPREESS or ENCRYPR | COMPRESS.
 * @param password - works if (cflags & ENCRYPT).
 * @return file handle if successful or NULL.
 */
log_handle_t* file_handle_create(const char* fileName, size_t fileSize, size_t fileBak, size_t ioBufSize, uint8_t cflag, const char* password);

/* options for stream handle create */
#define DEBUG_STDERR	1
#define ERR_STRERR	2
#define WARN_STDERR	4
#define INFO_STDERR	8
#define INFO1_STDERR	16

/**
 * create log stream handle
 * @param stream - standard streams for output and error output.
 * values for streams are constructed by a bitwise-inclusive or
 * flags from the above macros list.
 * 0 indicates stdout.
 * @return stream handle.
 */
log_handle_t* stream_handle_create(uint8_t streams);

//font color
#define FBLACK		"\x1B[30m"
#define FRED		"\x1B[31m"
#define FGREEN		"\x1B[32m"
#define FYELLO		"\x1B[33m"
#define FBLUE		"\x1B[34m"
#define FMAGENTA	"\x1B[35m"
#define FCYAN		"\x1B[36m"
#define FWHITE		"\x1B[37m"
//background color
#define BGBLACK		"\x1B[40m"
#define BGRED		"\x1B[41m"
#define BGGREEN		"\x1B[42m"
#define BGYELLOW	"\x1B[43m"
#define BGBLUE		"\x1B[44m"
#define BGMAGENTA	"\x1B[45m"
#define BGCYAN		"\x1B[46m"
#define BGWHITE		"\x1B[47m"
//font style
#define HIGHTLIGHT	"\x1B[1m"
#define UNDERLINE	"\x1B[4m"
#define BLINK		"\x1B[5m"
#define REVERSE		"\x1B[7m"
#define BLANK		"\x1B[8m"

/**
 * set stream handle parameters.
 *
 *
 */
void* set_stream_param(void* sh, log_level_t level, const char* color, const char* bgcolor, const char* style);

/**
 * log print
 * @param lhs - log handle list
 * @param format - C string that contains the text to be written to output.
 * similar to printf's format
 */
#define log_debug(lhs, format, ...)	_log_debug(lhs, format, ##__VA_ARGS__)

#define log_err(lhs, format, ...)	_log_err(lhs, format, ##__VA_ARGS__)

#define log_warn(lhs, format, ...)	_log_warn(lhs, format, ##__VA_ARGS__)

#define log_info(lhs, format, ...)	_log_info(lhs, format, ##__VA_ARGS__)

#define log_info1(lhs, format, ...)	_log_info1(lhs, format, ##__VA_ARGS__)


#ifdef __cplusplus
}
#endif

#endif //__LOG_H__
