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

/**
 * 打印图片到屏幕
 * filename : 图片路径
 * setx,sety:要显示图片的起始位置
 * clean:是否清屏
*/
int display_picture(char *filename, int setx, int sety, bool clean)
{
  printf("filename: %s \n", filename);
  //打开lcd屏幕设备
  int lcdfd = open("/dev/fb0", O_RDWR);
  if (lcdfd == -1)
  {
    perror("Failed to open lcd");
    return -1;
  }

  //申请映射内存---假定lcd屏幕就是800*480的分辨率，每个像素点4个字节
  int *lcdmem = mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, lcdfd, 0);
  if (lcdmem == MAP_FAILED)
  {
    perror("mmap lcdmem failed");
    return -1;
  }
  //清屏
  if (clean)
  {
    bzero(lcdmem, LCD_WIDTH * LCD_HEIGHT * 4);
  }

  //从图片中获取颜色数据
  int bmpfd = open(filename, O_RDONLY);
  if (bmpfd == -1)
  {
    perror("Failed to open image");
    return -1;
  }

  //从图片中读取头文件
  char headinfo[54];
  read(bmpfd, headinfo, 54);

  //从图片的头文件中获取图片的宽度和高度
  int bmp_w = headinfo[18] | headinfo[19] << 8 | headinfo[20] << 16 | headinfo[21] << 24;
  int bmp_h = headinfo[22] | headinfo[23] << 8 | headinfo[24] << 16 | headinfo[25] << 24;
  printf("bmp_w=%d,bmp_h=%d\n", bmp_w, bmp_h);

  int win_size = (4 - bmp_w * 3 % 4) % 4; //windows补齐的字节数
  int bmp_lenth = bmp_w * 3 + win_size;   //图片1行的字节数= 像素点个数*3 + windows补齐的字节数

  char bmpbuf[bmp_lenth * bmp_h];         //bmp图片总共的颜色数据,单位字节
  read(bmpfd, bmpbuf, bmp_lenth * bmp_h); //从图片文件中读取所有数据读到缓冲区里面

  int x, y;
  char lcdbuf[bmp_w * bmp_h * 4]; //只定义图片大小的缓冲区
  for (x = 0; x < bmp_w; x++)
    for (y = 0; y < bmp_h; y++)
    {
      lcdbuf[4 * (x + bmp_w * y)] = bmpbuf[3 * x + bmp_lenth * y];         //图片的蓝色对齐lcd的蓝色
      lcdbuf[4 * (x + bmp_w * y) + 1] = bmpbuf[3 * x + bmp_lenth * y + 1]; //图片的绿色对齐lcd的绿色
      lcdbuf[4 * (x + bmp_w * y) + 2] = bmpbuf[3 * x + bmp_lenth * y + 2]; //图片的红色对齐lcd的红色
      lcdbuf[4 * (x + bmp_w * y) + 3] = 0;                                 //图片没有透明度，所以lcd的透明度不用对齐
    }

  //确定每一行需要绘制的字节个数
  //如果图片宽度小于 800-setx,则绘制图片的宽度
  //如果图片宽度大于 800-setx,
  int draw_lenth = bmp_w < 800 - setx ? bmp_w : 800 - setx;

  for (y = 0; y < 480 - sety && y < bmp_h; y++)
    memcpy(lcdmem + setx + (y + sety) * 800, &lcdbuf[4 * (bmp_w * (bmp_h - 1 - y))], draw_lenth * 4);

  munmap(lcdmem, 800 * 480 * 4);
  if (close(lcdfd) != 0)
    perror("打印图片 关闭文件失败");
  return 0;
}

