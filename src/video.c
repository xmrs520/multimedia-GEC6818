#include "../inc/main.h"

/**
 * 初始化视频列表
*/
video_list_p video_list_init()
{
  video_list_p head = (video_list_p)malloc(sizeof(video_list));
  if (head != NULL)
  {
    strcpy(head->name, "初始化");
    strcpy(head->path, "初始化");
    head->index = 0;
    head->page = 0;
    head->next = head;
    head->prev = head;
    return head;
  }
  else
  {
    printf("video allocate failed\n");
    return NULL;
  }
}

/**
 * 创建视频列表
*/
bool video_create_node(video_list_p head, char *names, char *paths, int indexs, int pages)
{
  video_list_p p = head;
  video_list_p node = (video_list_p)malloc(sizeof(video_list));
  if (node == NULL)
  {
    printf("video allocate failed\n");
    return false;
  }

  strcpy(node->name, names);
  strcpy(node->path, paths);
  node->index = indexs;
  node->page = pages;

  while (p->next != head)
  {
    p = p->next;
  }
  p->next = node;
  node->prev = p;
  node->next = head;
  head->prev = node;
  return true;
}

/**
 * 寻找列表中第 n 视频
*/
video_list_p find_video(video_list_p head, int *n)
{
  video_list_p p = head->next;
  while (p != head)
  {
    if (p->index == *n)
    {
      return p;
    }
    p = p->next;
  }
  return NULL;
}

/**
 * 销毁视频列表
*/
void destroy_video_list(video_list_p head)
{
  video_list_p p = head->next;
  video_list_p q = p->next;
  while (q != head)
  {
    free(p);
    p = q;
    q = p->next;
  }
  free(p);
  free(q);
}

/**
 * 打印视频列表
*/
void display_video_list(video_list_p head)
{
  printf("---------初始化视频列表---------\n");
  video_list_p p = head->next;
  while (p != head)
  {
    printf("name = %s \n", p->name);
    printf("path = %s \n", p->path);
    printf("index = %d \n", p->index);
    printf("page = %d \n", p->page);
    p = p->next;
  }
}

/**
 * 显示视频列表到界面
*/
int show_video_list(video_list_p video_head, int pages, int indexs)
{
  int tail = 0; //当前页总共个数
  char path[100];
  video_list_p p = video_head->next;
  int offset = 0;
  while ((p != video_head) && (p->page == pages))
  {
    bzero(path, sizeof(path));
    read_pictrue_path("video_icon", path);
    display_picture(path, 555, 42 + offset, false);
    if (indexs == p->index)
      font_tools_2(p->name, 620, 45 + offset, 0, 25, 24, 0x00ffb923, 181, 70, 0x00262626);
    else
      font_tools_2(p->name, 620, 45 + offset, 0, 25, 24, 0x00ffffff, 181, 70, 0x00262626);
    offset += 70;
    tail++;
    p = p->next;
  }
  return tail;
}

/**
 * 视频首页初始化
 * tail: 当前页总共个数
 * index：视频序号
*/
int init_video_interface(video_list_p video_head, int *tail, int indexs)
{
  int max;
  char path[100];
  bzero(path, sizeof(path));
  read_pictrue_path("video_bg", path);
  display_picture(path, 0, 0, false);
  bzero(path, sizeof(path));
  read_pictrue_path("bofang_v", path);
  display_picture(path, 0, 439, false);
  bzero(path, sizeof(path));
  read_pictrue_path("shengyin_v", path);
  display_picture(path, 310, 440, false);
  max = read_video_list(video_head);
  display_video_list(video_head);
  *tail = show_video_list(video_head, 1, indexs);
  //命令有名管道
  make_fifo();
  return max;
}

/**
 * 视频首页
*/
void loading_video_interface(video_list_p video_head, int pages, int indexs)
{
  char path[100];
  bzero(path, sizeof(path));
  read_pictrue_path("video_bg", path);
  display_picture(path, 0, 0, false);
  bzero(path, sizeof(path));
  read_pictrue_path("bofang_v", path);
  display_picture(path, 0, 439, false);
  bzero(path, sizeof(path));
  read_pictrue_path("shengyin_v", path);
  display_picture(path, 310, 440, false);
  show_video_list(video_head, pages, indexs);
}
/**
 * 视频列表处理
 * No: 当前页的第n个视频
*/
void video_list_handle(int *No, int tail, xy *p)
{
  int y;
  y = p->y;
  if (is_key_area(p, 555, 42, tail * 70, 800))
  {
    *No = (y - 42) / 70;
  }
}

//任务线程 读数据
void *read_data_thread(void *arg)
{
  char pos[10];
  char volume[4];
  char pause[1];
  shmbuf data;
  //接收参数
  struct arg *args = (struct arg *)arg;
  int shmid = args->arg1;
  pthread_rwlock_t *rwlock = (pthread_rwlock_t *)args->ptr[0];

  //读锁上锁
  if (pthread_rwlock_rdlock(rwlock) != 0)
    printf("读锁上锁 错误 \n");

  //映射内存空间
  shmbuf *share = (shmbuf *)shmat(shmid, NULL, 0);
  memcpy(&data, share, sizeof(data));
  sprintf(pos, "%f", data.pos);
  sprintf(volume, "%d", data.volume);
  sprintf(pause, "%d", data.pause);

  if (args->arg4[0] != NULL)
    strcpy(args->arg4[0], pos);
  if (args->arg4[1] != NULL)
    strcpy(args->arg4[1], volume);
  if (args->arg4[2] != NULL)
    strcpy(args->arg4[2], pause);

  shmdt(share);
  //读写解锁
  pthread_rwlock_unlock(rwlock);
  //子线程退出
  pthread_exit(NULL);
}

