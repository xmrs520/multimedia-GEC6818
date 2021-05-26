#include "../inc/main.h"

/**
 * 检查指定进程是否运行
 * return : 成功 返回pid 失败返回 -1
*/
pid_t get_pid(char *name)
{
  char buf[256] = "";
  char cmd[30] = "";
  pid_t pid = 0;
  sprintf(cmd, "pidof %s", name);

  FILE *pFd = popen(cmd, "r");
  if (pFd != NULL)
  {
    while (fgets(buf, sizeof(buf), pFd))
    {
      pid = atoi(buf);
      break;
    }
    pclose(pFd);
  }
  //pclose(pFd);
  return pid;
}

/**
 * 时间格式化
 * flag: ture 时分秒 false 分秒
*/
void time_format(int second, char *time_str, bool flag)
{
  int h, m, s;
  char hh[4], mm[4], ss[4];
  h = second / 3600;
  m = second % 3600 / 60;
  s = second % 60;
  if (h < 10)
    sprintf(hh, "0%d", h);
  else
    sprintf(hh, "%d", h);
  if (m < 10)
    sprintf(mm, "0%d", m);
  else
    sprintf(mm, "%d", m);
  if (s < 10)
    sprintf(ss, "0%d", s);
  else
    sprintf(ss, "%d", s);
  if (flag)
    sprintf(time_str, "%s:%s:%s", hh, mm, ss);
  else
    sprintf(time_str, "%s:%s", mm, ss);
}

/**
 * 在屏幕上显示文字
*/
void fontt(char *text,
           unsigned int x,
           unsigned int y,
           int size,
           unsigned int color)
{
  Init_Font();

  Clean_Area(0,    //x坐标起始点
             0,    //y坐标起始点
             0,    //绘制的宽度
             0,    //绘制的高度
             0x0); //往屏幕指定区域填充颜色

  Display_characterX(x,     //x坐标起始点
                     y,     //y坐标起始点
                     text,  //GB2312 中文字符串
                     color, //字体颜色值
                     size);

  UnInit_Font();
  printf("在屏幕上显示文字 : %s\n", text);
}

/**
 * 在屏幕上自定义显示文字
*/
void fonts(char *text,
           unsigned int x,
           unsigned int y,
           int size,
           unsigned int color,
           int Xbg,
           int Ybg,
           int width,
           int height,
           unsigned long bgc)
{
  Init_Font();

  Clean_Area(Xbg,    //x坐标起始点
             Ybg,    //y坐标起始点
             width,  //绘制的宽度
             height, //绘制的高度
             bgc);   //往屏幕指定区域填充颜色

  Display_characterX(x,     //x坐标起始点
                     y,     //y坐标起始点
                     text,  //GB2312 中文字符串
                     color, //字体颜色值
                     size);

  UnInit_Font();
  printf("在屏幕上显示文字 : %s\n", text);
}
