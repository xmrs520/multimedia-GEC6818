#include "../inc/main.h"

/**
 * 初始化音乐列表
*/
music_list_p music_list_init()
{
  music_list_p head = (music_list_p)malloc(sizeof(music_list));
  if (head != NULL)
  {
    strcpy(head->song_title, "未知");
    strcpy(head->singer, "未知");
    strcpy(head->path, "未知");
    head->index = 0;
    head->page = 0;
    head->next = head;
    head->prev = head;
    return head;
  }
  perror("allocate failed");
  return NULL;
}

/**
 * 创建音乐列表
*/
bool create_music_list(music_list_p head, char *song, char *singer, int index, int pages, char *path)
{
  music_list_p p = head;
  music_list_p node = (music_list_p)malloc(sizeof(music_list));

  if (node != NULL)
  {
    strcpy(node->song_title, song);
    strcpy(node->singer, singer);
    strcpy(node->path, path);
    node->index = index;
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
  perror("allocate failed");
  return false;
}

/**
 * 销毁音乐列表
*/
void destroy_music_list(music_list_p head)
{
  music_list_p p = head;
  music_list_p q = p->next;
  while (p->next != head)
  {
    free(p);
    p = q;
    q = q->next;
  }
  free(q);
}

/**
 * 打印歌曲列表
*/
void display_music_list(music_list_p head)
{
  printf("---------初始化音乐列表---------\n");
  music_list_p p = head->next;
  while (p != head)
  {
    printf("song = %s \n", p->song_title);
    printf("singer = %s \n", p->singer);
    printf("index = %d \n", p->index);
    printf("path = %s \n", p->path);
    p = p->next;
  }
}

/**
 * 音乐初始化
*/
int music_init(music_list_p head)
{
  char path[100];
  bzero(path, sizeof(path));
  read_pictrue_path("music_login", path);
  display_picture(path, 0, 0, false);
  bzero(path, sizeof(path));
  read_pictrue_path("music_bg", path);
  sleep(1);
  display_picture(path, 0, 0, false);
  bzero(path, sizeof(path));
  read_pictrue_path("head", path);
  display_picture(path, 97, 60, false);
  bzero(path, sizeof(path));
  read_pictrue_path("bofang", path);
  display_picture(path, 265, 125, false);
  bzero(path, sizeof(path));
  read_pictrue_path("shengyin", path);
  display_picture(path, 78, 156, false);

  //初始化音乐列表
  int max = read_music_list(head);
  show_music_list(1, head);
  music_list_p p = head->next;
  font_tools(p->song_title, 200, 30, 26, 0x00000000, 175, 22, 0x00ffe7e7);
  font_tools(p->singer, 200, 55, 22, 0x00b3b1c9, 175, 22, 0x00ffe7e7);
  //命令有名管道
  make_fifo();
  return max;
  //display_music_list(head);
}

/**
 * 界面刷新
*/
void refresh(music_list_p list)
{
  char path[100];
  bzero(path, sizeof(path));
  read_pictrue_path("music_bg", path);
  display_picture(path, 0, 0, false);
  bzero(path, sizeof(path));
  read_pictrue_path("head", path);
  display_picture(path, 97, 60, false);
  bzero(path, sizeof(path));
  read_pictrue_path("bofang", path);
  display_picture(path, 265, 125, false);
  bzero(path, sizeof(path));
  read_pictrue_path("shengyin", path);
  display_picture(path, 78, 156, false);
  music_list_p p = list;
  music_list_p phead = p;
  font_tools(p->song_title, 200, 30, 26, 0x00000000, 175, 22, 0x00ffe7e7);
  font_tools(p->singer, 200, 55, 22, 0x00b3b1c9, 175, 22, 0x00ffe7e7);

  char buf[256];
  char name[100];
  char head_img[256];
  //获取当前绝对路径
  getcwd(buf, sizeof(buf));
  //文件路径
  sscanf(strstr(p->path, "music/"), "%*[^/]/%[^.].", name);
  sprintf(head_img, "%s/img/%s.bmp", buf, name);
  if (access(head_img, F_OK) == 0)
    display_picture(head_img, 97, 60, false);
  while (1)
  {
    if (phead->page == 0)
      break;
    phead = phead->next;
  }
  show_music_list(p->page, phead);
}

/**
 * 进度条处理
*/
void handle_bar(int m)
{
  int lcd_fd = open(LCD_PATH, O_RDWR);
  if (lcd_fd < 0)
  {
    perror("Failed to open lcd\n");
  }
  int x = 0, y;
  int color = 0x00ff0066;
  int *share_addr = NULL;
  share_addr = mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, lcd_fd, 0);
  if (share_addr == MAP_FAILED)
  {
    perror("mmap failed\n");
  }
  //printf("handle_bar ---m--- %d \n", m);
  for (x = 200; x <= 200 + m; x++)
  {
    for (y = 0; y < 8; y++)
      memcpy(share_addr + x + (y + 79) * 800, &color, 4);
  }
  munmap(share_addr, 800 * 480 * 4);
  close(lcd_fd);
}