//任务线程 写数据
void *write_data_thread(void *arg)
{
  shmbuf data;
  //接收参数
  struct arg *args = (struct arg *)arg;
  int shmid = args->arg1;
  pthread_rwlock_t *rwlock = (pthread_rwlock_t *)args->ptr[0];

  //写锁上锁
  if (pthread_rwlock_wrlock(rwlock) != 0)
    printf("写锁上锁 错误 \n");

  //映射内存空间
  shmbuf *share = (shmbuf *)shmat(shmid, NULL, 0);
  memcpy(&data, share, sizeof(data));
  if (args->arg4[0] != NULL)
    data.pos = atof(args->arg4[0]);
  if (args->arg4[1] != NULL)
    data.volume = atoi(args->arg4[1]);
  if (args->arg4[2] != NULL)
    data.pause = atoi(args->arg4[2]);
  memcpy(share, &data, sizeof(data));

  shmdt(share);
  //读写解锁
  pthread_rwlock_unlock(rwlock);
  //子线程退出
  pthread_exit(NULL);
}

//进度条读取 总长度
void *bar_read_time(void *arg)
{
  char time[100];
  shmbuf data;
  //接收参数
  struct arg *args = (struct arg *)arg;
  int shmid = args->arg1;
  pthread_rwlock_t *rwlock = (pthread_rwlock_t *)args->ptr[0];

  //读锁上锁
  if (pthread_rwlock_rdlock(rwlock) != 0)
    printf("读锁上锁 错误 \n");

  //映射内存空间
  shmbuf *share = (shmbuf *)shmat(shmid, NULL, 0);
  memcpy(&data, share, sizeof(data));
  sprintf(time, "%d", data.time);
  if (args->arg4[3] != NULL)
    strcpy(args->arg4[3], time);
  shmdt(share);
  //读写解锁
  pthread_rwlock_unlock(rwlock);
  //子线程退出
  pthread_exit(NULL);
}
//进度条读取 播放状态
void *bar_read_play_flag(void *arg)
{
  char pause[1];
  shmbuf data;
  //接收参数
  struct arg *args = (struct arg *)arg;
  int shmid = args->arg1;
  pthread_rwlock_t *rwlock = (pthread_rwlock_t *)args->ptr[0];

  //读锁上锁
  if (pthread_rwlock_rdlock(rwlock) != 0)
    printf("读锁上锁 错误 \n");

  //映射内存空间
  shmbuf *share = (shmbuf *)shmat(shmid, NULL, 0);
  memcpy(&data, share, sizeof(data));
  sprintf(pause, "%d", data.pause);
  strcpy(args->arg4[2], pause);

  shmdt(share);
  //读写解锁
  pthread_rwlock_unlock(rwlock);
  //子线程退出
  pthread_exit(NULL);
}
//进度条读取 进度
void *bar_read_pos(void *arg)
{
  char pos[10];
  shmbuf data;
  //接收参数
  struct arg *args = (struct arg *)arg;
  int shmid = args->arg1;
  pthread_rwlock_t *rwlock = (pthread_rwlock_t *)args->ptr[0];

  //读锁上锁
  if (pthread_rwlock_rdlock(rwlock) != 0)
    printf("读锁上锁 错误 \n");

  //映射内存空间
  shmbuf *share = (shmbuf *)shmat(shmid, NULL, 0);
  memcpy(&data, share, sizeof(data));
  sprintf(pos, "%f", data.pos);
  if (args->arg4[0] != NULL)
    strcpy(args->arg4[0], pos);

  shmdt(share);
  //读写解锁
  pthread_rwlock_unlock(rwlock);
  //子线程退出
  pthread_exit(NULL);
}
//播放读取 播放状态
void *play_read_play_flag(void *arg)
{
  char pause[1];
  shmbuf data;
  //接收参数
  struct arg *args = (struct arg *)arg;
  int shmid = args->arg1;
  pthread_rwlock_t *rwlock = (pthread_rwlock_t *)args->ptr[0];

  //读锁上锁
  if (pthread_rwlock_rdlock(rwlock) != 0)
    printf("读锁上锁 错误 \n");

  //映射内存空间
  shmbuf *share = (shmbuf *)shmat(shmid, NULL, 0);
  memcpy(&data, share, sizeof(data));
  sprintf(pause, "%d", data.pause);
  strcpy(args->arg4[2], pause);

  shmdt(share);
  //读写解锁
  pthread_rwlock_unlock(rwlock);
  //子线程退出
  pthread_exit(NULL);
}
//播放发送 播放进度
void *play_write_pos(void *arg)
{
  shmbuf data;
  //接收参数
  struct arg *args = (struct arg *)arg;
  int shmid = args->arg1;
  pthread_rwlock_t *rwlock = (pthread_rwlock_t *)args->ptr[0];

  //写锁上锁
  if (pthread_rwlock_wrlock(rwlock) != 0)
    printf("写锁上锁 错误 \n");

  //映射内存空间
  shmbuf *share = (shmbuf *)shmat(shmid, NULL, 0);
  memcpy(&data, share, sizeof(data));
  if (args->arg4[0] != NULL)
    data.pos = atof(args->arg4[0]);
  memcpy(share, &data, sizeof(data));

  shmdt(share);
  //读写解锁
  pthread_rwlock_unlock(rwlock);
  //子线程退出
  pthread_exit(NULL);
}
//播放发送 总长度
void *play_write_time(void *arg)
{
  shmbuf data;
  //接收参数
  struct arg *args = (struct arg *)arg;
  int shmid = args->arg1;
  pthread_rwlock_t *rwlock = (pthread_rwlock_t *)args->ptr[0];

  //写锁上锁
  if (pthread_rwlock_wrlock(rwlock) != 0)
    printf("写锁上锁 错误 \n");

  //映射内存空间
  shmbuf *share = (shmbuf *)shmat(shmid, NULL, 0);
  memcpy(&data, share, sizeof(data));
  if (args->arg4[3] != NULL)
    data.time = atoi(args->arg4[3]);

  memcpy(share, &data, sizeof(data));

  shmdt(share);
  //读写解锁
  pthread_rwlock_unlock(rwlock);
  //子线程退出
  pthread_exit(NULL);
}
//暂停 发送播放状态
void *pause_write_play_flag(void *arg)
{
  shmbuf data;
  //接收参数
  struct arg *args = (struct arg *)arg;
  int shmid = args->arg1;
  pthread_rwlock_t *rwlock = (pthread_rwlock_t *)args->ptr[0];

  //写锁上锁
  if (pthread_rwlock_wrlock(rwlock) != 0)
    printf("写锁上锁 错误 \n");

  //映射内存空间
  shmbuf *share = (shmbuf *)shmat(shmid, NULL, 0);
  memcpy(&data, share, sizeof(data));
  if (args->arg4[2] != NULL)
    data.pause = atoi(args->arg4[2]);
  memcpy(share, &data, sizeof(data));

  shmdt(share);
  //读写解锁
  pthread_rwlock_unlock(rwlock);
  //子线程退出
  pthread_exit(NULL);
}
//退出
void *video_return_write(void *arg)
{
  shmbuf data;
  //接收参数
  struct arg *args = (struct arg *)arg;
  int shmid = args->arg1;
  pthread_rwlock_t *rwlock = (pthread_rwlock_t *)args->ptr[0];

  //写锁上锁
  if (pthread_rwlock_wrlock(rwlock) != 0)
    printf("写锁上锁 错误 \n");

  //映射内存空间
  shmbuf *share = (shmbuf *)shmat(shmid, NULL, 0);
  memcpy(&data, share, sizeof(data));
  if (args->arg2 != NULL)
    data.pause = atoi(args->arg2);
  memcpy(share, &data, sizeof(data));
  shmdt(share);
  //读写解锁
  pthread_rwlock_unlock(rwlock);
  //子线程退出
  pthread_exit(NULL);
}
/**
 * 进度条处理
*/
void video_handle_bar(int time, int s)
{
  int pos;
  int lcd_fd = open(LCD_PATH, O_RDWR);
  if (lcd_fd < 0)
  {
    perror("Failed to open lcd\n");
  }
  int x = 0, y;
  int color = 0x003486fa;
  int *share_addr = NULL;
  share_addr = mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, lcd_fd, 0);
  if (share_addr == MAP_FAILED)
  {
    perror("mmap failed\n");
  }
  //printf("handle_bar ---m--- %d \n", m);
  pos = (int)ceil(550 / (double)time * s);
  for (x = 0; x <= pos; x++)
  {
    for (y = 0; y < 5; y++)
      memcpy(share_addr + x + (y + 435) * 800, &color, 4);
  }
  munmap(share_addr, 800 * 480 * 4);
  close(lcd_fd);
}

