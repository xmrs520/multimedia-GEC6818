#include "../inc/main.h"

/**
 * 音乐
*/
void to_music(music_list_p music_head, xy *p)
{
  music(music_head, p);
}
// void *to_music(void *arg)
// {
//   //接收参数
//   struct arg *args = (struct arg *)arg;
//   music_list_p head = (music_list_p)(args->ptr[0]);
//   xy *p = (xy *)(args->ptr[1]);
//   music(head, p);
// }

/**
 * 娱乐
*/
void to_happy(xy *p)
{
  piano_interface(p);
}

/**
 * 放映厅
*/
void to_video(video_list_p video_head, xy *p)
{
  video_interface(video_head, p);
}

/**
 * 显示主界面
*/
void show_home()
{
  char path[512];
  bzero(path, sizeof(path));
  read_pictrue_path("home", path);
  display_picture(path, 0, 0, false);
}
/**
 * 广播mplayer消息
*/
// void *broadcast_mp(void *arg)
// {
//   //接收参数
//   struct arg *args = (struct arg *)arg;
//   int *fd_pipe = (int *)(args->ptr[0]);
//   char buf[100];
//   close(fd_pipe[1]);
//   int size = 0;
//   while (1)
//   {
//     sleep(1);
//     bzero(buf, sizeof(buf));
//     size = read(fd_pipe[0], buf, sizeof(buf)); //从无名管道的写端读取信息打印在屏幕上
//     buf[size] = '\0';
//     printf("【pipe】 %s\n", buf);
//   }
// }

/**
 * 主页界面
*/
void home(music_list_p music_head, video_list_p video_head, xy *p)
{
  show_home();
  int status;
  while (1)
  {
    get_xy(p);
    if (is_key_area(p, 300, 28, 460, 120)) //娱乐
    {
      pid_t pid = fork();
      if (pid == 0) //子进程
      {
        to_happy(p);
      }
      if (pid > 0) //父进程
      {
        waitpid(pid, &status, 0);
        //判断子进程是否正常退出，如果是正常返回为真
        if (WIFEXITED(status))
          printf("log-xmrs: 主界面\n");
        else
          printf("log-xmrs: 娱乐进程非正常退出\n");
        show_home();
      }
    }
    else if (is_key_area(p, 67, 206, 225, 280)) //放映厅
    {
      pid_t pid = fork();
      if (pid == 0) //子进程
      {
        to_video(video_head, p);
      }
      if (pid > 0) //父进程
      {
        waitpid(pid, &status, 0);
        //判断子进程是否正常退出，如果是正常返回为真
        if (WIFEXITED(status))
          printf("log-xmrs: 主界面\n");
        else
          printf("log-xmrs: 视频进程非正常退出\n");
        show_home();
      }
    }
    else if (is_key_area(p, 300, 350, 460, 440)) //音乐
    {
      pid_t pid = fork();
      if (pid == 0) //子进程
      {
        to_music(music_head, p);
      }
      if (pid > 0) //父进程
      {
        waitpid(pid, &status, 0);
        //判断子进程是否正常退出，如果是正常返回为真
        if (WIFEXITED(status))
          printf("log-xmrs: 主界面\n");
        else
          printf("log-xmrs: 音乐进程非正常退出\n");
        show_home();
      }
    }
  }
}