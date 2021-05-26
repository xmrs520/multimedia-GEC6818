#include "../inc/thread_pool.h"
//压栈实例
void handler(void *arg)
{
  //获取当前取消的ID
  printf("[%u] is endID\n", (unsigned)pthread_self());
  //如果被取消的时候，处于上锁状态，被取消的线程自动解锁，防止死锁
  pthread_mutex_unlock((pthread_mutex_t *)arg);
}
//线程执行的任务  处理任务节点
void *routine(void *arg)
{
  //线程池变量传参  类型强转
  thread_pool *pool = (thread_pool *)arg;
  //用于指向任务结构体的节点指针
  struct task *p;

  while (1) //循环操作任务
  {
    //防止线程处理任务被取消
    pthread_cleanup_push(handler, (void *)&pool->lock);
    //任务资源上锁
    pthread_mutex_lock(&pool->lock);
    //=================================================
    //任务队列中没有任务和线程池销毁，线程处于阻塞状态，等待执行
    while (pool->waiting_tasks == 0 && !pool->shutdown)
    {
      //自动上锁，让线程处于等待状态
      pthread_cond_wait(&pool->cond, &pool->lock);
    }
    //任务队列中没有任务和线程池没有销毁
    if (pool->waiting_tasks == 0 && pool->shutdown == true)
    {
      //自动解锁
      pthread_mutex_unlock(&pool->lock);
      //线程退出
      pthread_exit(NULL);
    }
    //有任务，线程处理数据
    p = pool->task_list->next;       //取出任务节点
    pool->task_list->next = p->next; //向下移动获取任务
    pool->waiting_tasks--;           //任务数量减少
    //=========================================
    //线程解锁，任务完成
    pthread_mutex_unlock(&pool->lock);
    //弹栈 保护资源
    pthread_cleanup_pop(0);
    //如果线程处于任务执行阶段，不能取消线程，先完成任务
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    //执行函数任务
    (p->task)(p->arg);
    //处理完成任务，可以响应取消线程
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    //释放  处理完成节点的任务指针
    free(p);
  }
  //子线程退出
  pthread_exit(NULL);
}
//线程池初始化
bool init_pool(thread_pool *pool, unsigned int threads_number)
{
  //初始化互斥锁
  pthread_mutex_init(&(pool->lock), NULL);
  //初始化条件变量
  pthread_cond_init(&(pool->cond), NULL);
  //初始化线程池的生死标志
  pool->shutdown = false;
  //初始化任务节点 给任务节点申请空间
  pool->task_list = malloc(sizeof(struct task));
  //初始化线程ID
  pool->tids = malloc(sizeof(pthread_t) * MAX_ACTIVE_THREADS);
  //判断节点指针和id指针是否申请失败
  if (pool->task_list == NULL || pool->tids == NULL)
  {
    perror("allocate memory error");
    return false;
  }
  //任务节点的向下的指针初始化为NULL
  pool->task_list->next = NULL;
  //初始化任务数量,等待处理任务数
  pool->waiting_tasks = 0;
  //初始化开始的线程数
  pool->active_threads = threads_number;
  //创建线程申请的数目
  int i;
  for (i = 0; i < pool->active_threads; i++)
  {
    //线程创建
    int ret = pthread_create(&((pool->tids)[i]), NULL, routine, (void *)pool);
    if (ret != 0)
    {
      perror("create threads error");
      return false;
    }
  }

  return true; //初始化成功返回为真
}
//添加任务节点
bool add_task(thread_pool *pool, void *(*task)(void *arg), void *arg)
{
  //申请新的任务节点内存
  struct task *new_task = malloc(sizeof(struct task));
  if (new_task == NULL)
  {
    perror("allocate memory error");
    return false;
  }
  //初始化新的任务节点，给数据域赋值
  new_task->task = task;
  new_task->arg = arg;
  new_task->next = NULL;
  //只要访问任务，就必须上锁
  pthread_mutex_lock(&pool->lock);
  //如果任务数量已经到大最大值，不能添加任务直接走人
  if (pool->waiting_tasks >= MAX_WAITING_TASKS)
  {
    //解锁
    pthread_mutex_unlock(&pool->lock);
    //打印错误信息，然后释放新节点
    fprintf(stderr, "too many tasks.\n");
    free(new_task);

    return false;
  }
  //寻找任务节点尾部
  struct task *tmp = pool->task_list;
  while (tmp->next != NULL)
    tmp = tmp->next;
  //循环结束找末尾，插入新节点
  tmp->next = new_task;
  //任务数+1
  pool->waiting_tasks++;
  // printf("%d\n", pool->waiting_tasks);
  //解锁
  pthread_mutex_unlock(&pool->lock);
  //唤醒线程池中的任意一个线程，准备添加下一个任务
  pthread_cond_signal(&pool->cond);

  return true;
}
//添加线程
int add_thread(thread_pool *pool, unsigned additional_threads)
{
  //如果添加线程数为0
  if (additional_threads == 0)
    return 0;
  //总线程数 = 现有线程数量+新加入的线程数量
  unsigned total_threads = pool->active_threads + additional_threads;
  //记录创建成功的线程总数
  int i, actual_increment = 0;
  //循环创建新的线程，起始位现有线程数，线程总数不能超过最大线程数
  for (i = pool->active_threads;
       i < total_threads && i < MAX_ACTIVE_THREADS;
       i++)
  {
    //创建线程
    if (pthread_create(&((pool->tids)[i]),
                       NULL, routine, (void *)pool) != 0)
    {
      perror("add threads error");
      //线程创建失败，马上退出
      if (actual_increment == 0)
        return -1;

      break;
    }
    actual_increment++; //记录线程数+1
  }
  //现有的线程数据  += 新的线程数
  pool->active_threads += actual_increment;
  return actual_increment; //返回线程数
}
//删除线程
int remove_thread(thread_pool *pool, unsigned int removing_threads)
{
  //删除0条线程
  if (removing_threads == 0)
    return pool->active_threads; //返回现有线程数
                                 //总线程数 = 现有线程数 - 删除线程数
  int remain_threads = pool->active_threads - removing_threads;
  //线程池剩余线程数，不能为0或者负数，最少需要一条线程
  remain_threads = remain_threads > 0 ? remain_threads : 1;
  //循环取消线程
  int i;
  //线程数组的最大下标=数组长度-1
  for (i = pool->active_threads - 1; i > remain_threads - 1; i--)
  {
    //减少对应的线程数
    errno = pthread_cancel(pool->tids[i]);
    //如果取消失败，马上break;
    if (errno != 0)
      break;
  }
  //没有取消任何一条线程
  if (i == pool->active_threads - 1)
    return -1; //异常退出
  else         //有取消线程，但是取消不完全
  {
    //i数组的下标，需要+1得到完整的线程数
    pool->active_threads = i + 1;
    return i + 1;
  }
}
//销毁线程池
bool destroy_pool(thread_pool *pool)
{
  //线程池的存在标志是否为真
  pool->shutdown = true;
  //唤醒所有的线程，让他们恢复解锁装态，全部退出
  pthread_cond_broadcast(&pool->cond);

  int i;
  //循环等待线程回收
  for (i = 0; i < pool->active_threads; i++)
  {
    //回收子线程
    errno = pthread_join(pool->tids[i], NULL);
    if (errno != 0) //回收失败打印错误
    {
      printf("join tids[%d] error: %s\n",
             i, strerror(errno));
    }
    else //子线程退出成功打印编号
      printf("[%u] is joined\n", (unsigned)pool->tids[i]);
  }
  //释放所有内存
  free(pool->task_list);
  free(pool->tids);
  free(pool);

  return true;
}
