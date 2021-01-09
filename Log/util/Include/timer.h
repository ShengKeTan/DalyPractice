#ifndef __PRO_TIME_H__
#define __PRO_TIME_H__

/* 获取时钟运行周期，除以主频得到时间，秒
   比clock()快一个数量级，依赖<linux/types.h>*/
#define rdtsc(x)    \
{   \
    __u32 lo, hi;   \
    __asm__ __volatile__    \
    (   \
        "rdtsc":"=a"(lo), "=d"(hi)  \
    );  \
    x = (__u64)hi << 32 | lo;   \
}

#ifdef __cplusplus
extern "C" {
#endif

/* 多线程加锁，单线程可以去掉以节省资源 */
#define MULTITHREAD

/* 计时器可使用最大个数 */
#define TIMEKEEPER_NUM 1000

/* 手动计时设置n - [0， TIMEKEEPER_NUM - 1] */
int timekeeper_start_man(int n);

/* 自动选取一个空闲计时器，返回值（ > 0）为输出时钟句柄 thnd */
int timekeeper_start_auto();

/** 获取时间，thnd为输入参数：计时器句柄
* 返回值 > 0, time为输出参数：距上次经过的的时间，单位s
*/
int timekeeper_pause(int thnd, double* time);

/* 获取时间，与pause不同的是获取完成后会清除释放计时器 */
int timekeeper_shutoff(int thnd, double* time);

/* 销毁所有计时器 */
void timekeeper_destory();

#ifdef __cplusplus
}
#endif

#endif // __PRO_TIME_H__ 