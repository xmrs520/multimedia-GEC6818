#include "../inc/main.h"

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