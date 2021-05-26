#include "../inc/main.h"

/**
 * 棋子链表初始化 
*/
chess_info_p chess_link_init()
{
  chess_info_p head = (chess_info_p)malloc(sizeof(chess_info));
  if (head == NULL)
  {
    perror("allocate failed");
    return NULL;
  }
  head->id = -1;
  head->x = -1;
  head->y = -1;
  head->level = -1;
  head->role = -1;
  head->show = -1;
  head->selected = -1;
  strcpy(head->path, "NULL");
  head->next = NULL;
  head->prev = NULL;
  return head;
}

/**
 * 棋子链表销毁
*/
int destroy_chess_pieces_info_link(chess_info_p head)
{
  if (head == NULL || head->next == NULL)
  {
    printf("log-xmrs: 棋子链表销毁 空链表\n");
    return -1;
  }
  chess_info_p p = head->next;
  chess_info_p q = p->next;
  while (q != NULL)
  {
    free(p);
    p = q;
    q = p->next;
  }
  free(p);
  free(q);
  return 0;
}

/**
 * 棋子链表创建
*/
bool create_chess_pieces_info_link(chess_info_p head)
{
  int i, j;
  chess_info_p p = head;
  char path[256];
  i = -1;

  for (j = 0; j < 36; j++)
  {
    chess_info_p node = (chess_info_p)malloc(sizeof(chess_info));
    if (node == NULL)
    {
      perror("allocate failed");
      return false;
    }
    //红方
    if (j < 18)
    {
      node->id = j + 1;
      if (j < 8)
      {
        bzero(path, sizeof(path));
        read_pictrue_path("assassin_red", path);
        strcpy(node->path, path);
        node->level = 1;
      }
      if (j >= 8 && j < 12)
      {
        bzero(path, sizeof(path));
        read_pictrue_path("lifeguard_red", path);
        strcpy(node->path, path);
        node->level = 2;
      }
      if (j >= 12 && j < 14)
      {
        bzero(path, sizeof(path));
        read_pictrue_path("archer_red", path);
        strcpy(node->path, path);
        node->level = 3;
      }
      if (j >= 14 && j < 16)
      {
        bzero(path, sizeof(path));
        read_pictrue_path("knight_red", path);
        strcpy(node->path, path);
        node->level = 4;
      }
      if (j == 16)
      {
        bzero(path, sizeof(path));
        read_pictrue_path("general_red", path);
        strcpy(node->path, path);
        node->level = 5;
      }
      if (j == 17)
      {
        bzero(path, sizeof(path));
        read_pictrue_path("king_red", path);
        strcpy(node->path, path);
        node->level = 6;
      }
      node->role = 0;
      node->show = 0;
      node->selected = 0;

      p->next = node;
      node->prev = p;
      node->next = NULL;
    }
    //蓝方
    if (j >= 18)
    {
      node->id = j + 1;
      if (j < 26)
      {
        bzero(path, sizeof(path));
        read_pictrue_path("assassin_blue", path);
        strcpy(node->path, path);
        node->level = 1;
      }
      if (j >= 26 && j < 30)
      {
        bzero(path, sizeof(path));
        read_pictrue_path("lifeguard_blue", path);
        strcpy(node->path, path);
        node->level = 2;
      }
      if (j >= 30 && j < 32)
      {
        bzero(path, sizeof(path));
        read_pictrue_path("archer_blue", path);
        strcpy(node->path, path);
        node->level = 3;
      }
      if (j >= 32 && j < 34)
      {
        bzero(path, sizeof(path));
        read_pictrue_path("knight_blue", path);
        strcpy(node->path, path);
        node->level = 4;
      }
      if (j == 34)
      {
        bzero(path, sizeof(path));
        read_pictrue_path("general_blue", path);
        strcpy(node->path, path);
        node->level = 5;
      }
      if (j == 35)
      {
        bzero(path, sizeof(path));
        read_pictrue_path("king_blue", path);
        strcpy(node->path, path);
        node->level = 6;
      }
      node->role = 1;
      node->show = 0;
      node->selected = 0;

      p->next = node;
      node->prev = p;
      node->next = NULL;
    }
    // if (j % 6 == 0)
    //   i++;
    // node->y = j % 6; //行
    // node->x = i;     //列
    p = p->next;
  }
}

