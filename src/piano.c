#include "../inc/main.h"

void piano_init()
{
  int i;
  char path[100];
  bzero(path, sizeof(path));
  read_pictrue_path("background", path);
  display_picture(path, 0, 0, false);

  bzero(path, sizeof(path));
  read_pictrue_path("bar", path);
  display_picture(path, 0, 0, false);

  bzero(path, sizeof(path));
  read_pictrue_path("logo", path);
  display_picture(path, 229, 370, false);

  bzero(path, sizeof(path));
  read_pictrue_path("key_off", path);
  for (i = 0; i < 12; i++)
    display_picture(path, 10 + i * 65, 47, false);
}

void *piano_play(void *arg)
{
  usleep(7000);
  system("killall -9 mplayer");
  char cmd[512];
  char buf[512];
  //接收参数
  struct arg *args = (struct arg *)arg;
  bzero(buf, sizeof(buf));
  strcpy(buf, args->arg3);
  bzero(cmd, sizeof(cmd));
  sprintf(cmd, "mplayer -quiet %s &", buf);
  system(cmd);
}

void piano_control(thread_pool *pool, xy *p)
{
  int up = 0;
  int down = 0;
  int old = -1;
  char key_off[100];
  char key_on[100];
  bool flage = true;
  bzero(key_off, sizeof(key_off));
  read_pictrue_path("key_off", key_off);
  bzero(key_on, sizeof(key_on));
  read_pictrue_path("key_on", key_on);
  char mp3[12][100] = {
      {"/mnt/sd/Multi-Media-App/mp3/piano/d1.mp3"},
      {"/mnt/sd/Multi-Media-App/mp3/piano/d2.mp3"},
      {"/mnt/sd/Multi-Media-App/mp3/piano/d3.mp3"},
      {"/mnt/sd/Multi-Media-App/mp3/piano/d4.mp3"},
      {"/mnt/sd/Multi-Media-App/mp3/piano/d5.mp3"},
      {"/mnt/sd/Multi-Media-App/mp3/piano/d6.mp3"},
      {"/mnt/sd/Multi-Media-App/mp3/piano/d7.mp3"},
      {"/mnt/sd/Multi-Media-App/mp3/piano/d8.mp3"},
      {"/mnt/sd/Multi-Media-App/mp3/piano/d9.mp3"},
      {"/mnt/sd/Multi-Media-App/mp3/piano/d10.mp3"},
      {"/mnt/sd/Multi-Media-App/mp3/piano/d11.mp3"},
      {"/mnt/sd/Multi-Media-App/mp3/piano/d12.mp3"},
  };
  int touch_fd = open(TOUCH_SCREENT_PATH, O_RDWR);
  if (touch_fd < 0)
  {
    printf("Failed to open touch screent\n");
  }

  struct input_event ts;
  while (1)
  {
    usleep(1000);
    read(touch_fd, &ts, sizeof(ts));
    if (ts.type == EV_ABS)
    {
      if (ts.code == ABS_X)
      {
        p->x = (ts.value) * 800 / 1024;
      }
      if (ts.code == ABS_Y)
      {
        p->y = (ts.value) * 480 / 600;
      }
    }

    //down
    if ((p->y > 47 && p->y < 327) && (p->x > 10 && p->x < 790))
    {
      if (ts.type == EV_ABS && ts.code == 0)
      {
        if (((p->x - 10) / 65) != down)
        {
          display_picture(key_off, 10 + down * 65, 47, false);
          flage = true;
          // struct arg *task_arg = (struct arg *)malloc(sizeof(struct arg));
          // strcpy(task_arg->arg3, mp3[down]);
          // add_task(pool, piano_play, (void *)task_arg);
        }

        down = (p->x - 10) / 65;
        display_picture(key_on, 10 + down * 65, 47, false);
        printf("down\n");
        if (flage)
        {
          flage = false;
          struct arg *task_arg = (struct arg *)malloc(sizeof(struct arg));
          strcpy(task_arg->arg3, mp3[down]);
          add_task(pool, piano_play, (void *)task_arg);
        }
      }
    }

    //up
    if (ts.type == EV_KEY && ts.code == BTN_TOUCH && ts.value == 0 &&
        (p->y > 47 && p->y < 327) && (p->x > 10 && p->x < 790))
    {
      up = (p->x - 10) / 65;
      display_picture(key_off, 10 + up * 65, 47, false);
      flage = true;
      printf("up\n");
    }

    //break
    if (ts.type == EV_KEY && ts.code == BTN_TOUCH && ts.value == 0 &&
        (p->x < 571 && p->x > 229 && p->y < 436 && p->y > 370))
    {
      break;
    }
  }
  close(touch_fd);
}

void piano_interface(xy *p)
{
  // 线程池初始化
  thread_pool *pool = (thread_pool *)malloc(sizeof(thread_pool));
  init_pool(pool, 3);

  piano_init();
  piano_control(pool, p);

  system("killall -9 mplayer");
  usleep(500);
  destroy_pool(pool);
  exit(EXIT_SUCCESS);
}