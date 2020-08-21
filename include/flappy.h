#pragma once
#include "type.h"

struct bar
{
    int bar_x;               //挡板坐标
    int bar_yTop, bar_yDown; //挡板开口上下坐标
    int bar_dist;            //挡板上下开口距离
};

void showScreen();
void addScreen(unsigned char *Img, int w, int h, int _x, int _y);
void showbg();   //背景整体左移，最后一列由新的背景替代
void showpipe(); // 在背景上增添柱子
void showbird(); // 在背景上增添小鸟
void openImg(char *path, unsigned char *Img, int w, int h);
struct bar newbar();
void startup();                                     //数据初始化
void show();                                        //显示界面
void updateWithoutInput();                          //与用户输入无关的更新
void updateWithInpute(int fd_stdin, int fd_stdout); //与用户输入有关的更新
PUBLIC void waitkey();
PUBLIC void startflappyBird(int fd_stdin, int fd_stdout);