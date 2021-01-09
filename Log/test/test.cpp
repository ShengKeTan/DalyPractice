#include "timer.h"
#include "list.h"
#include "Log.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include <assert.h>
#include <mcheck.h>


#define MAX_FILE_SIZE	(10 * 1024)
#define MAX_BACKUP_NUM	2
#define IO_BUFFER_SIZE	(10 * 1024)

#define THREAD_NUMS		4

#define PRINT_TIMES		10

#define UNUSED_RETURN(x)	(void)((x)+1)

typedef struct fileData
{
	int tid;
	char file[128];
} fileData;

void* write_test(void* arg)
{
	int thd  = timekeeper_start_auto();

	fileData *data = (fileData *)arg;

	//UNUSED_RETURN(mkdir(data->file, 0755));

	log_handle_t* fh1 = file_handle_create(data->file, MAX_FILE_SIZE, MAX_BACKUP_NUM, IO_BUFFER_SIZE, NORMALIZE, NULL);

	log_handle_t* sh1 = stream_handle_create(ERR_STRERR);
	sh1 = (log_handle_t *)set_stream_param((void *)sh1, (log_level_t)LOG_ERR, FRED, NULL, UNDERLINE);
	sh1 = (log_handle_t *)set_stream_param((void *)sh1, (log_level_t)LOG_WARN, FBLUE, NULL, UNDERLINE);
	sh1 = (log_handle_t *)set_stream_param((void *)sh1, (log_level_t)LOG_INFO, FGREEN, NULL, NULL);
	//sh1 = (log_handle_t *)set_stream_param((void *)sh1, (log_level_t)LOG_INFO1, FYELLO, NULL, NULL);

	list<log_handle_t *> tlist;
	add_to_handle_list(tlist, fh1);
	add_to_handle_list(tlist, sh1);

	for (int i = 0; i < PRINT_TIMES; ++i)
	{
		log_info1(tlist, "[INFO1]...%d--%s[%d]\n", data->tid, "info1 test!", i);
		log_info(tlist, "[INFO]...%d--%s[%d]\n", data->tid, "info test!", i);
		log_warn(tlist, "[WARN]...%d--%s[%d]\n", data->tid, "warn test!", i);
		log_err(tlist, "[ERROR]...%d--%s[%d]\n", data->tid, "error test!", i);
	}

	log_destory(tlist);

	double time = 0;
	timekeeper_shutoff(thd, &time);
	printf("total time: %lf\n", time);
	return NULL;
}

int main(int argc, char** argv)
{
	//assert(!setenv("MALLOCK_TRACE", "./malloc.log", 1));
	mtrace();
	__u64 begin, end;
	rdtsc(begin);

	UNUSED_RETURN(mkdir("./tmp", 0755));

	if (chdir("./tmp") == -1)
	{
		perror("change dir error!");
		exit(0);
	}

	for (int i = 0; i < THREAD_NUMS; ++i)
	{
		char dir[8] = { 0 };
		sprintf(dir, "%d", i);
		UNUSED_RETURN(mkdir(dir, 0755));
	}

	time_t rawtime = time(NULL);
	struct tm timeinfo;
	localtime_r(&rawtime, &timeinfo);

	pthread_t tid[THREAD_NUMS];
	fileData data[THREAD_NUMS];
	for (int i = 0; i < THREAD_NUMS; ++i)
	{
		sprintf(data[i].file, "./%d/thread[%d]-%04d-%02d-%02d", i, i, timeinfo.tm_year + 1900,
		          timeinfo.tm_mon + 1, timeinfo.tm_mday);
		data[i].tid = i;
		pthread_create(&tid[i], NULL, write_test, (void*)&data[i]);
		sleep(1);
	}

	for (int i = 0; i < THREAD_NUMS; ++i)
	{
		pthread_join(tid[i], NULL);
	}

	rdtsc(end);

	printf("cost %llu CPU cyles\n", end - begin);

	return 0;
}