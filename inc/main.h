#ifndef __MAIN_H_
#define __MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>
#include <strings.h>
#include <string.h>
#include <syslog.h>
#include <sys/file.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <pthread.h>
#include <semaphore.h>
#include <dirent.h>
#include <fcntl.h>
#include <math.h>
#include <sys/mman.h>
#include <linux/input.h>
#include "thread_pool.h"
#include "font.h"

#define LCD_WIDTH 800
#define LCD_HEIGHT 480

#define LCD_PATH "/dev/fb0"
#define TOUCH_SCREENT_PATH "/dev/input/event0"

#define IMG_DATA_PATH "./data/pictrue.data"
#define MUSIC_DATA_PATH "./data/music.data"
#define VIDEO_DATA_PATH "./data/video.data"
/**
 * 坐标结构体
*/
typedef struct index_x_y
{
  int x; //横坐标
  int y; //纵坐标
} xy;

/**
 * 歌曲列表
*/
typedef struct music_list
{
  char song_title[100]; //歌名
  char singer[100];     //歌手
  int index;            //序号
  int page;             //页码
  char path[256];       //路径
  struct music_list *next;
  struct music_list *prev;
} music_list, *music_list_p;

/**
 * 视频列表
*/
typedef struct video_list
{
  char name[512]; // 视频名
  char path[512]; // 路径
  int index;      // 序号
  int page;       // 页码
  struct video_list *next;
  struct video_list *prev;
} video_list, *video_list_p;

/**
 * 参数
*/
struct arg
{
  int arg1;
  char arg2[256];
  char arg3[512];
  char *arg4[10];
  void *ptr[10];
};

/**
 * 消息队列 数据
*/
struct msgbuf
{
  long int mtype; /*消息的类型 */
  int flag;       /*播放状态*/
  int pos;        /*播放进度 1~100*/
  int time;       /*总时长*/
  char mtext[50]; /*备注*/
};

/**
 * 共享内存 数据
*/
typedef struct shmbuf
{
  float pos;  //进度值
  int volume; //声音值
  int pause;  //暂停标志位
  int time;   //总时长
} shmbuf;

/**
 * 链式队
*/
typedef struct qnode
{
  char data[200];
  int time;
  struct qnode *next;
  struct qnode *prev;
} data_node, *data_node_p;

typedef struct link
{
  struct qnode *front;
  struct qnode *rear;
} link_queue, *link_queue_p;

/**
 * 棋子信息
*/
typedef struct chess_pieces_info
{
  int id;
  int x;
  int y;
  int level;
  int role;     //0: 红方 1: 蓝方
  int show;     //0: 不显示 1: 显示
  int selected; //0: 未选中 1: 选中
  char path[256];
  struct chess_pieces_info *next;
  struct chess_pieces_info *prev;
} chess_info, *chess_info_p;

/**
 * 走棋记录
*/
typedef struct chess_pieces_backup
{
  int id;
  int x;
  int y;
  int show;
} chess_backup, *chess_backup_p;

/**
 * 走棋记录 链栈管理
*/
typedef struct stack
{
  int size;        //栈大小
  struct key *top; //栈顶
} link_stack, *link_stack_p;
/****************welcome.c start*****************************************************/

/**
 * 进度条
*/
int progress_bar();

/**
 * 欢迎界面
*/
bool welcome();

/****************welcome.c end *****************************************************/

/****************touch_screen.c start*****************************************************/

/**
 * 获取坐标
*/
int get_xy(xy *p);

/**
 * 获取坐标 plus
*/
bool get_xy_plus(xy *p);

/**
 * 是否在触摸键区域
 * (x1,y1),(x2,y2)
 * p: 坐标结构体指针;
*/
bool is_key_area(xy *p, int x1, int y1, int x2, int y2);

/****************touch_screen.c end *****************************************************/

/****************home.c start *****************************************************/

/**
 * 主页界面
*/
void home(music_list_p music_head, video_list_p video_head, xy *p);

