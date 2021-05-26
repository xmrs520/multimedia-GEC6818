#include "../inc/main.h"

/**
 加载必要文件
*/

void *loadin_file(void *arg)
{
  char buf[256];
  char dir_mp3[256];   //mp3路径
  char dir_music[256]; //music路径
  char dir_video[256]; //video路径
  char dir_data[256];  //数据路径
  char music_list_data[256];
  char data[256];
  char path[512];
  char song[50];
  char singer[40];
  //获取当前绝对路径
  getcwd(buf, sizeof(buf));

  sprintf(dir_mp3, "%s/mp3", buf);
  sprintf(dir_video, "%s/video", buf);
  sprintf(dir_data, "%s/data", buf);
  sprintf(dir_music, "%s/music", dir_mp3);

  //检查一下目录是否存在
  if (access(dir_mp3, F_OK) != 0)
  {
    mkdir(dir_mp3, 0777);
  }
  if (access(dir_video, F_OK) != 0)
  {
    mkdir(dir_video, 0777);
  }
  if (access(dir_data, F_OK) != 0)
  {
    mkdir(dir_data, 0777);
  }
  if (access(dir_music, F_OK) != 0)
  {
    mkdir(dir_music, 0777);
  }

  /*打开music目录*/

  /*打开music目录*/
  DIR *music_dp = opendir(dir_music);
  if (music_dp != NULL)
  {
    struct dirent *music_ep = NULL;
    while (1) //扫描歌曲
    {
      music_ep = readdir(music_dp);
      if (music_ep == NULL)
        break;
      if (music_ep->d_type == DT_REG)
      {
        //检查是否为MP3文件
        if (strstr(music_ep->d_name, ".mp3") != NULL)
        {
          bzero(song, sizeof(song));
          bzero(singer, sizeof(singer));
          bzero(path, sizeof(path));
          sscanf(music_ep->d_name, "%[^-]-%[^.].", song, singer);
          sprintf(music_list_data, "%s/music.data", dir_data);
          sprintf(path, "%s/%s", dir_music, music_ep->d_name);
          FILE *data_fp;
          data_fp = fopen(music_list_data, "w+"); //保存到音乐数据文件中
          if (data_fp != NULL)
          {
            sprintf(data, "song-%s-singer-%s-path:%s\n", song, singer, path);
            fwrite(data, sizeof(data), 1, data_fp);
            fclose(data_fp);
          }
        }
      }
    }
    closedir(music_dp);
  }
  else
  {
    perror("打开music目录失败");
    return NULL;
  }

  //扫描歌单
  // if (ep->d_type == DT_REG && strcmp(ep->d_name, "music.data") == 0)
  // {
  // }
}

//扫描视频
