#include "../inc/main.h"

/**
 * 读取图片地址
*/
bool read_pictrue_path(char *name, char *path)
{
  FILE *fp;
  char *ret;
  char buf[512];
  char getname[30];
  fp = fopen(IMG_DATA_PATH, "r");
  if (fp != NULL)
  {
    while (1)
    {
      ret = fgets(buf, sizeof(buf), fp);
      if (ret == NULL)
      {
        break;
      }
      sscanf(buf, "%s", getname);
      if (strcmp(getname, name) == 0)
      {
        sscanf(buf, "%*[^:]:%s", path);
        fclose(fp);
        return true;
      }
    }
    printf("没有那个图片\n");
    return false;
  }
  else
  {
    printf("打开图片数据出错\n");
    return false;
  }
}

/**
 * 读取歌词文件
*/
FILE *read_lyric(char *path)
{
  char buf[256];
  char name[100];
  char lrc[256];
  //获取当前绝对路径
  getcwd(buf, sizeof(buf));
  //文件路径
  sscanf(strstr(path, "music/"), "%*[^/]/%[^.].", name);
  sprintf(lrc, "%s/data/%s.lrc", buf, name);
  //printf("read_lyric%s \n", lrc);
  FILE *fp;
  fp = fopen(lrc, "r");
  if (fp != NULL)
    return fp;
  else
    return NULL;
}

/**
 * 读歌曲列表并保存到链表中
*/
int read_music_list(music_list_p head)
{
  FILE *fp;
  char *ret;
  char buf[512];
  char song[100];
  char singer[100];
  int index = 0;
  int pages = 1;
  char path[200];

  fp = fopen(MUSIC_DATA_PATH, "r");
  if (fp != NULL)
  {
    while (1)
    {
      ret = fgets(buf, sizeof(buf), fp);
      if (ret == NULL)
      {
        break;
      }
      sscanf(buf, "%*[^-]-%[^-]-%*[^-]-%[^-]-%*[^:]:%[^\n]", song, singer, path);
      index++;
      if (index % 5 == 0)
      {
        pages++;
      }
      create_music_list(head, song, singer, index, pages, path);
      bzero(buf, sizeof(buf));
      bzero(song, sizeof(song));
      bzero(singer, sizeof(singer));
      bzero(path, sizeof(path));
    }
    fclose(fp);
    return index;
  }
  else
  {
    printf("打开音乐数据出错\n");
    return 0;
  }
}

/**
 * 读取视频列表并保存到链表中
 * 返回值：总视频数
*/
int read_video_list(video_list_p head)
{
  FILE *fp;
  char *ret;
  char buf[512];
  char name[512]; // 视频名
  char path[512]; // 路径
  int index = 0;  // 序号
  int page = 1;   // 页码
  fp = fopen(VIDEO_DATA_PATH, "r");
  if (fp != NULL)
  {
    while (1)
    {
      bzero(buf, sizeof(buf));
      bzero(name, sizeof(name));
      bzero(path, sizeof(path));
      ret = fgets(buf, sizeof(buf), fp);
      if (ret == NULL)
        break;
      sscanf(buf, "%*[^:]:%[^/]/%*[^:]:%[^\n]", name, path);
      index++;
      if (index % 6 == 0)
        page++;
      video_create_node(head, name, path, index, page);
    }
    fclose(fp);
    return index;
  }
  else
  {
    printf("打开视频数据文件失败\n");
    return 0;
  }
}
