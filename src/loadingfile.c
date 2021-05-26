#include "../inc/main.h"

/**
 ���ر�Ҫ�ļ�
*/

void *loadin_file(void *arg)
{
  char buf[256];
  char dir_mp3[256];   //mp3·��
  char dir_music[256]; //music·��
  char dir_video[256]; //video·��
  char dir_data[256];  //����·��
  char music_list_data[256];
  char data[256];
  char path[512];
  char song[50];
  char singer[40];
  //��ȡ��ǰ����·��
  getcwd(buf, sizeof(buf));

  sprintf(dir_mp3, "%s/mp3", buf);
  sprintf(dir_video, "%s/video", buf);
  sprintf(dir_data, "%s/data", buf);
  sprintf(dir_music, "%s/music", dir_mp3);

  //���һ��Ŀ¼�Ƿ����
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

  /*��musicĿ¼*/

  /*��musicĿ¼*/
  DIR *music_dp = opendir(dir_music);
  if (music_dp != NULL)
  {
    struct dirent *music_ep = NULL;
    while (1) //ɨ�����
    {
      music_ep = readdir(music_dp);
      if (music_ep == NULL)
        break;
      if (music_ep->d_type == DT_REG)
      {
        //����Ƿ�ΪMP3�ļ�
        if (strstr(music_ep->d_name, ".mp3") != NULL)
        {
          bzero(song, sizeof(song));
          bzero(singer, sizeof(singer));
          bzero(path, sizeof(path));
          sscanf(music_ep->d_name, "%[^-]-%[^.].", song, singer);
          sprintf(music_list_data, "%s/music.data", dir_data);
          sprintf(path, "%s/%s", dir_music, music_ep->d_name);
          FILE *data_fp;
          data_fp = fopen(music_list_data, "w+"); //���浽���������ļ���
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
    perror("��musicĿ¼ʧ��");
    return NULL;
  }

  //ɨ��赥
  // if (ep->d_type == DT_REG && strcmp(ep->d_name, "music.data") == 0)
  // {
  // }
}

//ɨ����Ƶ
