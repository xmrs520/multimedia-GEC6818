#include "../inc/main.h"

link_queue_p queue_init()
{
  link_queue_p q = (link_queue_p)malloc(sizeof(link_queue));
  if (q != NULL)
  {
    q->front = q->rear = NULL;
    return q;
  }
  else
  {
    printf("初始化 链队失败\n");
    return NULL;
  }
}

bool queue_empty(link_queue_p q)
{
  return (q->rear == NULL);
}

bool en_queue(link_queue_p q, int times, char *datas)
{
  data_node_p node = (data_node_p)malloc(sizeof(data_node));
  if (node == NULL)
  {
    perror("allocate failed");
    return false;
  }
  strcpy(node->data, datas);
  node->time = times;
  node->next = NULL;
  node->prev = NULL;
  if (queue_empty(q))
  {
    q->front = q->rear = node;
  }
  else
  {
    q->rear->next = node;
    node->prev = q->rear;
    q->rear = node;
  }
  return true;
}

bool dequeue(link_queue_p q, int *times, char *datas)
{
  data_node_p tmp = NULL;
  if (queue_empty(q))
  {
    printf("出队失败，队空\n");
    return false;
  }
  tmp = q->front;
  if (q->front == q->rear)
  {
    q->front = q->rear = NULL;
  }
  else
  {
    q->front = q->front->next;
  }
  *times = tmp->time;
  strcpy(datas, tmp->data);
  free(tmp);
  return true;
}

bool display_lrc(link_queue_p q)
{
  char lrc[200];
  int offset = 0;
  if (queue_empty(q))
  {
    printf("队列为空\n");
    return false;
  }

  data_node_p tmp = q->front;
  while (tmp != NULL)
  {
    font_tools(tmp->data, 460, 235 + offset, 26, 0x00afb1b8, 300, 30, 0x00ffffff);
    //printf("%s\n", tmp->data);
    //printf("%d\n", tmp->time);
    tmp = tmp->next;
    offset += 30;
  }
  return true;
}

void destroy_queue(link_queue_p q)
{
  data_node_p pre = q->front, p = NULL;
  if (pre != NULL)
  {
    p = pre->next;
    while (p != NULL)
    {
      free(pre);
      pre = p;
      p = p->next;
    }
    free(pre);
  }
  free(q);
}
/*
// void lyric(FILE *fp)
// {
//   int i, j;
//   int n = 0;
//   int second = 0;
//   char lrc[200];
//   char buf[200];
//   int m, s;
//   char mm[4];
//   char ss[4];
//   int time = 0;
//   char *ret;
//   bool refresh = false;
//   bool emtpy = false;
//   bool first = true;
//   int offset_q2 = 0;
//   int offset_q1 = 0;
//   link_queue_p q1 = queue_init();
//   link_queue_p q2 = queue_init();

//   while (1)
//   {
//     printf("2222222222222\n");
//     printf("refresh %d first %d \n", refresh, first);
//     if (refresh || first)
//     {
//       printf("111111111111 n=%d\n", n);
//       //q1 读取数据
//       fseek(fp, 0, SEEK_SET);
//       for (i = 0; i < n + 5; i++)
//       {
//         bzero(buf, sizeof(buf));
//         ret = fgets(buf, sizeof(buf), fp);
//         if (i < n + 5 && i >= n)
//         {
//           //printf("i = %d\n", i);
//           bzero(mm, sizeof(mm));
//           bzero(ss, sizeof(ss));
//           bzero(lrc, sizeof(lrc));
//           printf("%s", buf);
//           sscanf(buf, "%*1s%[^:]:%[^.].%*3s%*c%[^\n]", mm, ss, lrc);
//           m = atoi(mm);
//           s = atoi(ss);
//           second = m * 60 + s;
//           en_queue(q1, second, lrc);
//           if (ret == NULL)
//             emtpy = true;
//         }
//         else
//           continue;
//       }
//       //q2读取数据
//       fseek(fp, 0, SEEK_SET);
//       for (j = 0; j < n; j++)
//       {
//         bzero(buf, sizeof(buf));
//         fgets(buf, sizeof(buf), fp);
//         if (n <= 5 && j < n)
//         {
//           bzero(mm, sizeof(mm));
//           bzero(ss, sizeof(ss));
//           bzero(lrc, sizeof(lrc));
//           printf("%s", buf);
//           sscanf(buf, "%*1s%[^:]:%[^.].%*3s%*c%[^\n]", mm, ss, lrc);
//           m = atoi(mm);
//           s = atoi(ss);
//           second = m * 60 + s;
//           en_queue(q2, second, lrc);
//         }
//         else if (n >= 5 && (j < n && j >= n - 5))
//         {
//           bzero(mm, sizeof(mm));
//           bzero(ss, sizeof(ss));
//           bzero(lrc, sizeof(lrc));
//           sscanf(buf, "%*1s%[^:]:%[^.].%*3s%*c%[^\n]", mm, ss, lrc);
//           m = atoi(mm);
//           s = atoi(ss);
//           second = m * 60 + s;
//           en_queue(q2, second, lrc);
//         }
//       }
//     }
//     sleep(1);
//     time++;

//     //初始化
//     if (first)
//     {
//       display_picture("/mnt/sd/Multi-Media-App/img/lrc_bg.bmp", 431, 0, false);
//       display_lrc(q1);
//       first = false;
//     }

//     //是否刷新歌词
//     printf("time=%d\n", time);
//     printf("q1->front->time=%d\n", q1->front->time);
//     if (time == q1->front->time)
//     {
//       int t;
//       offset_q1 = 0;
//       offset_q2 = 0;
//       bool flag = true;
//       display_picture("/mnt/sd/Multi-Media-App/img/lrc_bg.bmp", 431, 0, false);

//       // q2旧歌词
//       if (!queue_empty(q2))
//       {
//         data_node_p tmp = q2->rear;
//         while (tmp != NULL)
//         {
//           font_tools(tmp->data, 460, 205 - offset_q2, 26, 0x00afb1b8, 300, 30, 0x00ffffff);
//           offset_q2 += 30;
//           tmp = tmp->prev;
//         }
//       }

//       //q1新歌词
//       data_node_p tmp2 = q1->front;
//       while (tmp2 != NULL)
//       {
//         if (flag)
//         {
//           font_tools(tmp2->data, 460, 235 + offset_q1, 26, 0x00e84c6d, 300, 30, 0x00ffffff);
//           tmp2 = tmp2->next;
//           offset_q1 += 30;
//           flag = false;
//         }
//         else
//         {
//           font_tools(tmp2->data, 460, 235 + offset_q1, 26, 0x00afb1b8, 300, 30, 0x00ffffff);
//           tmp2 = tmp2->next;
//           offset_q1 += 30;
//         }
//       }
//       //出队刷新
//       while (dequeue(q1, &t, buf))
//         ;
//       while (dequeue(q2, &t, buf))
//         ;
//       n++;
//       refresh = true;
//     }
//     else
//     {
//       refresh = false;
//     }
//     if (emtpy)
//       break;
//   }
//   fclose(fp);
// }
*/