/**
 * 界面初始化
*/
void game_interface_init()
{
  int i = 0;
  int j = 1;
  int offset_x = 0, offset_y = -75;
  char path[512];
  bool flag = false;
  char qi_green[512];
  char qi_white[512];
  read_pictrue_path("qi_green", qi_green);
  read_pictrue_path("qi_white", qi_white);

  bzero(path, sizeof(path));
  read_pictrue_path("game_bg", path);
  display_picture(path, 0, 0, false);

  for (i = 0; i < 36; i++)
  {
    if (i % 6 == 0)
    {
      offset_x = 0;
      offset_y += 75;
      flag = !flag;
    }
    if (flag)
    {
      display_picture(qi_green, 175 + offset_x, 14 + offset_y, false);
    }
    if (!flag)
    {
      display_picture(qi_white, 175 + offset_x, 14 + offset_y, false);
    }
    flag = !flag;
    offset_x += 75;
  }
}

/**
 * 棋盘初始化
*/
void chessboard_init(chess_info_p (*chessboard)[6], int *datas, chess_info_p head)
{
  int i, j;
  chess_info_p p = NULL;
  j = -1;
  for (i = 0; i < 36; i++)
  {
    p = head->next;
    while (p != NULL)
    {
      if (p->id == datas[i])
        break;
      p = p->next;
    }
    if (i % 6 == 0)
      j++;
    p->x = i % 6; //列
    p->y = j;     //行
    chessboard[j][i % 6] = p;
  }
}

/**
 * 随机数
*/
int rands(int strat, int end)
{
  srand((unsigned)time(NULL) + (unsigned)rand());
  return (rand() % (end - strat + 1) + strat);
}

/**
 * 随机数据
*/
void rand_data(int data[], int n, int strat, int end)
{
  int i, j;
  int num;
  bool flag = false;
  for (i = 0; i < n;)
  {
    num = rands(strat, end);
    //printf("num = %d\n", num);
    for (j = 0; j < i; j++)
    {
      if (data[j] == num)
      {
        flag = true;
        break;
      }
      else
        flag = false;
    }
    if (!flag)
    {
      data[i] = num;
      i++;
    }
    else
      continue;
  }
}

void play_mp3(char *mp3)
{
  char cmd[100];
  system("killname -9 madplay");
  sprintf(cmd, "madplay '%s' &", mp3);
  system(cmd);
}

/**
 * 退出
*/
void game_return(chess_info_p head, int msgid, thread_pool *pool)
{
  destroy_chess_pieces_info_link(head);
  destroy_pool(pool);
  // 删除消息队列
  if (msgctl(msgid, IPC_RMID, 0) == -1)
    fprintf(stderr, "msgctl(IPC_RMID) failed\n");
}

/**
 * 刷新
*/
void chessboard_refresh(int x, int y)
{
  char path[512];
  int flag = 1;
  if (y % 2 == 0)
    flag = 2; // 偶数行
  else
    flag = 1; //奇数行

  if ((flag == 1 && x % 2 == 0) || (flag == 2 && x % 2 != 0))
  {
    bzero(path, sizeof(path));
    read_pictrue_path("white", path);
  }
  else if ((flag == 1 && x % 2 != 0) || (flag == 2 && x % 2 == 0))
  {
    bzero(path, sizeof(path));
    read_pictrue_path("green", path);
  }
  display_picture(path, 175 + x * 75, 14 + y * 75, false);
}

/**
 * 翻子
*/
bool show_chess(int x, int y, chess_info_p (*chessboard)[6], char (*mp3)[100])
{
  if (chessboard[y][x]->show == 0)
  {
    chessboard_refresh(x, y);
    chessboard[y][x]->show = 1;
    display_picture_circle(chessboard[y][x]->path, 175 + x * 75, 14 + y * 75, false);
    sleep(1);
    play_mp3(mp3[chessboard[y][x]->level - 1]);
    return true;
  }
  else
    return false;
}

