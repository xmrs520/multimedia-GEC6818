#include "../inc/font.h"

//初始化Lcd
struct LcdDevice *init_lcd(const char *device)
{
  //申请空间
  struct LcdDevice *lcd = malloc(sizeof(struct LcdDevice));
  if (lcd == NULL)
  {
    return NULL;
  }

  //1打开设备
  lcd->fd = open(device, O_RDWR);
  if (lcd->fd < 0)
  {
    perror("open lcd fail");
    free(lcd);
    return NULL;
  }

  //映射
  lcd->mp = mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, lcd->fd, 0);

  return lcd;
}

//打印字
int font_tools(char *test,
               int x,
               int y,
               int size,
               unsigned int tcolor,
               int width,
               int height,
               unsigned int bgc)
{
  unsigned int A, R, G, B, a, r, g, b;
  A = bgc >> 24;
  R = (bgc >> 16) & 0xff;
  G = (bgc >> 8) & 0xff;
  B = (bgc >> 0) & 0xff;

  a = tcolor >> 24;
  r = (tcolor >> 16) & 0xff;
  g = (tcolor >> 8) & 0xff;
  b = (tcolor >> 0) & 0xff;

  //初始化Lcd
  struct LcdDevice *lcd = init_lcd("/dev/fb0");

  //打开字体
  font *f = fontLoad("/usr/share/fonts/DroidSansFallback.ttf");

  //字体大小的设置
  fontSetSize(f, size);

  //创建一个画板（点阵图）  250画板宽度     50画板高度
  bitmap *bm = createBitmapWithInit(width, height, 4, getColor(A, B, G, R)); //也可使用createBitmapWithInit函数，改变画板颜色
  //bitmap *bm = createBitmap(288, 100, 4);

  //char buf[] = "晚安123456789"; //显示的数据

  //将字体写到点阵图上
  fontPrint(f, bm, 0, 0, test, getColor(a, b, g, r), 0);

  //把字体框输出到LCD屏幕上
  show_font_to_lcd(lcd->mp, x, y, bm);

  //关闭字体，关闭画板
  fontUnload(f);
  destroyBitmap(bm);
}

//自定义打印字
int font_tools_2(char *test,
                 int x,
                 int y,
                 int X,
                 int Y,
                 int size,
                 unsigned int tcolor,
                 int width,
                 int height,
                 unsigned int bgc)
{
  unsigned int A, R, G, B, a, r, g, b;
  A = bgc >> 24;
  R = (bgc >> 16) & 0xff;
  G = (bgc >> 8) & 0xff;
  B = (bgc >> 0) & 0xff;

  a = tcolor >> 24;
  r = (tcolor >> 16) & 0xff;
  g = (tcolor >> 8) & 0xff;
  b = (tcolor >> 0) & 0xff;

  //初始化Lcd
  struct LcdDevice *lcd = init_lcd("/dev/fb0");

  //打开字体
  font *f = fontLoad("/usr/share/fonts/DroidSansFallback.ttf");

  //字体大小的设置
  fontSetSize(f, size);

  //创建一个画板（点阵图）  250画板宽度     50画板高度
  bitmap *bm = createBitmapWithInit(width, height, 4, getColor(A, B, G, R)); //也可使用createBitmapWithInit函数，改变画板颜色
  //bitmap *bm = createBitmap(288, 100, 4);

  //char buf[] = "晚安123456789"; //显示的数据

  //将字体写到点阵图上
  fontPrint(f, bm, X, Y, test, getColor(a, b, g, r), 0);

  //把字体框输出到LCD屏幕上
  show_font_to_lcd(lcd->mp, x, y, bm);

  //关闭字体，关闭画板
  fontUnload(f);
  destroyBitmap(bm);
}