/**
 * 进度条
*/
void video_progress_bar(int shmid, pthread_rwlock_t *rwlock)
{
  char time_str[10];
  char pos[4];
  char play_flag[1] = {"1"};
  char time[100];
  int i = 0;
  while (1)
  {
    i++;
    if (i < 4)
    {
      //创建任务参数 总时长
      struct arg *task_arg4 = (struct arg *)malloc(sizeof(struct arg));
      task_arg4->arg1 = shmid;
      task_arg4->ptr[0] = (void *)rwlock;
      task_arg4->arg4[3] = time;
      //读取总时长
      pthread_t thread5;
      //创建线程
      pthread_create(&thread5, NULL, bar_read_time, (void *)task_arg4);
      //回收子线程
      pthread_join(thread5, NULL);
    }

    //创建任务参数 播放标志
    struct arg *task_arg1 = (struct arg *)malloc(sizeof(struct arg));
    task_arg1->arg1 = shmid;
    task_arg1->ptr[0] = (void *)rwlock;
    task_arg1->arg4[2] = play_flag;
    //读取播放标志
    pthread_t thread1;
    //创建线程
    pthread_create(&thread1, NULL, bar_read_play_flag, (void *)task_arg1);
    //回收子线程
    pthread_join(thread1, NULL);
    printf("进度条接收 play_flag %s \n", play_flag);

    if (atoi(play_flag) == 0)
      strcpy(play_flag, "1");
    if (atoi(play_flag) == 2)
    {
      sleep(1);
      continue;
    }
    else if (atoi(play_flag) == -1)
    {
      break;
    }
    else if (atoi(play_flag) == 1)
    {
      sleep(1);
      //创建任务参数
      struct arg *task_arg = (struct arg *)malloc(sizeof(struct arg));
      task_arg->arg1 = shmid;
      task_arg->ptr[0] = (void *)rwlock;
      task_arg->arg4[0] = pos;
      pthread_t thread;
      //创建线程
      pthread_create(&thread, NULL, bar_read_pos, (void *)task_arg);
      //回收子线程
      pthread_join(thread, NULL);
      video_handle_bar(atoi(time), atoi(pos));
      printf("进度条 %s \n", pos);
      time_format(atoi(pos), time_str, true);
      font_tools(time_str, 68, 451, 20, 0x00ffffff, 63, 20, 0x00000000);
    }
  }
  printf("log-xmrs: 进度条结束\n");
}