/**
 * 走子
*/
bool move_chess(int x1, int y1, int role, chess_info_p (*chessboard)[6], char (*mp3)[100], xy *p)
{
  int x, y, x2, y2;
  chess_info_p q1 = chessboard[y1][x1]; //提子
  chess_info_p q2 = NULL;               //落子
  if (q1 != NULL && q1->role == role)
  {
    q1->selected = 1; //提子
    printf("----------提子\n");
    display_picture(q1->path, 175 + x1 * 75, 14 + y1 * 75, false);
    while (1)
    {
      if (!get_xy_plus(p))
        continue;
      if (is_key_area(p, 175, 14, 612, 466))
      {
        x2 = (p->x - 175) / 75; //落子 x1
        y2 = (p->y - 14) / 75;  //落子 y1
        break;
      }
    }
    x = abs(x1 - x2);
    y = abs(y1 - y2);
    printf("------x--y----(%d,%d)\n", x, y);
    printf("------x1-y1---(%d,%d)\n", x1, y1);
    printf("------x2-y2---(%d,%d)\n", x2, y2);
    if (!(x >= 1 && y >= 1)) //合法移棋
    {
      q2 = chessboard[y2][x2];
      if (q2 == NULL) //移子
      {
        printf("----------移子\n");
        q1->selected = 0; //落子
        chessboard_refresh(x1, y1);
        chessboard[y1][x1] = NULL;
        chessboard[y2][x2] = q1;
        display_picture_circle(chessboard[y2][x2]->path, 175 + x2 * 75, 14 + y2 * 75, false);
        play_mp3(mp3[7]);
        return true;
      }
      else if (q2->selected == 1) //取消提子
      {
        printf("----------q1->selected %d\n", q1->selected);
        printf("----------q2->selected %d\n", q2->selected);
        q1->selected = 0;
        printf("----------取消提子\n");
        chessboard_refresh(x1, y1);
        display_picture_circle(q1->path, 175 + x1 * 75, 14 + y1 * 75, false);
        return false;
      }
      else if (q1->role != q2->role && q2->show == 1) //吃子
      {
        if ((q1->level == 1 && q2->level == 6) || (q2->level <= q1->level))
        {
          if (q1->level == 6 && q2->level == 1)
          {
            q1->selected = 0; //取消提子
            printf("----------吃子非法3\n");
            chessboard_refresh(x1, y1);
            display_picture_circle(q1->path, 175 + x1 * 75, 14 + y1 * 75, false);
            return false;
          }
          else
          {
            printf("----------吃子\n");
            q1->selected = 0; //落子
            play_mp3(mp3[7]);
            chessboard_refresh(x1, y1);
            chessboard[y1][x1] = NULL;
            chessboard[y2][x2] = q1;
            display_picture_circle(chessboard[y2][x2]->path, 175 + x2 * 75, 14 + y2 * 75, false);
            return true;
          }
        }
        else
        {
          q1->selected = 0; //取消提子
          printf("----------吃子非法1\n");
          chessboard_refresh(x1, y1);
          display_picture_circle(q1->path, 175 + x1 * 75, 14 + y1 * 75, false);
          return false;
        }
      }
      else
      {
        q1->selected = 0; //取消提子
        printf("----------吃子非法2\n");
        chessboard_refresh(x1, y1);
        display_picture_circle(q1->path, 175 + x1 * 75, 14 + y1 * 75, false);
        return false;
      }
    }
    else
    {
      q1->selected = 0; //取消提子
      printf("----------移棋非法\n");
      chessboard_refresh(x1, y1);
      display_picture_circle(q1->path, 175 + x1 * 75, 14 + y1 * 75, false);
      return false;
    }
  }
  else
  {
    return false;
  }
}

/**
 * 回合
*/
void rounds(int *round)
{
  int i = 0;
  char time_points_black[256];
  char time_points_red[256];
  char time_points_blue[256];
  *round = rands(1, 10);
  read_pictrue_path("time_points_black", time_points_black);
  read_pictrue_path("time_points_red", time_points_red);
  read_pictrue_path("time_points_blue", time_points_blue);
  while (i < 3)
  {
    display_picture(time_points_black, 11, 65, false);
    display_picture(time_points_blue, 727, 65, false);
    usleep(500000);
    display_picture(time_points_red, 11, 65, false);
    display_picture(time_points_black, 727, 65, false);
    usleep(500000);
    i++;
  }
  if (*round % 2 == 1)
  {
    display_picture(time_points_black, 11, 65, false);
    display_picture(time_points_blue, 727, 65, false);
  }
  else
  {
    display_picture(time_points_red, 11, 65, false);
    display_picture(time_points_black, 727, 65, false);
  }
  *round %= 2;
}

/**
 * 回合提示
*/
void rounds_tips(int round)
{
  char time_points_black[256];
  char time_points_red[256];
  char time_points_blue[256];
  read_pictrue_path("time_points_black", time_points_black);
  read_pictrue_path("time_points_red", time_points_red);
  read_pictrue_path("time_points_blue", time_points_blue);
  if (round == 1)
  {
    display_picture(time_points_black, 11, 65, false);
    display_picture(time_points_blue, 727, 65, false);
  }
  else
  {
    display_picture(time_points_red, 11, 65, false);
    display_picture(time_points_black, 727, 65, false);
  }
}