/**
 * 圆形图片
*/
int display_picture_circle(char *filename, int setx, int sety, bool clean)
{
  double center_point_x = 0.0;
  double center_point_y = 0.0;
  double r = 0.0; //半径
  if (filename == NULL)
    return 0;
  printf("filename: %s \n", filename);
  //打开lcd屏幕设备
  int lcdfd = open("/dev/fb0", O_RDWR);
  if (lcdfd == -1)
  {
    perror("Failed to open lcd");
    return -1;
  }

  //申请映射内存---假定lcd屏幕就是800*480的分辨率，每个像素点4个字节
  int *lcdmem = mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, lcdfd, 0);
  if (lcdmem == MAP_FAILED)
  {
    perror("mmap lcdmem failed");
    return -1;
  }
  //清屏
  if (clean)
  {
    bzero(lcdmem, LCD_WIDTH * LCD_HEIGHT * 4);
  }

  //从图片中获取颜色数据
  int bmpfd = open(filename, O_RDONLY);
  if (bmpfd == -1)
  {
    perror("Failed to open image");
    return -1;
  }

  //从图片中读取头文件
  char headinfo[54];
  read(bmpfd, headinfo, 54);

  //从图片的头文件中获取图片的宽度和高度
  int bmp_w = headinfo[18] | headinfo[19] << 8 | headinfo[20] << 16 | headinfo[21] << 24;
  int bmp_h = headinfo[22] | headinfo[23] << 8 | headinfo[24] << 16 | headinfo[25] << 24;
  printf("bmp_w=%d,bmp_h=%d\n", bmp_w, bmp_h);

  int win_size = (4 - bmp_w * 3 % 4) % 4; //windows补齐的字节数
  int bmp_lenth = bmp_w * 3 + win_size;   //图片1行的字节数= 像素点个数*3 + windows补齐的字节数

  char bmpbuf[bmp_lenth * bmp_h];         //bmp图片总共的颜色数据,单位字节
  read(bmpfd, bmpbuf, bmp_lenth * bmp_h); //从图片文件中读取所有数据读到缓冲区里面

  //圆心
  center_point_x = (double)bmp_w / 2;
  center_point_y = (double)bmp_h / 2;
  //半径
  r = center_point_x < center_point_y ? center_point_x : center_point_y;
  double d = 0.0;
  int x, y;
  char lcdbuf[bmp_w * bmp_h * 4]; //只定义图片大小的缓冲区
  for (x = 0; x < bmp_w; x++)
    for (y = 0; y < bmp_h; y++)
    {

      lcdbuf[4 * (x + bmp_w * y)] = bmpbuf[3 * x + bmp_lenth * y];         //图片的蓝色对齐lcd的蓝色
      lcdbuf[4 * (x + bmp_w * y) + 1] = bmpbuf[3 * x + bmp_lenth * y + 1]; //图片的绿色对齐lcd的绿色
      lcdbuf[4 * (x + bmp_w * y) + 2] = bmpbuf[3 * x + bmp_lenth * y + 2]; //图片的红色对齐lcd的红色
      lcdbuf[4 * (x + bmp_w * y) + 3] = 0;                                 //图片没有透明度，所以lcd的透明度不用对齐
    }

  //确定每一行需要绘制的字节个数
  //如果图片宽度小于 800-setx,则绘制图片的宽度
  //如果图片宽度大于 800-setx,
  int draw_lenth = bmp_w < 800 - setx ? bmp_w : 800 - setx;

  for (y = 0; y < 480 - sety && y < bmp_h; y++)
    for (x = 0; x < 800 - setx && x < bmp_w; x++)
    {
      d = sqrt(pow(((double)x - center_point_x), 2.0) + pow(((double)y - center_point_y), 2.0));
      if (d <= r)
      {
        //lcdmem[(setx + x) + (y + sety) * 800] = lcdbuf[4 * (bmp_w * (bmp_h - 1 - y) + x)];
        memcpy(lcdmem + (setx + x) + (y + sety) * 800, &lcdbuf[4 * (bmp_w * (bmp_h - 1 - y) + x)], 4);
      }
    }

  munmap(lcdmem, 800 * 480 * 4);
  if (close(lcdfd) != 0)
    perror("圆形图片 关闭文件失败");
  return 0;
}