/**
 * 播放
*/
void vdieo_play(int shmid, int msgid, pthread_rwlock_t *rwlock, video_list_p list, char *pos)
{
  float x = 800.0 * 0.65;
  float y = 480.0 * 0.65;
  char vdieo_cmd[512];
  char buf[256];
  char tmp[5];
  char time_str[10];
  char play_flag[2] = {"1"};
  char play_pos[4];
  FILE *mp;
  bool flag = false;

  //打印暂停图片
  char path[100];
  bzero(path, sizeof(path));
  read_pictrue_path("zanting_v", path);
  display_picture(path, 0, 439, false);

  system("killall -9 mplayer");
  bzero(vdieo_cmd, sizeof(vdieo_cmd));
  sprintf(vdieo_cmd,
          "mplayer -slave -quiet -input file=/tmp/fifo %s -zoom -geometry 15:80  -x %d -y %d  &",
          list->path, (int)x, (int)y);
  mp = popen(vdieo_cmd, "r");

  //打开管道文件
  int fifo_fd = open("/tmp/fifo", O_RDWR);
  if (fifo_fd == -1)
  {
    perror("open 管道 error!");
  }

  //获取总时长
  write(fifo_fd, "get_time_length\n", strlen("get_time_length\n"));
  do
  {
    bzero(buf, sizeof(buf));
    fgets(buf, sizeof(buf), mp);
    printf("%s", buf);
  } while (strncmp(buf, "ANS_LENGTH=", strlen("ANS_LENGTH=")) != 0);
  bzero(tmp, sizeof(tmp));
  sscanf(buf, "%*[^=]=%s", tmp);
  time_format(atoi(tmp), time_str, true);
  printf("总时长 %s \n", time_str);
  font_tools(time_str, 151, 451, 20, 0x00ffffff, 61, 20, 0x00000000);

  //创建任务参数
  struct arg *task_arg3 = (struct arg *)malloc(sizeof(struct arg));
  task_arg3->arg1 = shmid;
  task_arg3->ptr[0] = (void *)rwlock;
  task_arg3->arg4[3] = tmp;
  //发送总时长
  pthread_t thread4;
  //创建线程
  pthread_create(&thread4, NULL, play_write_time, (void *)task_arg3);
  //回收子线程
  pthread_join(thread4, NULL);

  // 接收播放状态
  //struct msgbuf recvflag;

  //发送进度
  while (1)
  {

    //创建任务参数
    struct arg *task_arg1 = (struct arg *)malloc(sizeof(struct arg));
    task_arg1->arg1 = shmid;
    task_arg1->ptr[0] = (void *)rwlock;
    task_arg1->arg4[2] = play_flag;

    //读取播放标志
    pthread_t thread1;
    //创建线程
    pthread_create(&thread1, NULL, play_read_play_flag, (void *)task_arg1);
    //回收子线程
    pthread_join(thread1, NULL);

    if (atoi(play_flag) == 0)
      strcpy(play_flag, "1");

    // msgrcv(msgid, &recvflag, sizeof(recvflag), 500, IPC_NOWAIT);
    // if (recvflag.flag != 0)
    // {
    //   sprintf(play_flag, "%d", recvflag.flag);
    // }
    // printf("播放接收 play_flag %s \n", play_flag);

    if (atoi(play_flag) == 2)
    {
      if (!flag)
      {
        write(fifo_fd, "pause\n", strlen("pause\n"));
        flag = true;
      }
      sleep(1);
      continue;
    }
    else if (atoi(play_flag) == -1 || atoi(play_pos) > atoi(tmp) - 5)
    {
      break;
    }
    else if (atoi(play_flag) == 1)
    {
      if (flag)
      {
        write(fifo_fd, "pause\n", strlen("pause\n"));
        flag = false;
      }
      sleep(1);
      //获取进度
      if (write(fifo_fd, "get_time_pos\n", strlen("get_time_pos\n")) != strlen("get_time_pos\n"))
        perror("write get_time_pos");
      do
      {
        bzero(buf, sizeof(buf));
        fgets(buf, sizeof(buf), mp);
        printf("%s", buf);
      } while (strncmp(buf, "ANS_TIME_POSITION=", strlen("ANS_TIME_POSITION=")) != 0);
      bzero(play_pos, sizeof(play_pos));
      sscanf(buf, "%*[^=]=%s", play_pos);
      //printf("play_pos %s \n", play_pos);

      //创建任务参数
      struct arg *task_arg = (struct arg *)malloc(sizeof(struct arg));
      task_arg->arg1 = shmid;
      task_arg->ptr[0] = (void *)rwlock;
      task_arg->arg4[0] = play_pos; // 发送进度
      pthread_t thread;
      //创建线程
      pthread_create(&thread, NULL, play_write_pos, (void *)task_arg);
      //回收子线程
      pthread_join(thread, NULL);
    }
  }
  system("killall -9 mplayer");
  pclose(mp);
  close(fifo_fd);
  printf("log-xmrs: 播放进程结束 \n");
}

