#include "../inc/main.h"
#include "../inc/thread_pool.h"
typedef struct test
{
  int pos;
  int volume;
  int pause;
} test;

//播放 发送进度
void *fun1(void *arg)
{
  int *a = malloc(sizeof(int));
  test data;
  //接收参数
  struct arg *args = (struct arg *)arg;
  int shmid = args->arg1;
  pthread_rwlock_t *rwlock = (pthread_rwlock_t *)args->ptr[0];
  int s = atoi(args->arg2);

  //写锁上锁
  if (pthread_rwlock_wrlock(rwlock) != 0)
    printf("写锁上锁 错误 \n");

  //映射内存空间
  test *p = (test *)shmat(shmid, NULL, 0);
  memcpy(&data, p, sizeof(data));
  if (args->arg3 || data.volume)
  {
  }
  *a = data.pause;
  data.pos = s;
  memcpy(p, &data, sizeof(data));

  printf("11111111111111111\n");
  //读写解锁
  pthread_rwlock_unlock(rwlock);

  //子线程退出
  pthread_exit((void *)a);
}

// 退出播放
void *fun2(void *arg)
{
  test data;
  //接收参数
  struct arg *args = (struct arg *)arg;
  int shmid = args->arg1;
  pthread_rwlock_t *rwlock = (pthread_rwlock_t *)args->ptr[0];

  //写锁上锁
  pthread_rwlock_wrlock(rwlock);

  //映射内存空间
  test *p = (test *)shmat(shmid, NULL, 0);
  memcpy(&data, p, sizeof(data));
  data.pause = -1;
  memcpy(p, &data, sizeof(data));

  //读写解锁
  pthread_rwlock_unlock(rwlock);
  //子线程退出
  pthread_exit(NULL);
}

//进度条
void *fun3(void *arg)
{
  int *a = malloc(sizeof(int));
  *a = 0;
  test data;
  //接收参数
  struct arg *args = (struct arg *)arg;
  int shmid = args->arg1;
  pthread_rwlock_t *rwlock = (pthread_rwlock_t *)args->ptr[0];
  int *pos = (int *)args->ptr[1];

  //读锁上锁
  if (pthread_rwlock_rdlock(rwlock) != 0)
    printf("读锁上锁 错误\n");

  //映射内存空间
  test *p = (test *)shmat(shmid, NULL, 0);
  memcpy(&data, p, sizeof(data));
  *pos = data.pos;
  *a = data.pause;
  //读写解锁
  pthread_rwlock_unlock(rwlock);
  //子线程退出
  pthread_exit((void *)a);
}

int main(int argc, char const *argv[])
{
  int shmid;
  //申请键值
  key_t key = ftok(".", 20);
  //申请共享内存
  shmid = shmget(key, 1024, IPC_CREAT | IPC_EXCL | 0777);
  if (shmid == -1 && errno == EEXIST)
  {
    shmid = shmget(key, 1024, 0777);
  }
  else if (shmid == -1)
  {
    perror("shmid error");
  }
  //定义读写锁对象
  pthread_rwlock_t rwlock;

  //初始化读写锁
  pthread_rwlock_init(&rwlock, NULL);

  pid_t pid = fork();

  if (pid == 0)
  { //播放 发送进度
    int s = 0;
    char ss[4];
    int *flag = NULL;
    while (1)
    {
      s++;
      bzero(ss, sizeof(ss));
      sprintf(ss, "%d", s);

      struct arg *task_arg = (struct arg *)malloc(sizeof(struct arg));
      task_arg->arg1 = shmid;
      strcpy(task_arg->arg2, ss);
      task_arg->ptr[0] = (void *)(&rwlock);

      sleep(1);

      pthread_t thread1;
      //创建线程
      pthread_create(&thread1, NULL, fun1, (void *)task_arg);
      //回收子线程
      pthread_join(thread1, (void **)&flag);

      printf("播放接收 是否退出播放 %d\n", *flag);
      if (*flag == -1)
      {
        break;
      }
    }
    printf("播放结束 \n");
  }

  if (pid > 0)
  {
    pid_t pid2 = fork();
    if (pid2 == 0)
    { // 退出播放
      // sleep(10);

      // struct arg *task_arg = (struct arg *)malloc(sizeof(struct arg));
      // task_arg->arg1 = shmid;
      // task_arg->ptr[0] = (void *)(&rwlock);

      // pthread_t thread3;
      // //创建线程
      // pthread_create(&thread3, NULL, fun2, (void *)task_arg);
      // //回收子线程
      // pthread_join(thread3, NULL);

      printf("退出播放结束\n");
    }

    if (pid2 > 0)
    { //进度条

      int *flag = NULL;
      int pos = 0;
      while (1)
      {
        sleep(1);
        struct arg *task_arg = (struct arg *)malloc(sizeof(struct arg));
        task_arg->arg1 = shmid;
        task_arg->ptr[0] = (void *)(&rwlock);
        task_arg->ptr[1] = (void *)&pos;

        pthread_t thread2;
        //创建线程
        pthread_create(&thread2, NULL, fun3, (void *)task_arg);
        printf("进度条 读取到进度%d\n", pos);
        //回收子线程
        pthread_join(thread2, (void **)&flag);

        printf("进度条 是否退出播放 %d\n", *flag);
        if (*flag == -1)
        {
          break;
        }
      }
      printf("进度条结束\n");
    }
    wait(NULL);
    wait(NULL);
  }
  return 0;
}