/**
 * 计时 红方
*/
void *red_time(void *arg)
{
  int red_time = 0;
  int flag = 1;
  char buf[20];
  //接收参数
  struct arg *args = (struct arg *)arg;
  int msgid = args->arg1;

  struct msgbuf recv_red_time;
  while (1)
  {
    sleep(1);
    msgrcv(msgid, &recv_red_time, sizeof(recv_red_time), 50, IPC_NOWAIT);

    if (recv_red_time.flag != 0)
      flag = recv_red_time.flag;
    if (flag == 1)
      continue;
    else if (flag == 2)
    {
      red_time++;
      time_format(red_time, buf, false);
      font_tools(buf, 67, 82, 28, 0x333743, 50, 30, 0xf2f2f2);
    }
    else if (recv_red_time.flag == -1)
    {
      break;
    }
  }
  printf("计时 红方 结束\n");
}

/**
 * 计时 蓝方
*/
void *blue_time(void *arg)
{
  int blue_time = 0;
  char buf[20];
  int flag = 1;
  //接收参数
  struct arg *args = (struct arg *)arg;
  int msgid = args->arg1;

  struct msgbuf recv_blue_time;
  while (1)
  {
    sleep(1);
    msgrcv(msgid, &recv_blue_time, sizeof(recv_blue_time), 55, IPC_NOWAIT);

    if (recv_blue_time.flag != 0)
      flag = recv_blue_time.flag;
    if (flag == 1)
      continue;
    else if (flag == 2)
    {
      blue_time++;
      time_format(blue_time, buf, false);
      font_tools(buf, 668, 73, 28, 0x333743, 50, 30, 0xf2f2f2);
    }
    else if (flag == -1)
    {
      break;
    }
  }
  printf("计时 蓝方 结束\n");
}

