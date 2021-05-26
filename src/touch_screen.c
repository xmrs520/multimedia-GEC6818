#include "../inc/main.h"
/**
 * 获取坐标
*/
int get_xy(xy *p)
{
  int touch_fd = open(TOUCH_SCREENT_PATH, O_RDWR);
  if (touch_fd < 0)
  {
    perror("Failed to open touch screent\n");
    return 0;
  }

  struct input_event ts;

  while (1)
  {
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
    if (ts.type == EV_KEY && ts.code == BTN_TOUCH && ts.value == 0)
      break;
  }
  if (close(touch_fd) != 0)
    perror("关闭文件失败");
  return 1;
}

/**
 * 获取坐标 plus
*/
bool get_xy_plus(xy *p)
{
  bool flag = true;
  int x, y;
  int touch_fd = open(TOUCH_SCREENT_PATH, O_RDWR);
  if (touch_fd < 0)
  {
    perror("Failed to open touch screent");
    return false;
  }

  struct input_event ts;

  while (1)
  {
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
    if (ts.type == EV_ABS && ts.code == 0)
    {
      if (flag)
      {
        flag = false;
        x = p->x;
        y = p->y;
      }
      //printf("down(%d,%d)\n", p->x, p->y);
    }

    //up
    if (ts.type == EV_KEY && ts.code == BTN_TOUCH && ts.value == 0)
    {
      flag = true;
      //printf("up(%d,%d)\n", p->x, p->y);
      if ((p->x - x) < 10 && (p->y - y) < 10)
        break;
    }
  }
  if (close(touch_fd) != 0)
    perror("关闭文件失败");
  return true;
}

/**
 * 是否在触摸键区域
 * (x1,y1),(x2,y2)
 * p: 坐标结构体指针;
*/
bool is_key_area(xy *p, int x1, int y1, int x2, int y2)
{
  if ((p->x > x1 && p->x < x2) &&
      (p->y > y1 && p->y < y2))
  {
    return true;
  }
  else
  {
    return false;
  }
}
