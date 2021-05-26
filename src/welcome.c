#include "../inc/main.h"

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
  close(lcdfd);

  return 0;
}

/**
 * 进度条
*/
int progress_bar()
{
  int lcd_fd = open(LCD_PATH, O_RDWR);
  if (lcd_fd < 0)
  {
    printf("Failed to open lcd\n");
    return 0;
  }
  int x, y;
  int color = 0x00ff0066;
  int *share_addr = NULL;
  share_addr = mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, lcd_fd, 0);
  if (share_addr == MAP_FAILED)
  {
    printf("mmap failed\n");
  }
  for (x = 0; x < 800; x++)
  {
    usleep(500);
    for (y = 0; y < 15; y++)
      memcpy(share_addr + x + (y + 465) * 800, &color, 4);
  }

  munmap(share_addr, 800 * 480 * 4);
  close(lcd_fd);
}

//广告
void *handle2(void *arg)
{
  char path[512];
  bzero(path, sizeof(path));
  read_pictrue_path("advertisement1", path);
  display_picture(path, 0, 0, false);
  bzero(path, sizeof(path));
  read_pictrue_path("advertisement2", path);
  sleep(2);
  display_picture(path, 0, 0, false);
  sleep(2);
}

/**
 * 欢迎界面
*/
bool welcome()
{
  //线程池初始化
  thread_pool *pool = (thread_pool *)malloc(sizeof(thread_pool));
  init_pool(pool, 1);
  char path[512];
  read_pictrue_path("loading", path);
  display_picture(path, 0, 0, false);
  //进度条
  progress_bar();
  //广告
  add_task(pool, handle2, (void *)"广告");
  //销毁线程池
  destroy_pool(pool);
}