/**
 * 控制
*/
void games_control(chess_info_p (*chessboard)[6], char (*mp3)[100], int *ids, chess_info_p head, int msgid, thread_pool *pool, xy *p)
{
  play_mp3(mp3[8]);
  sleep(1);
  int red_points = 0, blue_points = 0;
  int x1, y1;
  bool show = false; //是否翻子
  int round = 0;     // 回合 0: 红方 1: 蓝方
  play_mp3(mp3[10]);
  rounds(&round);
  struct arg *task_arg1 = (struct arg *)malloc(sizeof(struct arg));
  task_arg1->arg1 = msgid;
  add_task(pool, red_time, (void *)task_arg1);

  struct arg *task_arg2 = (struct arg *)malloc(sizeof(struct arg));
  task_arg2->arg1 = msgid;
  add_task(pool, blue_time, (void *)task_arg2);

  struct msgbuf sendmsg1; //红方
  sendmsg1.mtype = 50;

  struct msgbuf sendmsg2; //蓝方
  sendmsg2.mtype = 55;

  if (round == 0)
  {
    sendmsg1.flag = 2;
    msgsnd(msgid, &sendmsg1, sizeof(sendmsg1.flag), 0);
  }
  else
  {
    sendmsg2.flag = 2;
    msgsnd(msgid, &sendmsg2, sizeof(sendmsg2.flag), 0);
  }
  pid_t pid = getpid();
  char cmd[50];
  while (1)
  {
    printf("\nlog-xmrs: 一起玩pid = %d\n", pid);
    printf("进程打开了多少文件\n");
    bzero(cmd, sizeof(cmd));
    sprintf(cmd, "lsof -p %d | wc -l", pid);
    system(cmd);
    // bzero(cmd, sizeof(cmd));
    // sprintf(cmd, "lsof -p %d", pid);
    // system(cmd);
    system("ulimit -a");
    if (!get_xy_plus(p))
      continue;
    printf("(%d,%d)\n", p->x, p->y);
    /*走棋*/
    play_mp3(mp3[6]);
    if (is_key_area(p, 175, 14, 612, 466))
    {
      x1 = (p->x - 175) / 75;
      y1 = (p->y - 14) / 75;

      if (chessboard[y1][x1] == NULL)
        continue;
      if (round == 0) //红方
      {
        printf("红方\n");
        //翻子
        show = show_chess(x1, y1, chessboard, mp3);
        if (show)
        {
          round = 1;
          sendmsg1.flag = 1;
          msgsnd(msgid, &sendmsg1, sizeof(sendmsg1.flag), 0);
          sendmsg2.flag = 2;
          msgsnd(msgid, &sendmsg2, sizeof(sendmsg2.flag), 0);
          rounds_tips(round);
          continue;
        }
        //走子
        if (move_chess(x1, y1, 0, chessboard, mp3, p))
          round = 1;
        else
          continue;
        sendmsg1.flag = 1;
        msgsnd(msgid, &sendmsg1, sizeof(sendmsg1.flag), 0);
        sendmsg2.flag = 2;
        msgsnd(msgid, &sendmsg2, sizeof(sendmsg2.flag), 0);
        rounds_tips(round);
      }
      else //蓝方
      {
        printf("蓝方\n");
        //翻子
        show = show_chess(x1, y1, chessboard, mp3);
        if (show)
        {
          round = 0;
          sendmsg2.flag = 1;
          msgsnd(msgid, &sendmsg2, sizeof(sendmsg2.flag), 0);
          sendmsg1.flag = 2;
          msgsnd(msgid, &sendmsg1, sizeof(sendmsg1.flag), 0);
          rounds_tips(round);
          continue;
        }
        //走子
        if (move_chess(x1, y1, 1, chessboard, mp3, p))
          round = 0;
        else
          continue;
        sendmsg2.flag = 1;
        msgsnd(msgid, &sendmsg2, sizeof(sendmsg2.flag), 0);
        sendmsg1.flag = 2;
        msgsnd(msgid, &sendmsg1, sizeof(sendmsg1.flag), 0);
        rounds_tips(round);
      }
    }
    // 退出
    else if (is_key_area(p, 718, 400, 800, 480))
    {
      sendmsg1.flag = -1;
      msgsnd(msgid, &sendmsg1, sizeof(sendmsg1.flag), 0);
      sendmsg2.flag = -1;
      msgsnd(msgid, &sendmsg2, sizeof(sendmsg2.flag), 0);
      printf("log-xmrs: 退出\n");
      break;
    }
    // 积分
    else if (is_key_area(p, 643, 400, 723, 480))
    {
      printf("log-xmrs: 积分\n");
    }
    // 悔棋
    else if (is_key_area(p, 90, 400, 150, 480))
    {
      printf("log-xmrs: 悔棋\n");
    }
    // 重新开始
    else if (is_key_area(p, 10, 400, 80, 480))
    {
      play_mp3(mp3[8]);
      printf("log-xmrs: 重新开始\n");
      sendmsg1.flag = -1;
      msgsnd(msgid, &sendmsg1, sizeof(sendmsg1.flag), 0);
      sendmsg2.flag = -1;
      msgsnd(msgid, &sendmsg2, sizeof(sendmsg2.flag), 0);
      destroy_chess_pieces_info_link(head);
      create_chess_pieces_info_link(head);
      chessboard_init(chessboard, ids, head);
      game_interface_init();
      sleep(1);
      play_mp3(mp3[10]);
      rounds(&round);
      add_task(pool, red_time, (void *)task_arg1);
      add_task(pool, blue_time, (void *)task_arg2);
    }
  }
  game_return(head, msgid, pool);
}

void games(xy *p)
{
  int i, n;
  int id[36] = {0};
  char path[512];
  chess_info_p chessboard[6][6];

  bzero(path, sizeof(path));
  read_pictrue_path("game_loading", path);
  display_picture(path, 0, 0, false);

  char mp3[13][100] = {
      {"./mp3/game/1.mp3"},
      {"./mp3/game/2.mp3"},
      {"./mp3/game/3.mp3"},
      {"./mp3/game/4.mp3"},
      {"./mp3/game/5.mp3"},
      {"./mp3/game/6.mp3"},
      {"./mp3/game/clike.mp3"},
      {"./mp3/game/down.mp3"},
      {"./mp3/game/restart.mp3"},
      {"./mp3/game/restart1.mp3"},
      {"./mp3/game/restart2.mp3"},
      {"./mp3/game/restart3.mp3"},
      {"./mp3/game/wellcome.mp3"},
  };

  play_mp3(mp3[12]);
  rand_data(id, 36, 1, 36);
  chess_info_p chess_list = chess_link_init();
  create_chess_pieces_info_link(chess_list);
  chessboard_init(chessboard, id, chess_list);

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

  // 线程池初始化
  thread_pool *pool = (thread_pool *)malloc(sizeof(thread_pool));
  init_pool(pool, 4);

  sleep(1);
  game_interface_init();
  games_control(chessboard, mp3, id, chess_list, msgid, pool, p);
  printf("log-xmrs: games 退出\n");
  exit(EXIT_SUCCESS);
}
