#pragma once
#include "font.h"
#include "BGA.h"

#define MAX_SHEETS   256
#define SHEET_IN_USE 1

typedef struct s_sheet
{
    unsigned char *buf;
    int bxsize, bysize; // 图层大小
    int vx0, vy0;       // 图层位置
    int col_inv;        // 透明色色号
    int height;         // 图层高度
    int flags;          // 图层设定
} SHEET;

typedef struct s_shtctl
{
    unsigned char *map; // 大小等于xsize*ysize, 用来表示每个像素属于哪个图层
    SHEET *sheets[MAX_SHEETS];
    int xsize, ysize, top;
    SHEET _sheets[MAX_SHEETS];
} SHTCTL;

extern int scr_x;
extern int scr_y;
extern int scr_bpp;

extern SHTCTL *ctl;

#define PEN_BLACK        0  //黑
#define PEN_RED          1  //梁红
#define PEN_LIGHT_GREEN  2  //亮绿
#define PEN_LIGHT_YELLOW 3  //亮黄
#define PEN_LIGHT_BLUE   4  //亮蓝
#define PEN_LIGHT_PURPLE 5  //亮紫
#define PEN_BLUE         6  //浅亮蓝
#define PEN_WHITE        7  //白
#define PEN_LIGHT_GRAY   8  //亮灰
#define PEN_DARK_RED     9  //暗红
#define PEN_DARK_GREEN   10 //暗绿
#define PEN_DARK_YELLOW  11 //暗黄
#define PEN_DARK_CLAN    12 //暗青
#define PEN_DARK_PURPLE  13 //暗紫
#define PEN_DARK_BLUE    14 //浅暗蓝
#define PEN_DARK_GRAY    15 //暗灰

typedef struct s_canvas
{
    unsigned char *buf;
    int pitch;
} CANVAS;

typedef struct s_rect
{
    int x, y, w, h;
} RECT;

extern const unsigned char cursor[16];

void initPalette();

void cacheFonts();

void init_video();

void drawTextboxTo(unsigned char *buf, int pitch, int x0, int y0, int x1,
                   int y1, int c);

void setPalette(int start, int end, const unsigned char *palette);

void putPixelTo(unsigned char *dst, int pitch, int x, int y, int color);

void putPixel(int x, int y, int color);

void drawTextToClr(unsigned char *dst, int pitch, int x, int y, const char *str,
                   int color, int back);

void drawCircleTo(unsigned char *buf, int pitch, int xc, int yc, int r,
                  int color);

void drawRectTo(unsigned char *dst, int pitch, int x1, int y1, int x2, int y2,
                int color);

void drawRect(int x1, int y1, int x2, int y2, int color);

void fillRectTo(unsigned char *dst, int pitch, int x1, int y1, int x2, int y2,
                int color);

void fillRect(int x1, int y1, int x2, int y2, int color);

void drawLineTo(unsigned char *dst, int pitch, int x0, int y0, int x1, int y1,
                int color);

void drawLine(int x0, int y0, int x1, int y1, int color);

void drawGlyphTo(unsigned char *dst, int pitch, int x, int y,
                 const unsigned char *glyph, int color);

void drawGlyph(int x, int y, const unsigned char *glyph, int color);

void drawCharTo(unsigned char *dst, int pitch, int x, int y, char ch, int color);

void drawChar(int x, int y, char ch, int color);

void drawTextTo(unsigned char *dst, int pitch, int x, int y, const char *str,
                int color);

void drawText(int x, int y, const char *str, int color);

// 依次输出
void gotoxy(int x, int y);

void putchar(char ch, int color);

void printstr(const char *str, int color);

// 图层控制, 在init_video中调用
int init_sheets(int x_size, int y_size);

// 分配新的图层
SHEET *alloc_sheet();

// 设定图层信息, height==-1表示隐藏
void sheet_setbuf(SHEET *sht, unsigned char *buf, int xsize, int ysize,
                  int col_inv);

// 图层水平移动
void movexy(SHEET *sht, int vx0, int vy0);

// 设置图层高度
void movez(SHEET *sht, int height);

// 刷新图层sht内部clipbox的屏幕区域
void refresh_local(SHEET *sht, int bx0, int by0, int bx1, int by1);

// 预刷新
void refresh_map(int vx0, int vy0, int vx1, int vy1, int h0);

// 刷新屏幕clipbox区域的所有图层
void refresh(int vx0, int vy0, int vx1, int vy1, int h0, int h1);

// 释放图层
void free_sheet(SHEET *sht);

void drawWindowTo(unsigned char *buf, int pitch, int xsize, int ysize,
                  const char *title);