/**
 * 音乐播放进度条
 * flag : 1 开始 2 暂停 -1 结束
 * msgtpy : 100 150
*/
void *music_progress_bar(void *arg)
{
  int percent = 0;
  int play_flag = 1;
  double tmp = 0.0;
  int shake = 1; //防垃圾数据干扰
  int m = 0, s = 0, second = 0;
  char time_str[10];
  //接收参数
  struct arg *args = (struct arg *)arg;
  int msgid = args->arg1;

  //接收播放进度
  struct msgbuf recvpos_150;

  //recvpos_15.pos = -1;
  //接收播放状态
  struct msgbuf recvflag;

  while (1)
  {

    //接收播放进度
    bzero(recvpos_150.mtext, sizeof(recvpos_150.mtext));
    if (msgrcv(msgid, &recvpos_150, sizeof(recvpos_150), 150, IPC_NOWAIT) == -1) // 150号邮件 进度
      perror("recvpos_150 接收消息 失败");

    tmp = 0.0; //置零
    tmp = (double)(atoi(recvpos_150.mtext));
    percent = floor(170 * tmp / 100);
    if (percent == 166)
      continue;

    //接收播放状态
    if (msgrcv(msgid, &recvflag, sizeof(recvflag), 100, IPC_NOWAIT) == -1)
      perror("recvflag 100号 接收消息 失败");
    if (recvflag.flag != 0)
      play_flag = recvflag.flag;

    printf("recvflag.flag = %d \n", recvflag.flag);
    printf("percent = %d  \n", percent);

    handle_bar(percent); //打印播放进度
    sleep(1);
    /*开始播放*/
    if (play_flag == 1)
    {
      bzero(time_str, sizeof(time_str));
      time_format(second, time_str, false);
      fonts(time_str, 200, 87, 1, 0xf26c91, 200, 87, 45, 11, 0xffe7e7);
      second++;
      printf("开始 \n");
    }
    /*暂停*/
    if (play_flag == 2)
      printf("暂停\n");
    /*播放结束*/
    if (percent >= 164 || play_flag == -1)
      break;
  }
  //打印播放图片
  char path[100];
  bzero(path, sizeof(path));
  read_pictrue_path("bofang", path);
  display_picture(path, 265, 125, false);
  printf("log-xmrs: 进度条结束\n");
}

/**
 * 打印歌单
*/
void show_music_list(int pages, music_list_p head)
{

  music_list_p p = head->next;
  int offset = 0;

  while (p != head)
  {
    if (p->page == pages)
    {
      font_tools(p->song_title, 80, 268 + offset, 30, 0xec4141, 250, 30, 0x00ffffff);
      offset += 30;
    }

    p = p->next;
  }
}