/**
 * 暂停
*/
void voide_pause(bool *flag, int shmid, int msgid, pthread_rwlock_t *rwlock)
{
  char pause[1] = {"0"};
  //打开管道文件
  // int fifo_fd = open("/tmp/fifo", O_RDWR);
  // if (fifo_fd == -1)
  // {
  //   perror("open 管道 error!");
  // }
  //从 播放---->暂停
  if (!(*flag))
  {
    // //发送播放状态
    // struct msgbuf sendflag;
    // sendflag.mtype = 500;
    // sendflag.flag = 2;
    // if (msgsnd(msgid, &sendflag, sizeof(sendflag.flag), 0) != 0)
    //   perror("发送播放状态 500号 失败");

    strcpy(pause, "2");
    //创建任务参数
    struct arg *task_arg = (struct arg *)malloc(sizeof(struct arg));
    task_arg->arg1 = shmid;
    task_arg->ptr[0] = (void *)rwlock;
    task_arg->arg4[2] = pause;

    pthread_t thread1;
    //创建线程
    pthread_create(&thread1, NULL, pause_write_play_flag, (void *)task_arg);
    //回收子线程
    pthread_join(thread1, NULL);

    //打印播放图片
    char path[100];
    bzero(path, sizeof(path));
    read_pictrue_path("bofang_v", path);
    display_picture(path, 0, 439, false);

    //write(fifo_fd, "pause\n", strlen("pause\n"));
    *flag = true;
  }
  //从 暂停---->播放
  else
  {
    // //发送播放状态
    // struct msgbuf sendflag;
    // sendflag.mtype = 500;
    // sendflag.flag = 1;
    // if (msgsnd(msgid, &sendflag, sizeof(sendflag.flag), 0) != 0)
    //   perror("发送播放状态 500号 失败");

    strcpy(pause, "1");
    //创建任务参数
    struct arg *task_arg = (struct arg *)malloc(sizeof(struct arg));
    task_arg->arg1 = shmid;
    task_arg->ptr[0] = (void *)rwlock;
    task_arg->arg4[2] = pause;

    pthread_t thread;
    //创建线程
    pthread_create(&thread, NULL, pause_write_play_flag, (void *)task_arg);
    //回收子线程
    pthread_join(thread, NULL);

    //打印暂停图片
    char path[100];
    bzero(path, sizeof(path));
    read_pictrue_path("zanting_v", path);
    display_picture(path, 0, 439, false);

    // write(fifo_fd, "pause\n", strlen("pause\n"));
    *flag = false;
  }
  //close(fifo_fd);
  printf("log-xmrs:暂停结束\n");
}

/**
 * 静音
*/

