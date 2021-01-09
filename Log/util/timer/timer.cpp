#include <ctime>
#include <cstring>

#include "timer.h"

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

struct safe_timespec
{
    struct timespec spec;
    long is_ref;            // 0: free    1: busy
};

#define NSEC_PER_SEC    1000000000

/* 计时器临界区 */
#if defined(_WIN32) || defined(_WIN64)
#define sleep(x)    Sleep(x)
static CRITICAL_SECTION _timekeeper_mutex;
#else
#define atomic_inc(x) __sync_add_and_fetch((x), 1)
static pthread_mutex_t _timekeeper_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

/* 计时器数组 */
static struct safe_timespec _ptimekeeer[TIMEKEEPER_NUM] = { 0 };

/* 多线程初始化标记，防止多次初始化 */
static volatile int _timekeeper_sync = 0;

/* 初始化标志 */
static volatile int _is_initialized = 0;


static void _timekeeper_initialize(void)
{
    if (!_is_initialized)
    {
        /* 原子操作加1，如果结果为1，则只有本线程调用可以初始化 
        *  不为1（> 1），则说明有其他线程调用，等待其他线程初始化完毕即可
        */
    #if defined(_WIN32) || defined(_WIN64)
        if (InterlockedIncrement(&_timekeeper_sync) == 1)
    #else
        if (atomic_inc(&_timekeeper_sync) == 1)
    #endif
        {
        #if defined(_WIN32) || defined(_WIN64)
            InitializedCriticalSection(&_timekeeper_mutex);
        #endif
            memset(_ptimekeeer, 0, TIMEKEEPER_NUM * sizeof(struct safe_timespec));
            _is_initialized = 1;
        }
        else
        {
            /* 等待其他线程完成初始化 */
            while (!_is_initialized)
            {
                sleep(0);
            }
        }
    }
}

static void _timekeeper_lock(void)
{
    _timekeeper_initialize();

#if defined(_WIN32) || defined(_WIN64)
    EnterCriticalSection(_timekeeper_mutex);
#else
    pthread_mutex_lock(&_timekeeper_mutex);
#endif

    if (!_is_initialized)
    {
        memset(_ptimekeeer, 0, TIMEKEEPER_NUM * sizeof(struct safe_timespec));
        _is_initialized = 1;
    }
}

static void _timekeeper_unlock(void)
{
#if defined(_WIN32) || defined(_WIN64)
    LeaveCriticalSection(&_timekeeper_mutex);
#else
    pthread_mutex_unlock(&_timekeeper_mutex);
#endif
}

static int _timekeeper_get(int n)
{
    _timekeeper_lock();

    if (_ptimekeeer[n].is_ref != 0)
    {
        _timekeeper_unlock();
        return -2;
    }

    timespec_get(&(_ptimekeeer[n].spec), TIME_UTC);
    _ptimekeeer[n].is_ref = 1;

    _timekeeper_unlock();
    return 0;
}

int timekeeper_sart_man(int n)
{
    return _timekeeper_get(n);
}

int timekeeper_start_auto()
{
    for (int i = 0; i  < TIMEKEEPER_NUM; ++i)
    {
        if (0 == _timekeeper_get(i))
        {
            return i;
        }
    }
    return -1;
}

int timekeeper_pause(int thnd, double* time)
{
    struct timespec tk;
    if (timespec_get(&tk, TIME_UTC) != 0)
    {
        *time = tk.tv_sec - (_ptimekeeer[thnd].spec.tv_sec)  + 
		(double)(tk.tv_nsec - (_ptimekeeer[thnd].spec).tv_nsec) / NSEC_PER_SEC;
    }
    else
    {
        return -4;
    }
    return 0;
}

int timekeeper_shutoff(int thnd, double* time)
{
    int ret;
    if ((ret = timekeeper_pause(thnd, time)) != 0)
    {
        return ret;
    }

    _timekeeper_lock();
    _ptimekeeer[thnd].is_ref = 0;
    _timekeeper_unlock();
    
    return 0;
}

void timekeeper_destory()
{
    _timekeeper_lock();
    _is_initialized = 0;
    _timekeeper_unlock();
}