/**
 * 寻找列表中第 n 首歌
*/
music_list_p find(int *n, music_list_p head)
{
  music_list_p p = head->next;
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
 * 创建管道
*/
bool make_fifo()
{
  // //判断文件是否存在
  // if (access("/tmp/fifo", F_OK) == 0)
  // {
  //   unlink("/tmp/fifo");
  // }
  // else
  // {
  //   //创建管道文件，在开发板/tmp下面创建一个fifo的管道文件
  //   int ret = mkfifo("/tmp/fifo", 0777);
  //   if (ret < 0)
  //   {
  //     perror("mkfifo /tmp/fifo false!");
  //     return false;
  //   }
  // }
  unlink("/tmp/fifo");
  //创建管道文件，在开发板 / tmp下面创建一个fifo的管道文件
  int ret = mkfifo("/tmp/fifo", 0777);
  if (ret < 0)
  {
    perror("mkfifo /tmp/fifo false!");
    return false;
  }
  return true;
}

/**
 * 打印歌词
*/
void *show_lyric(void *arg)
{
  int i, j;
  int n = 0;
  int second = 0;
  char lrc[200];
  char buf[200];
  int m, s;
  char mm[4];
  char ss[4];
  int time = 0;
  char *ret;
  int play_flag = 1;
  bool refresh = false;
  bool emtpy = false;
  bool first = true;
  int offset_q2 = 0;
  int offset_q1 = 0;
  link_queue_p q1 = queue_init();
  link_queue_p q2 = queue_init();

  //接收参数
  struct arg *args = (struct arg *)arg;
  int msgid = args->arg1;
  char path[200];
  bzero(path, sizeof(path));
  strcpy(path, args->arg2);

  // 接收播放状态  20 号
  struct msgbuf recvflag;
  //msgrcv(msgid, &recvflag, sizeof(recvflag), 20, IPC_NOWAIT); // 邮件 20 号
  //printf("接收播放状态  20 号 %d\n", recvflag.flag);

  FILE *fp = NULL;
  fp = read_lyric(path);

  if (fp == NULL)
  {
    font_tools("暂无歌词，请欣赏！", 460, 235, 26, 0x00e84c6d, 300, 30, 0x00ffffff);
    printf("log-xmrs: 没有歌词\n");
  }
  else
  {

    while (1)
    {
      // 接收播放状态  20 号
      msgrcv(msgid, &recvflag, sizeof(recvflag), 20, IPC_NOWAIT); // 邮件 20 号
      printf("接收播放状态  20 号 %d\n", recvflag.flag);
      if (recvflag.flag != 0)
        play_flag = recvflag.flag;

      if (play_flag == 1) //播放
      {
        if (refresh || first)
        {
          //q1 读取数据
          fseek(fp, 0, SEEK_SET);
          for (i = 0; i < n + 5; i++)
          {
            bzero(buf, sizeof(buf));
            ret = fgets(buf, sizeof(buf), fp);
            if (i < n + 5 && i >= n)
            {
              bzero(mm, sizeof(mm));
              bzero(ss, sizeof(ss));
              bzero(lrc, sizeof(lrc));
              printf("%s", buf);
              sscanf(buf, "%*1s%[^:]:%[^.].%*3s%*c%[^\n]", mm, ss, lrc);
              m = atoi(mm);
              s = atoi(ss);
              second = m * 60 + s;
              en_queue(q1, second, lrc);
              if (ret == NULL)
                emtpy = true;
            }
            else
              continue;
          }
          //q2读取数据
          fseek(fp, 0, SEEK_SET);
          for (j = 0; j < n; j++)
          {
            bzero(buf, sizeof(buf));
            fgets(buf, sizeof(buf), fp);
            if (n <= 5 && j < n)
            {
              bzero(mm, sizeof(mm));
              bzero(ss, sizeof(ss));
              bzero(lrc, sizeof(lrc));
              printf("%s", buf);
              sscanf(buf, "%*1s%[^:]:%[^.].%*3s%*c%[^\n]", mm, ss, lrc);
              m = atoi(mm);
              s = atoi(ss);
              second = m * 60 + s;
              en_queue(q2, second, lrc);
            }
            else if (n >= 5 && (j < n && j >= n - 5))
            {
              bzero(mm, sizeof(mm));
              bzero(ss, sizeof(ss));
              bzero(lrc, sizeof(lrc));
              sscanf(buf, "%*1s%[^:]:%[^.].%*3s%*c%[^\n]", mm, ss, lrc);
              m = atoi(mm);
              s = atoi(ss);
              second = m * 60 + s;
              en_queue(q2, second, lrc);
            }
          }
        }
        sleep(1);
        time++;

        //初始化
        if (first)
        {
          display_picture("/mnt/sd/Multi-Media-App/img/lrc_bg.bmp", 431, 0, false);
          display_lrc(q1);
          first = false;
        }
        //是否刷新歌词
        if (time == q1->front->time)
        {
          int t;
          offset_q1 = 0;
          offset_q2 = 0;
          bool flag = true;
          display_picture("/mnt/sd/Multi-Media-App/img/lrc_bg.bmp", 431, 0, false);

          // q2旧歌词
          if (!queue_empty(q2))
          {
            data_node_p tmp = q2->rear;
            while (tmp != NULL)
            {
              font_tools(tmp->data, 460, 205 - offset_q2, 26, 0x00afb1b8, 300, 30, 0x00ffffff);
              offset_q2 += 30;
              tmp = tmp->prev;
            }
          }

          //q1新歌词
          data_node_p tmp2 = q1->front;
          while (tmp2 != NULL)
          {
            if (flag)
            {
              font_tools(tmp2->data, 460, 235 + offset_q1, 26, 0x00e84c6d, 300, 30, 0x00ffffff);
              tmp2 = tmp2->next;
              offset_q1 += 30;
              flag = false;
            }
            else
            {
              font_tools(tmp2->data, 460, 235 + offset_q1, 26, 0x00afb1b8, 300, 30, 0x00ffffff);
              tmp2 = tmp2->next;
              offset_q1 += 30;
            }
          }
          //出队刷新
          while (dequeue(q1, &t, buf))
            ;
          while (dequeue(q2, &t, buf))
            ;
          n++;
          refresh = true;
        }
        else
        {
          refresh = false;
        }
      }
      else if (play_flag == 2) //暂停
      {
        continue;
      }
      else if (play_flag == -1) //停止
      {
        break;
      }
      // 歌词显示完毕
      if (emtpy)
        break;
    }

    fclose(fp);
  }

  printf("log-xmrs: 打印歌词结束 \n");
}

/**
 * 播放
*/
void *play(void *arg)
{
  char play_music[256];
  FILE *mp;
  char buf[200];
  char tmp[5];
  char time_str[10];
  int second = 0;
  int play_flag = 1;
  int play_pos = 0;

  //接收参数
  struct arg *args = (struct arg *)arg;
  music_list_p list = (music_list_p)(args->ptr[0]);
  thread_pool *pools = (thread_pool *)(args->ptr[1]);
  int msgid = args->arg1;
  //msgids = msgid;

  //  刷新
  refresh(list);

  // 进度条
  struct arg *task_arg2 = (struct arg *)malloc(sizeof(struct arg));
  task_arg2->arg1 = msgid;
  add_task(pools, music_progress_bar, (void *)task_arg2);

  //打印歌词
  struct arg *task_arg = (struct arg *)malloc(sizeof(struct arg));
  task_arg->arg1 = msgid;
  strcpy(task_arg->arg2, list->path);
  add_task(pools, show_lyric, (void *)task_arg);

  system("killall -9 mplayer");
  bzero(play_music, sizeof(play_music));
  sprintf(play_music, "mplayer -slave -quiet -input file=/tmp/fifo '%s' &", list->path);
  mp = popen(play_music, "r");
  //system(play_music);

  //打印播放图片
  char path[100];
  bzero(path, sizeof(path));
  read_pictrue_path("zanting", path);
  display_picture(path, 265, 125, false);
  //打开管道文件
  int fifo_fd = open("/tmp/fifo", O_RDWR);
  if (fifo_fd == -1)
  {
    perror("open 管道 error!");
  }

  //获取总时长
  write(fifo_fd, "get_time_length\n", 16);
  do
  {
    bzero(buf, sizeof(buf));
    fgets(buf, sizeof(buf), mp);
    printf("--->|%s", buf);
  } while (strncmp(buf, "ANS_LENGTH=", strlen("ANS_LENGTH=")) != 0);
  bzero(tmp, sizeof(tmp));
  sscanf(buf, "%*[^=]=%s", tmp);
  second = atoi(tmp);
  time_format(second, time_str, false);
  fontt(time_str, 320, 87, 1, 0xf26c91);

  // 接收播放状态
  struct msgbuf recvflag;

  //发送播放进度
  struct msgbuf sendpos;
  sendpos.mtype = 150;

  //sleep(1);
  while (1)
  {
    sleep(1);

    msgrcv(msgid, &recvflag, sizeof(recvflag), 110, IPC_NOWAIT); // 邮件 110 号

    if (recvflag.flag != 0)
      play_flag = recvflag.flag;

    if (play_flag == 2) // 暂停：不发送 mplayer 命令
      continue;
    if (play_flag == 1) // 播放：发送 mplayer 命令
    {
      if (write(fifo_fd, "get_percent_pos\n", 16) != 16)
        perror("write get_percent_pos");

      do
      {
        bzero(buf, sizeof(buf));
        fgets(buf, sizeof(buf), mp);
        printf("--->|%s", buf);
      } while (strncmp(buf, "ANS_PERCENT_POSITION=", strlen("ANS_PERCENT_POSITION=")) != 0);

      bzero(tmp, sizeof(tmp));
      sscanf(buf, "%*[^=]=%s", tmp);
      play_pos = atoi(tmp);
      printf("play_pos %d \n", play_pos);

      bzero(sendpos.mtext, sizeof(sendpos.mtext));
      strcpy(sendpos.mtext, tmp);
      if (msgsnd(msgid, &sendpos, sizeof(sendpos.mtext), 0) != 0) //发送播放进度
        perror("msgsnd 150号 发送播放进度 失败");
    }
    if (play_pos == 98 || play_flag == -1)
      break; // 播放完毕
  }
  pclose(mp);
  close(fifo_fd);
  system("killall -9 mplayer");
  printf("log-xmrs: play end \n");
}

/**
 * 暂停
*/
void *suspend(void *arg)
{

  //接收参数
  struct arg *args = (struct arg *)arg;
  bool *flag = (bool *)(args->ptr[0]);
  int msgid = args->arg1;

  //打开管道文件
  int fifo_fd = open("/tmp/fifo", O_RDWR);
  if (fifo_fd == -1)
  {
    perror("open 管道 error!");
  }
  //从 播放---->暂停
  if (!(*flag))
  {
    if (write(fifo_fd, "pause\n", 6) == 6)
    {
      *flag = true;
      char path[100];
      bzero(path, sizeof(path));
      read_pictrue_path("bofang", path);
      display_picture(path, 265, 125, false);
      printf("//从 播放---->暂停\n");
      close(fifo_fd);
      //发送播放状态
      struct msgbuf sendflag;
      sendflag.mtype = 110; //邮箱 110 号
      sendflag.flag = 2;    // 暂停标志
      if (msgsnd(msgid, &sendflag, sizeof(sendflag.flag), 0) != 0)
        perror("发送播放状态 110号 失败");

      struct msgbuf sendflag2;
      sendflag2.mtype = 100; //邮箱 100 号
      sendflag2.flag = 2;    // 暂停标志
      if (msgsnd(msgid, &sendflag2, sizeof(sendflag2.flag), 0) != 0)
        perror("发送播放状态 100号 失败");

      struct msgbuf sendflag3;
      sendflag3.mtype = 20; //邮箱 20 号
      sendflag3.flag = 2;   // 暂停标志
      if (msgsnd(msgid, &sendflag3, sizeof(sendflag3.flag), 0) != 0)
        perror("发送播放状态 20号 失败");
    }
    else
    {
      perror("write");
      close(fifo_fd);
    }
  }
  else //从 暂停---->播放
  {
    if (write(fifo_fd, "pause\n", 6) == 6)
    {
      *flag = false;
      char path[100];
      bzero(path, sizeof(path));
      read_pictrue_path("zanting", path);
      display_picture(path, 265, 125, false);
      printf("//从 暂停---->播放\n");

      close(fifo_fd);
      //发送播放状态
      struct msgbuf sendflag;
      sendflag.mtype = 110; //邮箱 110 号
      sendflag.flag = 1;    // 暂停标志 1 播放
      if (msgsnd(msgid, &sendflag, sizeof(sendflag.flag), 0) != 0)
        perror("发送播放状态 110号 失败 1");

      struct msgbuf sendflag2;
      sendflag2.mtype = 100; //邮箱 100 号
      sendflag2.flag = 1;    // 暂停标志 1 播放
      if (msgsnd(msgid, &sendflag2, sizeof(sendflag2.flag), 0) != 0)
        perror("发送播放状态 100号 失败 1");

      struct msgbuf sendflag3;
      sendflag3.mtype = 20; //邮箱 20 号
      sendflag3.flag = 1;   // 暂停标志
      if (msgsnd(msgid, &sendflag3, sizeof(sendflag3.flag), 0) != 0)
        perror("发送播放状态 20号 失败 1");
    }
    else
    {
      perror("write");
      close(fifo_fd);
    }
  }
  printf("suspend ok  \n");
}

/**
 * 声音
*/
bool volume(int opt, int *value)
{
  char buf[4];
  //打开管道文件
  int fifo_fd = open("/tmp/fifo", O_RDWR);
  if (fifo_fd == -1)
  {
    perror("open 管道 error!");
  }
  if (opt > 0) // + 声音
  {
    if (*value < 100)
      *value += 4;
    if (*value > 95)
      return true;
    bzero(buf, sizeof(buf));
    sprintf(buf, "%d%%", *value);
    if (write(fifo_fd, "volume +5\n", 10) == 10)
    {
      char path[100];
      bzero(path, sizeof(path));
      read_pictrue_path("shengyin", path);
      display_picture(path, 78, 156, false);
      close(fifo_fd);
      fonts(buf, 50, 170, 1, 0x21252b, 50, 170, 30, 11, 0xffffff);
    }
    else
    {
      perror("write");
      close(fifo_fd);
    }
  }
  else if (opt < 0) // - 声音
  {
    if (*value > 0)
      *value -= 2;
    if (*value < 0)
    {
      char path[100];
      bzero(path, sizeof(path));
      read_pictrue_path("jingyin", path);
      display_picture(path, 78, 156, false);
      return true;
    }
    bzero(buf, sizeof(buf));
    sprintf(buf, "%d%%", *value);
    if (write(fifo_fd, "volume -5\n", 10) == 10)
    {
      close(fifo_fd);
      fonts(buf, 50, 170, 1, 0x21252b, 50, 170, 30, 11, 0xffffff);
    }
    else
    {
      perror("write");
      close(fifo_fd);
    }
  }
}

/**
 * 通知其他线程，退出
*/
void *tell_pthread(void *arg)
{
  //接收参数
  struct arg *args = (struct arg *)arg;
  music_list_p list = (music_list_p)(args->ptr[0]);
  int msgid = args->arg1;

  //发送退出播放状态
  struct msgbuf sendflag_100;
  sendflag_100.mtype = 100; //邮箱 100 号
  sendflag_100.flag = -1;   // 暂停标志 -1 结束
  msgsnd(msgid, &sendflag_100, sizeof(sendflag_100.flag), 0);

  struct msgbuf sendflag_110;
  sendflag_110.mtype = 110; //邮箱 110 号
  sendflag_110.flag = -1;   // 暂停标志 -1 结束
  msgsnd(msgid, &sendflag_110, sizeof(sendflag_110.flag), 0);

  struct msgbuf sendflag_20;
  sendflag_20.mtype = 20; //邮箱 20 号
  sendflag_20.flag = -1;  // 暂停标志 -1 结束
  msgsnd(msgid, &sendflag_20, sizeof(sendflag_20.flag), 0);
}

/**
 * 返回主页
*/
void return_home(music_list_p head, int msgid, thread_pool *pool, xy *p)
{
  system("killall -9 mplayer");
  usleep(500);
  destroy_pool(pool);
  // 删除消息队列
  if (msgctl(msgid, IPC_RMID, 0) == -1)
    fprintf(stderr, "msgctl(IPC_RMID) failed\n");

  destroy_music_list(head);
  exit(EXIT_SUCCESS);
  //home(head, p);
}

/**
 * 音乐控制
*/
void music_control(int max, music_list_p head, thread_pool *pool, xy *p)
{
  int n = 1;      //默认列表中第1首
  int tail = max; //共计歌曲
  int volume_value = 90;
  bool flag = false; //暂停标志位
  bool first = true; //首次加载
  //2.获取系统键值
  key_t key = ftok("/", 20);

  //3.申请消息队列
  int msgid = msgget(key, IPC_CREAT | IPC_EXCL | 0777);
  if (msgid == -1 && errno == EEXIST) //EEXIST 文件已经存在
  {
    msgid = msgget(key, 0777);
  }
  else if (msgid == -1)
  {
    perror("msgget error\n");
  }
  printf("-------------msgid%d\n", msgid);

  while (1)
  {
    get_xy(p);
    if (is_key_area(p, 273, 118, 321, 164) && first) //播放
    {
      //任务1参数
      struct arg *task_arg = (struct arg *)malloc(sizeof(struct arg));
      task_arg->ptr[0] = (void *)find(&n, head);
      task_arg->ptr[1] = (void *)pool;
      task_arg->arg1 = msgid;
      add_task(pool, play, (void *)task_arg);

      first = false;
      printf("log-xmrs: 首次播放\n");
    }
    else if (is_key_area(p, 273, 118, 321, 164)) //暂停
    {
      /* 上一首是否播放完毕 */
      pid_t pid = get_pid("mplayer");
      if (pid != 0)
      {
        /* 正在播放 */
        //任务参数
        struct arg *task_arg = (struct arg *)malloc(sizeof(struct arg));
        task_arg->ptr[0] = (void *)(&flag);
        task_arg->arg1 = msgid;
        add_task(pool, suspend, (void *)task_arg);
        printf("log-xmrs: 暂停播放\n");
        printf("log-xmrs: mplayer pid = %d \n", pid);
      }
      else
      {
        //从新播放一遍
        //任务1参数
        // struct arg *task_arg = (struct arg *)malloc(sizeof(struct arg));
        // task_arg->ptr[0] = (void *)find(&n, head);
        // task_arg->arg1 = msgid;
        // add_task(pool, play, (void *)task_arg);

        struct msgbuf sendflag_110;
        sendflag_110.mtype = 110; //邮箱 110 号
        sendflag_110.flag = -1;   // 暂停标志 -1 结束
        msgsnd(msgid, &sendflag_110, sizeof(sendflag_110.flag), 0);

        //发送歌词
        struct msgbuf sendflag_20;
        sendflag_20.mtype = 20; //邮箱 20 号
        sendflag_20.flag = -1;  // 暂停标志 -1 结束
        msgsnd(msgid, &sendflag_20, sizeof(sendflag_20.flag), 0);
        // sendflag_20.flag = 1; // 暂停标志 1 结束
        // msgsnd(msgid, &sendflag_20, sizeof(sendflag_20.flag), 0);
        //发送进度条
        struct msgbuf sendflag_100;
        sendflag_100.mtype = 100; //邮箱 100 号
        sendflag_100.flag = -1;   // 暂停标志 -1 结束
        msgsnd(msgid, &sendflag_100, sizeof(sendflag_100.flag), 0);

        sleep(2);

        struct arg *task_arg = (struct arg *)malloc(sizeof(struct arg));
        task_arg->ptr[0] = (void *)find(&n, head);
        task_arg->ptr[1] = (void *)pool;
        task_arg->arg1 = msgid;
        add_task(pool, play, (void *)task_arg);
        printf("log-xmrs: 再听一遍\n");
      }
    }
    else if (is_key_area(p, 338, 119, 384, 164)) //下一首
    {
      if (n >= max)
        n = 1;
      else
        n++;

      first = false;
      struct msgbuf sendflag_110;
      sendflag_110.mtype = 110; //邮箱 110 号
      sendflag_110.flag = -1;   // 暂停标志 -1 结束
      msgsnd(msgid, &sendflag_110, sizeof(sendflag_110.flag), 0);

      //发送歌词
      struct msgbuf sendflag_20;
      sendflag_20.mtype = 20; //邮箱 20 号
      sendflag_20.flag = -1;  // 暂停标志 -1 结束
      msgsnd(msgid, &sendflag_20, sizeof(sendflag_20.flag), 0);
      // sendflag_20.flag = 1; // 暂停标志 1 结束
      // msgsnd(msgid, &sendflag_20, sizeof(sendflag_20.flag), 0);
      //发送进度条
      struct msgbuf sendflag_100;
      sendflag_100.mtype = 100; //邮箱 100 号
      sendflag_100.flag = -1;   // 暂停标志 -1 结束
      msgsnd(msgid, &sendflag_100, sizeof(sendflag_100.flag), 0);
      // sendflag_100.flag = 1; // 暂停标志 1 结束
      // msgsnd(msgid, &sendflag_100, sizeof(sendflag_100.flag), 0);

      // //任务参数
      // struct arg *task_arg2 = (struct arg *)malloc(sizeof(struct arg));
      // task_arg2->arg1 = msgid;
      // add_task(pool, tell_pthread, (void *)task_arg2);

      sleep(2);

      //任务1参数
      struct arg *task_arg = (struct arg *)malloc(sizeof(struct arg));
      task_arg->ptr[0] = (void *)find(&n, head);
      task_arg->ptr[1] = (void *)pool;
      task_arg->arg1 = msgid;
      add_task(pool, play, (void *)task_arg);

      printf("log-xmrs: 下一首 \n");
    }
    else if (is_key_area(p, 203, 122, 240, 164)) //上一首
    {
      if (n <= 1)
        n = max;
      else
        n--;
      first = false;
      struct msgbuf sendflag_110;
      sendflag_110.mtype = 110; //邮箱 110 号
      sendflag_110.flag = -1;   // 暂停标志 -1 结束
      msgsnd(msgid, &sendflag_110, sizeof(sendflag_110.flag), 0);

      //发送歌词
      struct msgbuf sendflag_20;
      sendflag_20.mtype = 20; //邮箱 20 号
      sendflag_20.flag = -1;  // 暂停标志 -1 结束
      msgsnd(msgid, &sendflag_20, sizeof(sendflag_20.flag), 0);
      // sendflag_20.flag = 1; // 暂停标志 1 结束
      // msgsnd(msgid, &sendflag_20, sizeof(sendflag_20.flag), 0);
      //发送进度条
      struct msgbuf sendflag_100;
      sendflag_100.mtype = 100; //邮箱 100 号
      sendflag_100.flag = -1;   // 暂停标志 -1 结束
      msgsnd(msgid, &sendflag_100, sizeof(sendflag_100.flag), 0);
      // sendflag_100.flag = 1; // 暂停标志 1 结束
      // msgsnd(msgid, &sendflag_100, sizeof(sendflag_100.flag), 0);

      // //任务参数
      // struct arg *task_arg2 = (struct arg *)malloc(sizeof(struct arg));
      // task_arg2->arg1 = msgid;
      // add_task(pool, tell_pthread, (void *)task_arg2);

      sleep(2);

      //任务1参数
      struct arg *task_arg = (struct arg *)malloc(sizeof(struct arg));
      task_arg->ptr[0] = (void *)find(&n, head);
      task_arg->ptr[1] = (void *)pool;
      task_arg->arg1 = msgid;
      add_task(pool, play, (void *)task_arg);

      printf("log-xmrs: 上一首 \n");
    }
    else if (is_key_area(p, 140, 164, 200, 203)) //声音 +
    {
      volume(1, &volume_value);
      printf("log-xmrs: 声音 + \n");
    }
    else if (is_key_area(p, 120, 164, 140, 203)) //声音 -
    {
      volume(-1, &volume_value);
      printf("log-xmrs: 声音 - \n");
    }
    else if (is_key_area(p, 5, 5, 64, 64)) //返回主页
    {
      //任务参数
      struct arg *task_arg = (struct arg *)malloc(sizeof(struct arg));
      task_arg->arg1 = msgid;
      add_task(pool, tell_pthread, (void *)task_arg);
      break;
    }
  }
  return_home(head, msgid, pool, p);
}

/**
 * 音乐
*/
void music(music_list_p head, xy *p)
{
  // 线程池初始化
  thread_pool *pool = (thread_pool *)malloc(sizeof(thread_pool));
  init_pool(pool, 10);

  int max = music_init(head);

  display_music_list(head);
  // while (1)
  // {
  //   get_xy(p);
  //   printf("x=%d y=%d\n", p->x, p->y);
  // }

  //从无名管道的写端读取信息进行广播

  music_control(max, head, pool, p);
}