/**
 * 全屏播放
*/
void full_screen(int shmid, int msgid, pthread_rwlock_t *rwlock, video_list_p list, xy *p)
{
  char seek[100];
  char play_pos[4];
  char pos[4];
  char vdieo_cmd[512];
  char buf[256];
  FILE *mp;
  char pause[1] = {"1"};
  bool flag = false;
  char play_flag[2] = {"1"};
  char tmp[5];
  int i = 0;
  /*发送播放结束*/
  //创建任务参数
  struct arg *task_arg3 = (struct arg *)malloc(sizeof(struct arg));
  task_arg3->arg1 = shmid;
  task_arg3->ptr[0] = (void *)rwlock;
  strcpy(task_arg3->arg2, "-1");
  //发送播放结束
  pthread_t thread4;
  //创建线程
  pthread_create(&thread4, NULL, video_return_write, (void *)task_arg3);
  //回收子线程
  pthread_join(thread4, NULL);

  sleep(1);

  /*读取进度*/
  //创建任务参数
  struct arg *task_arg = (struct arg *)malloc(sizeof(struct arg));
  task_arg->arg1 = shmid;
  task_arg->ptr[0] = (void *)rwlock;
  task_arg->arg4[0] = pos;
  pthread_t thread8;
  //创建线程
  pthread_create(&thread8, NULL, bar_read_pos, (void *)task_arg);
  //回收子线程
  pthread_join(thread8, NULL);

  system("killall -9 mplayer");

  bzero(vdieo_cmd, sizeof(vdieo_cmd));
  sprintf(vdieo_cmd,
          "mplayer -slave -quiet -input file=/tmp/fifo %s &",
          list->path);
  mp = popen(vdieo_cmd, "r");

  //打开管道文件
  int fifo_fd = open("/tmp/fifo", O_RDWR);
  if (fifo_fd == -1)
  {
    perror("open 管道 error!");
  }

  if (atoi(pos) < 3)
    strcpy(pos, "2");
  sprintf(seek, "seek %s\n", pos);
  write(fifo_fd, seek, strlen(seek) + 1);

  //创建任务参数 发送开始播放 “1”
  struct arg *task_arg2 = (struct arg *)malloc(sizeof(struct arg));
  task_arg2->arg1 = shmid;
  task_arg2->ptr[0] = (void *)rwlock;
  task_arg2->arg4[2] = pause;

  pthread_t thread;
  //创建线程
  pthread_create(&thread, NULL, pause_write_play_flag, (void *)task_arg2);
  //回收子线程
  pthread_join(thread, NULL);

  //发送进度
  while (1)
  {
    sleep(1);
    //创建任务参数 读取播放标志
    struct arg *task_arg1 = (struct arg *)malloc(sizeof(struct arg));
    task_arg1->arg1 = shmid;
    task_arg1->ptr[0] = (void *)rwlock;
    task_arg1->arg4[2] = play_flag;

    // 读取播放标志
    pthread_t thread1;
    //创建线程
    pthread_create(&thread1, NULL, play_read_play_flag, (void *)task_arg1);
    //回收子线程
    pthread_join(thread1, NULL);

    printf("全屏播放 play_flag %s \n", play_flag);

    if (atoi(play_flag) == 0)
      strcpy(play_flag, "1");

    if (atoi(play_flag) == 2)
    {
      if (!flag)
      {
        printf("--------------------\n");
        //获取进度
        // if (write(fifo_fd, "get_time_pos\n", strlen("get_time_pos\n")) != strlen("get_time_pos\n"))
        //   perror("write get_time_pos");

        write(fifo_fd, "get_time_pos\n", strlen("get_time_pos\n"));
        do
        {
          bzero(buf, sizeof(buf));
          fgets(buf, sizeof(buf), mp);
          printf("%s", buf);
          write(fifo_fd, "get_time_pos\n", strlen("get_time_pos\n"));
        } while (strncmp(buf, "ANS_TIME_POSITION=", strlen("ANS_TIME_POSITION=")) != 0);
        bzero(play_pos, sizeof(play_pos));
        sscanf(buf, "%*[^=]=%s", play_pos);
        printf("play_pos %s \n", play_pos);

        //创建任务参数 发送进度
        struct arg *task_arg = (struct arg *)malloc(sizeof(struct arg));
        task_arg->arg1 = shmid;
        task_arg->ptr[0] = (void *)rwlock;
        task_arg->arg4[0] = play_pos; // 发送进度
        pthread_t thread;
        //创建线程
        pthread_create(&thread, NULL, play_write_pos, (void *)task_arg);
        //回收子线程
        pthread_join(thread, NULL);

        write(fifo_fd, "pause\n", strlen("pause\n"));
        flag = true;
        printf("全屏播放 暂停\n");
      }
    }
    else if (atoi(play_flag) == -1)
    {
      break;
    }
    else if (atoi(play_flag) == 1)
    {
      if (flag)
      {
        printf("全屏播放 播放\n");
        write(fifo_fd, "pause\n", strlen("pause\n"));
        flag = false;
      }
    }
  }
  system("killall -9 mplayer");
  printf("log-xmrs: 全屏播放结束\n");
  pclose(mp);
  close(fifo_fd);
}

