#include "../inc/main.h"

int main(int argc, char const *argv[])
{
  music_list_p music_head = music_list_init();
  video_list_p video_head = video_list_init();
  xy p;

  //加载界面
  welcome();
  //主页面
  home(music_head, video_head, &p);

  return 0;
}