/****************home.c end *****************************************************/

/****************music.c start *****************************************************/

/**
 * 初始化音乐列表
*/
music_list_p music_list_init();

/**
 * 音乐初始化
*/
int music_init(music_list_p head);

/**
 * 销毁音乐列表
*/
void destroy_music_list(music_list_p head);

/**
 * 创建音乐列表
*/
bool create_music_list(
    music_list_p head,
    char *song,
    char *singer,
    int index,
    int pages,
    char *path);

/**
 * 打印歌曲列表
*/
void display_music_list(music_list_p head);

/**
 * 打印歌单
*/
void show_music_list(int pages, music_list_p head);

/**
 * 音乐
*/
void music(music_list_p head, xy *p);

/**
 * 创建管道
*/
bool make_fifo();

/****************music.c end *****************************************************/

/****************piano.c start *****************************************************/

void piano_interface(xy *p);

/****************piano.c end *****************************************************/

/****************read_file.c start *****************************************************/

/**
 * 读取图片地址
*/
bool read_pictrue_path(char *name, char *path);

/**
 * 读取歌词
*/
FILE *read_lyric(char *path);

/**
 * 读歌曲列表并保存到链表中
*/
int read_music_list(music_list_p head);

/**
 * 读取视频列表并保存到链表中
 * 返回值：总视频数
*/
int read_video_list(video_list_p head);

/****************read_file.c end *****************************************************/
/****************video.c start *****************************************************/

/**
 * 初始化视频列表
*/
video_list_p video_list_init();

/**
 * 创建视频列表
*/
bool video_create_node(video_list_p head, char *names, char *paths, int indexs, int pages);

/**
 * 寻找列表中第 n 视频
*/
video_list_p find_video(video_list_p head, int *n);

/**
 * 视频界面
*/
void video_interface(video_list_p video_head, xy *p);

/****************video.c end *****************************************************/

/****************font.c start *****************************************************/

/**
 * 在屏幕上显示文字
*/
void fontt(char *text,
           unsigned int x,
           unsigned int y,
           int size,
           unsigned int color);
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
           unsigned long bgc);
/****************font.c end *****************************************************/

/****************tool.c start *****************************************************/

/**
 * 检查指定进程是否运行
 * return : 成功 返回pid 失败返回 -1
*/
int get_pid(char *name);

/**
 * 时间格式化
 * flag: ture 时分秒 false 分秒
*/
void time_format(int second, char *time_str, bool flag);

int Init_Font(void);                 //显示字体库前先调用本函数进行初始化
void UnInit_Font(void);              //程序退出前，调用本函数
int Clean_Area(int X,                //x坐标起始点
               int Y,                //y坐标起始点
               int width,            //绘制的宽度
               int height,           //绘制的高度
               unsigned long color); //往屏幕指定区域填充颜色

int Display_characterX(unsigned int x,        //x坐标起始点
                       unsigned int y,        //y坐标起始点
                       unsigned char *string, //GB2312 中文字符串
                       unsigned int color,    //字体颜色值
                       int size);             //字体放大倍数 1~8

/**
 * 打印图片到屏幕
 * filename : 图片路径
 * setx,sety:要显示图片的起始位置
 * clean:是否清屏
*/
int display_picture(char *filename, int setx, int sety, bool clean);

/**
 * 圆形图片
*/
int display_picture_circle(char *filename, int setx, int sety, bool clean);

/****************tool.c end *****************************************************/

/****************lrc.c start *****************************************************/

link_queue_p queue_init();

bool queue_empty(link_queue_p q);

bool en_queue(link_queue_p q, int times, char *datas);

bool dequeue(link_queue_p q, int *times, char *datas);

bool display_lrc(link_queue_p q);

void destroy_queue(link_queue_p q);

/****************lrc.c end *****************************************************/

/****************game.c start *****************************************************/

/**
 * 皇家战棋
*/
void games(xy *p);

/****************game.c end *****************************************************/

#endif