/**
 * 全屏控制
*/
void full_screen_control(int shmid, pthread_rwlock_t *rwlock, video_list_p video_head, int pages, int n, xy *p)
{
  char buf[100];
  char path[100];
  char pos[4];
  char pause[1] = {"1"};
  char time[100];
  char time_str[10];

  //从 播放---->暂停
  strcpy(pause, "2");
  //创建任务参数
  struct arg *task_arg9 = (struct arg *)malloc(sizeof(struct arg));
  task_arg9->arg1 = shmid;
  task_arg9->ptr[0] = (void *)rwlock;
  task_arg9->arg4[2] = pause;

  pthread_t thread1;
  //创建线程
  pthread_create(&thread1, NULL, pause_write_play_flag, (void *)task_arg9);
  //回收子线程
  pthread_join(thread1, NULL);

  sleep(1);
  bzero(path, sizeof(path));
  read_pictrue_path("full_top", path);
  display_picture(path, 0, 0, false);
  bzero(path, sizeof(path));
  read_pictrue_path("full_bottom", path);
  display_picture(path, 0, 442, false);

  //创建任务参数 总时长
  struct arg *task_arg4 = (struct arg *)malloc(sizeof(struct arg));
  task_arg4->arg1 = shmid;
  task_arg4->ptr[0] = (void *)rwlock;
  task_arg4->arg4[3] = time;
  //读取总时长
  pthread_t thread5;
  //创建线程
  pthread_create(&thread5, NULL, bar_read_time, (void *)task_arg4);
  //回收子线程
  pthread_join(thread5, NULL);
  time_format(atoi(time), time_str, true);
  font_tools(time_str, 204, 453, 20, 0x00ffffff, 63, 20, 0x00000000);

  /*读取进度*/
  //创建任务参数
  struct arg *task_arg = (struct arg *)malloc(sizeof(struct arg));
  task_arg->arg1 = shmid;
  task_arg->ptr[0] = (void *)rwlock;
  task_arg->arg4[0] = pos;
  pthread_t thread;
  //创建线程
  pthread_create(&thread, NULL, bar_read_pos, (void *)task_arg);
  //回收子线程
  pthread_join(thread, NULL);
  time_format(atoi(pos), time_str, true);
  font_tools(time_str, 122, 453, 20, 0x00ffffff, 63, 20, 0x00000000);

  int pos_t;
  int lcd_fd = open(LCD_PATH, O_RDWR);
  if (lcd_fd < 0)
  {
    perror("Failed to open lcd\n");
  }
  int x = 0, y;
  int color = 0x003486fa;
  int *share_addr = NULL;
  share_addr = mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, lcd_fd, 0);
  if (share_addr == MAP_FAILED)
  {
    perror("mmap failed\n");
  }
  pos_t = (int)ceil(800 / (double)atoi(time) * atoi(pos));
  for (x = 0; x <= pos_t; x++)
  {
    for (y = 0; y < 5; y++)
      memcpy(share_addr + x + (y + 435) * 800, &color, 4);
  }
  munmap(share_addr, 800 * 480 * 4);
  close(lcd_fd);

  while (1)
  {
    get_xy(p);
    // 退出全屏
    if (is_key_area(p, 726, 442, 800, 480))
    {
      //创建任务参数
      struct arg *task_arg3 = (struct arg *)malloc(sizeof(struct arg));
      task_arg3->arg1 = shmid;
      task_arg3->ptr[0] = (void *)rwlock;
      strcpy(task_arg3->arg2, "-1");
      //发送播放结束
      pthread_t thread4;
      //创建线程
      pthread_create(&thread4, NULL, video_return_write, (void *)task_arg3);
      //回收子线程
      pthread_join(thread4, NULL);

      printf("0000000000000000000\n");
      sleep(1);
      system("killall -9 mplayer");
      loading_video_interface(video_head, pages, n);
      break;
    }

    //从 暂停---->播放
    if (is_key_area(p, 36, 442, 84, 480))
    {
      strcpy(pause, "1");
      //创建任务参数
      struct arg *task_arg = (struct arg *)malloc(sizeof(struct arg));
      task_arg->arg1 = shmid;
      task_arg->ptr[0] = (void *)rwlock;
      task_arg->arg4[2] = pause;

      pthread_t thread;
      //创建线程
      pthread_create(&thread, NULL, pause_write_play_flag, (void *)task_arg);
      //回收子线程
      pthread_join(thread, NULL);
      break;
    }
  }
  printf("log-xmrs: 全屏控制结束 退出全屏\n");
}

