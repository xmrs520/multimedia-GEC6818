#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <errno.h>
#include <pthread.h>

#define MAX_WAITING_TASKS 1000 //任务最大数量
#define MAX_ACTIVE_THREADS 20  //线程最大的数量

/*任务结构体 负责布置任务，单向链表操作*/
struct task
{
  void *(*task)(void *arg); //任务需要执行的事件
  void *arg;                //任务函数需要的参数
  struct task *next;        //指向下一任务的指针
};
/*线程池结构*/
typedef struct thread_pool
{
  pthread_mutex_t lock;   //互斥锁的资源管理
  pthread_cond_t cond;    //条件变量管理，线程的睡眠和唤醒
  struct task *task_list; //指向任务结构体，寻找任务节点

  pthread_t *tids; //线程的编号

  unsigned int waiting_tasks;  //任务数量
  unsigned int active_threads; //线程数量

  bool shutdown; //线程池的生死状态
} thread_pool;   //给struct thread_pool取别名为thread_pool

//线程池初始化
bool init_pool(thread_pool *pool, unsigned int threads_number);
//任务节点添加
bool add_task(thread_pool *pool, void *(*task)(void *arg), void *arg);
//添加线程数量
int add_thread(thread_pool *pool, unsigned int additional_threads_number);
//删除线程数量
int remove_thread(thread_pool *pool, unsigned int removing_threads_number);
//销毁线程池
bool destroy_pool(thread_pool *pool);
//线程函数的执行
void *routine(void *arg);

#endif
