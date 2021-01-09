#include <cstring>
#include <cstdarg>
#include <ctime>
#include <cstdlib>
#include <sys/syscall.h>

#include <unistd.h>

#include "Log.h"
#include "list.h"
#include "merror.h"
#include "LogFile.h"
#include "LogStream.h"

#define NON_NULL(x)     ((x) ? x : "")

static const char* const s_severity[] = {"[DEBUG]", "[ERR]", "[WARN]", "[INFO]", "[INFO1]", "\0"};

log_handle_t* stream_handle_create(uint8_t streams)
{
	log_handle_t* lh = (log_handle_t *)calloc(1, S_LOG_HANDLE_T);
	if (!lh)
	{
		exit_throw("failed to calloc!");
	}

	lh->tag = S_MODE;
	lh->hld = _stream_handle_create(streams);
	return lh;
}

log_handle_t* file_handle_create(const char* fileName, size_t fileSize, size_t fileBak, size_t ioBufSize, uint8_t cflag, const char* password)
{
	log_handle_t* lh = (log_handle_t *)calloc(1, S_LOG_HANDLE_T);
	if (!lh)
	{
		exit_throw("failed to calloc!");
	}

	lh->tag = F_MODE;
	lh->hld = _file_handle_create(fileName, fileSize, fileBak, ioBufSize, cflag, password);
	return lh;
}

void add_to_handle_list(list<log_handle_t *> &hlist, log_handle_t* hld)
{
	hlist.push_back(hld);
}

void _log_write(uint32_t mode, list<log_handle_t *> &hlist, log_level_t level, const char* format, ...)
{
	if (hlist.empty())
	{
		error_display("Log handle is null!\n");
		return;
	}

	if (!format)
	{
		error_display("Log format is null!\n");
		return;
	}

	va_list args;
	va_start(args, format);

	char log_msg[EX_SINGLE_LOG_SIZE] = { 0 };

/* debuf message */
#if (LOG_DEBUG_MESSAGE != 0)
	time_t rawtime = time(NULL);
	struct tm timeinfo;
	localtime_r(&rawtime, &timeinfo);
	pid_t tid = syscall(SYS_gettid);
	int infoLen = snprintf(log_msg, EX_SINGLE_LOG_SIZE, "%s%04d-%02d-%02d %02d:%02d:%02d %s<%s> %d: ", s_severity[level], 
	                   timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min,
					   timeinfo.tm_sec, va_arg(args, char*), va_arg(args, char*), tid);

#else
	int infoLen = strlen(s_severity[level]);
	strncpy(log_msg, s_severity[level], infoLen);
#endif

	int totalLen = vsnprintf(log_msg + infoLen, SINGLE_LOG_SIZE - infoLen, format, args);
	va_end(args);

	if (totalLen <= 0 || infoLen <= 0 || totalLen > SINGLE_LOG_SIZE - infoLen)
	{
		error_display("Failed to vsnprintf a text entry: (totalLen) %d\n", totalLen);
		return;
	}
	totalLen += infoLen;

	for (list<log_handle_t *>::iterator it = hlist.begin(); it != hlist.end(); ++it)
	{
		switch ((*it)->tag & mode)
		{
		case F_MODE:
			write_file((handle_file_t *)(*it)->hld, log_msg, totalLen);
			break;
		case S_MODE:
			write_stream((handle_stream_t *)(*it)->hld, level, log_msg);
			break;
		default:
			break;
		}
	}
}

void log_flush(list<log_handle_t *> &hlist)
{
	stream_handle_flush();
	
	for (list<log_handle_t *>::iterator it = hlist.begin(); it != hlist.end(); ++it)
	{
		if ((*it)->tag == F_MODE)
		{
			file_handle_flush((handle_file_t *)(*it)->hld);
		}
	} 
}

void log_destory(list<log_handle_t *> &hlist)
{
	for (list<log_handle_t *>::iterator it = hlist.begin(); it != hlist.end(); ++it)
	{
		switch ((*it)->tag)
		{
		case F_MODE:
			file_handle_destory((handle_file_t *)(*it)->hld);
			break;
		case S_MODE:
			stream_handle_destory((handle_stream_t *)(*it)->hld);
			break;	
		default:
			break;
		}
	}
}

void* set_stream_param(void* sh, log_level_t level, const char* color, const char* bgcolor, const char* style)
{
    if (!sh || ((log_handle_t *)sh)->tag != S_MODE || !(((log_handle_t *)sh)->hld))
    {
        error_display("input stream handle error!");
        return sh;
    }

    memset((((handle_stream_t *)(((log_handle_t *)sh)->hld))->style)[level], 0, STYLE_SIZE);
    sprintf((((handle_stream_t *)(((log_handle_t *)sh)->hld))->style)[level], "%s%s%s", NON_NULL(color), NON_NULL(bgcolor), NON_NULL(style));
    return sh;
}

char* log_file_md5(const char* filename, char* digest)
{
	//TODO
	return digest;
}

int log_file_uncompress(const char* src_filename, const char* dst_filename)
{
	//TODO
	return 0;
}