/**
 * 返回主页
*/
void video_return_home(video_list_p video_head, int shmid, int msgid, pthread_rwlock_t *rwlock)
{
  //创建任务参数
  struct arg *task_arg3 = (struct arg *)malloc(sizeof(struct arg));
  task_arg3->arg1 = shmid;
  task_arg3->ptr[0] = (void *)rwlock;
  strcpy(task_arg3->arg2, "-1");
  //发送播放结束
  pthread_t thread4;
  //创建线程
  pthread_create(&thread4, NULL, video_return_write, (void *)task_arg3);
  //回收子线程
  pthread_join(thread4, NULL);
  //销毁读写锁
  pthread_rwlock_destroy(rwlock);
  //销毁链表
  destroy_video_list(video_head);
  sleep(1);
  // 删除消息队列
  if (msgctl(msgid, IPC_RMID, 0) == -1)
    fprintf(stderr, "msgctl(IPC_RMID) failed\n");
  // 删除共享内存
  if (shmctl(shmid, IPC_RMID, NULL) == -1)
    fprintf(stderr, "shmctl(IPC_RMID) failed\n");

  printf("video_return_home 结束\n");
}
/**
 * 视频管理
*/
void video_control(int *No, int tail, video_list_p video_head, thread_pool *pool, xy *p)
{
  int n = 1;               //视频序号
  int page = 1;            //页码
  bool flag = false;       //暂停标志位
  pthread_rwlock_t rwlock; //定义读写锁对象
  bool full = true;        //全屏
  char buf[100];
  pid_t play_pid;
  pid_t bar_pid;
  char pos[4];
  char pause[1] = {"1"};
  //初始化读写锁
  pthread_rwlock_init(&rwlock, NULL);
  int shmid;
  //申请键值
  key_t key1 = ftok("/", 20);
  key_t key2 = ftok("/", 30);
  //申请共享内存
  shmid = shmget(key1, 2048, IPC_CREAT | IPC_EXCL | 0777);
  if (shmid == -1 && errno == EEXIST)
  {
    shmid = shmget(key1, 2048, 0777);
    sprintf(buf, "ipcrm -m %d", shmid);
    system(buf);
    shmid = shmget(key1, 2048, IPC_CREAT | IPC_EXCL | 0777);
  }
  else if (shmid == -1)
  {
    perror("shmid error");
  }

  //3.申请消息队列
  int msgid = msgget(key2, IPC_CREAT | IPC_EXCL | 0777);
  if (msgid == -1 && errno == EEXIST) //EEXIST 文件已经存在
  {
    msgid = msgget(key2, 0777);
    bzero(buf, sizeof(buf));
    sprintf(buf, "ipcrm -q %d", msgid);
    system(buf);
    msgid = msgget(key2, IPC_CREAT | IPC_EXCL | 0777);
  }
  else if (msgid == -1)
  {
    perror("msgget error\n");
  }

  while (1)
  {
    get_xy(p);
    /*视频播放区域 播放按钮 未全屏*/
    if (is_key_area(p, 0, 0, 554, 420))
    {
      printf("/*视频播放区域 播放按钮 未全屏*/\n");
      // pid_t mplayer_id = get_pid("mplayer");
      // if (mplayer_id == 0)
      // {
      //   // 播放
      //   play_pid = fork();
      //   if (play_pid == 0)
      //   {
      //     vdieo_play(shmid, msgid, &rwlock, find_video(video_head, &n), pos);
      //   }
      //   bar_pid = fork();
      //   if (bar_pid == 0)
      //   {
      //     video_progress_bar(shmid, &rwlock);
      //   }
      // }
    }
    /*播放按钮*/
    else if (is_key_area(p, 0, 440, 67, 480))
    {
      pid_t mplayer_id = get_pid("mplayer");
      //暂停
      if (mplayer_id != 0)
      {
        voide_pause(&flag, shmid, msgid, &rwlock);
      }
      else
      { // 播放
        play_pid = fork();
        if (play_pid == 0)
        {
          vdieo_play(shmid, msgid, &rwlock, find_video(video_head, &n), pos);
        }
        bar_pid = fork();
        if (bar_pid == 0)
        {
          video_progress_bar(shmid, &rwlock);
        }
      }
    }
    /*退出按钮*/
    else if (is_key_area(p, 236, 440, 285, 480))
    {
      break;
    }
    /*静音按钮*/
    else if (is_key_area(p, 310, 440, 352, 480))
    {
    }
    /*全屏控制按钮*/
    else if (is_key_area(p, 508, 440, 545, 480))
    {

      pid_t pid_full_screen = fork();
      if (pid_full_screen == 0)
      {
        full_screen(shmid, msgid, &rwlock, find_video(video_head, &n), p);
        //full = false;
      }
      /*全屏控制*/
      if (pid_full_screen > 0)
      {
        get_xy(p);

        //创建任务参数
        struct arg *task_arg31 = (struct arg *)malloc(sizeof(struct arg));
        task_arg31->arg1 = shmid;
        task_arg31->ptr[0] = (void *)&rwlock;
        strcpy(task_arg31->arg2, "-1");
        //发送播放结束
        pthread_t thread4;
        //创建线程
        pthread_create(&thread4, NULL, video_return_write, (void *)task_arg31);
        //回收子线程
        pthread_join(thread4, NULL);

        printf("0000000000000000000\n");
        sleep(1);
        system("killall -9 mplayer");
        loading_video_interface(video_head, page, n);

        sleep(1);
        //创建任务参数 发送开始播放 “1”
        struct arg *task_arg2 = (struct arg *)malloc(sizeof(struct arg));
        task_arg2->arg1 = shmid;
        task_arg2->ptr[0] = (void *)&rwlock;
        task_arg2->arg4[2] = pause;

        pthread_t thread;
        //创建线程
        pthread_create(&thread, NULL, pause_write_play_flag, (void *)task_arg2);
        //回收子线程
        pthread_join(thread, NULL);
        // while (1)
        // {
        //   printf("333333333333333333333\n");
        //   get_xy(p);
        //   //点击屏幕
        //   if (is_key_area(p, 0, 0, 800, 480))
        //   {
        //     full_screen_control(shmid, &rwlock, video_head, page, n, p);
        //   }
        //   if (!full)
        //     break;
        //   printf("------------------\n");
        // }
        // full = true;
        // printf("===================\n");
      }
    }
    // /*声音调节*/
    // else if (is_key_area(p,))
    // {
    // }
    /*播放列表选择框*/
    else if (is_key_area(p, 555, 42, 800, 480))
    {
      video_list_handle(No, tail, p);
      n = *No;
    }
  }
  video_return_home(video_head, shmid, msgid, &rwlock);
  wait(NULL);
  wait(NULL);
  printf("视频管理 结束\n");
}

/**
 * 视频界面
*/
void video_interface(video_list_p video_head, xy *p)
{
  int max;
  int tail = 0; // 当前页总共个数
  int No = 0;   // 当前页的第n个视频
  // 线程池初始化
  thread_pool *pool = (thread_pool *)malloc(sizeof(thread_pool));
  //init_pool(pool, 3);

  max = init_video_interface(video_head, &tail, 1);
  video_control(&No, tail, video_head, pool, p);
  // while (1)
  // {
  //   get_xy(p);
  //   printf("x =%d \n", p->x);
  //   printf("y =%d \n", p->y);
  // }
  exit(EXIT_SUCCESS);
}