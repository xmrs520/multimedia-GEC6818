#ifndef __font_h__
#define __font_h__

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#define color u32
#define getColor(a, b, c, d) (a | b << 8 | c << 16 | d << 24)

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;

typedef struct stbtt_fontinfo
{
  void *userdata;
  unsigned char *data; // pointer to .ttf file
  int fontstart;       // offset of start of font

  int numGlyphs; // number of glyphs, needed for range checking

  int loca, head, glyf, hhea, hmtx, kern; // table locations as offset from start of .ttf
  int index_map;                          // a cmap mapping for our chosen character encoding
  int indexToLocFormat;                   // format needed to map from glyph index to glyph
} stbtt_fontinfo;

typedef struct
{
  u32 height;
  u32 width;
  u32 byteperpixel;
  u8 *map;
} bitmap;

typedef struct
{
  stbtt_fontinfo *info;
  u8 *buffer;
  float scale;
} font;

//lcd设备结构体
struct LcdDevice
{
  int fd;
  unsigned int *mp; //保存映射首地址
};

//1.初始化字库
font *fontLoad(char *fontPath);

//2.设置字体的大小
void fontSetSize(font *f, s32 pixels);

//3.设置字体输出框的大小
bitmap *createBitmap(u32 width, u32 height, u32 byteperpixel);

//可以指定输出框的颜色
bitmap *createBitmapWithInit(u32 width, u32 height, u32 byteperpixel, color c);

//4.把字体输出到输出框中
void fontPrint(font *f, bitmap *screen, s32 x, s32 y, char *text, color c, s32 maxWidth);

//5.把输出框的所有信息显示到LCD屏幕中
void show_font_to_lcd(unsigned int *p, int px, int py, bitmap *bm);

// 关闭字体库
void fontUnload(font *f);

// 关闭bitmap
void destroyBitmap(bitmap *bm);

//打印字
int font_tools(char *test,
               int x,
               int y,
               int size,
               unsigned int tcolor,
               int width,
               int height,
               unsigned int bgc);

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
                 unsigned int bgc);
#endif