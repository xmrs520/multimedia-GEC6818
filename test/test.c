#include "../inc/main.h"

int main(int argc, char const *argv[])
{

  system("madplay ./mp3/music/haidi.mp3 &");
  sleep(5);
  printf("1111111111111\n");
  system("killall -19 madplay &");
  system("mplayer -quiet  './mp3/music/Five Hundred Miles-Stark SandsCarey M.mp3' &");
  return 0;
}
