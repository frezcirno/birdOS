#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "fs.h"
#include "tty.h"
#include "console.h"
#include "proc.h"
#include "global.h"
#include "proto.h"
#include "glib.h"
#include "BGA.h"
#include "font.h"
#include "string.h"
#include "memory.h"

unsigned char *vram;
int scr_x;
int scr_y;
int scr_bpp;
int scr_pitch;

int cur_x;
int cur_y;

int fontsmap[65536];

#define abs(x) ((x) > 0 ? (x) : (-(x)))

SHTCTL *ctl;

void init_video()
{
    scr_x = 800;
    scr_y = 600;
    scr_bpp = 8;
    scr_pitch = scr_x * scr_bpp / 8;

    bga_set_video_mode(scr_x, scr_y, scr_bpp, 1, 1);

    vram = (unsigned char *)bga_get_lfb_addr();

    init_descriptor(&gdt[SELECTOR_VIDEO], (unsigned int)vram, scr_x * scr_y,
                    DA_DRW | DA_DPL3);

    initPalette();
    cacheFonts();

    init_sheets(scr_x, scr_y);
}

void initPalette()
{
    static const unsigned char rgb[48] = {
        0x00, 0x00, 0x00, /*  0:黑 */
        0xff, 0x00, 0x00, /*  1:梁红 */
        0x00, 0xff, 0x00, /*  2:亮绿 */
        0xff, 0xff, 0x00, /*  3:亮黄 */
        0x00, 0x00, 0xff, /*  4:亮蓝 */
        0xff, 0x00, 0xff, /*  5:亮紫 */
        0x00, 0xff, 0xff, /*  6:浅亮蓝 */
        0xff, 0xff, 0xff, /*  7:白 */
        0xc6, 0xc6, 0xc6, /*  8:亮灰 */
        0x84, 0x00, 0x00, /*  9:暗红 */
        0x00, 0x84, 0x00, /* 10:暗绿 */
        0x84, 0x84, 0x00, /* 11:暗黄 */
        0x00, 0x00, 0x84, /* 12:暗青 */
        0x84, 0x00, 0x84, /* 13:暗紫 */
        0x00, 0x84, 0x84, /* 14:浅暗蓝 */
        0x84, 0x84, 0x84  /* 15:暗灰 */
    };
    setPalette(0, 15, rgb);
}

void setPalette(int start, int end, const unsigned char *palette)
{
    unsigned int eflags = load_eflags();
    disable_int();
    out_byte(0x03c8, start);
    for (int i = start; i <= end; i++)
    {
        out_byte(0x03c9, palette[0] / 4);
        out_byte(0x03c9, palette[1] / 4);
        out_byte(0x03c9, palette[2] / 4);
        palette += 3;
    }
    store_eflags(eflags);
}

void cacheFonts()
{
    for (unsigned int i = 0; i < fonts.Chars; i++)
    {
        unsigned short index = fonts.Index[i];
        fontsmap[index] = i;
    }
}

void putPixelTo(unsigned char *dst, int pitch, int x, int y, int color)
{
    dst[y * pitch + x] = color;
}

void putPixel(int x, int y, int color)
{
    unsigned int base = y * scr_pitch + x;
    vram[base] = color;
}

void drawLineTo(unsigned char *dst, int pitch, int x0, int y0, int x1, int y1,
                int color)
{
    // Bresenhamline算法
    int dx = x1 - x0;           // x偏移量
    int dy = y1 - y0;           // y偏移量
    int ux = (dx > 0 ? 1 : -1); // x伸展方向
    int uy = (dy > 0 ? 1 : -1); // y伸展方向
    int dx2 = dx << 1;          // x偏移量乘2
    int dy2 = dy << 1;          // y偏移量乘2
    if (abs(dx) > abs(dy))
    {                                     //以x为增量方向计算
        int e = -dx;                      // e = -0.5 * 2 * dx,把e 用2 * dx* e替换
        int y = y0;                       //起点y坐标
        for (int x = x0; x < x1; x += ux) //起点x坐标
        {
            putPixelTo(dst, pitch, x, y, color);
            e = e + dy2; //来自 2*e*dx= 2*e*dx + 2dy  （原来是 e = e + k）
            if (e > 0)   // e是整数且大于0时表示要取右上的点（否则是右下的点）
            {
                y += uy;
                e = e - dx2; // 2*e*dx = 2*e*dx - 2*dx  (原来是 e = e -1)
            }
        }
    }
    else
    {                                     //以y为增量方向计算
        int e = -dy;                      // e = -0.5 * 2 * dy,把e 用2 * dy* e替换
        int x = x0;                       //起点x坐标
        for (int y = y0; y < y1; y += uy) //起点y坐标
        {
            putPixelTo(dst, pitch, x, y, color);
            e = e + dx2; //来自 2*e*dy= 2*e*dy + 2dy  （原来是 e = e + k）
            if (e > 0)   // e是整数且大于0时表示要取右上的点（否则是右下的点）
            {
                x += ux;
                e = e - dy2; // 2*e*dy = 2*e*dy - 2*dy  (原来是 e = e -1)
            }
        }
    }
}

void drawLine(int x0, int y0, int x1, int y1, int color)
{
    // Bresenhamline算法
    int dx = x1 - x0;           // x偏移量
    int dy = y1 - y0;           // y偏移量
    int ux = (dx > 0 ? 1 : -1); // x伸展方向
    int uy = (dy > 0 ? 1 : -1); // y伸展方向
    int dx2 = dx << 1;          // x偏移量乘2
    int dy2 = dy << 1;          // y偏移量乘2
    if (abs(dx) > abs(dy))
    {                                     //以x为增量方向计算
        int e = -dx;                      // e = -0.5 * 2 * dx,把e 用2 * dx* e替换
        int y = y0;                       //起点y坐标
        for (int x = x0; x < x1; x += ux) //起点x坐标
        {
            putPixel(x, y, color);
            e = e + dy2; //来自 2*e*dx= 2*e*dx + 2dy  （原来是 e = e + k）
            if (e > 0)   // e是整数且大于0时表示要取右上的点（否则是右下的点）
            {
                y += uy;
                e = e - dx2; // 2*e*dx = 2*e*dx - 2*dx  (原来是 e = e -1)
            }
        }
    }
    else
    {                                     //以y为增量方向计算
        int e = -dy;                      // e = -0.5 * 2 * dy,把e 用2 * dy* e替换
        int x = x0;                       //起点x坐标
        for (int y = y0; y < y1; y += uy) //起点y坐标
        {
            putPixel(x, y, color);
            e = e + dx2; //来自 2*e*dy= 2*e*dy + 2dy  （原来是 e = e + k）
            if (e > 0)   // e是整数且大于0时表示要取右上的点（否则是右下的点）
            {
                x += ux;
                e = e - dy2; // 2*e*dy = 2*e*dy - 2*dy  (原来是 e = e -1)
            }
        }
    }
}

void drawRectTo(unsigned char *dst, int pitch, int x1, int y1, int x2, int y2,
                int color)
{
    for (int y = y1; y < y2; y++)
    {
        putPixelTo(dst, pitch, x1, y, color);
        putPixelTo(dst, pitch, x2, y, color);
    }
    for (int x = x1; x < x2; x++)
    {
        putPixelTo(dst, pitch, x, y1, color);
        putPixelTo(dst, pitch, x, y2, color);
    }
}

void drawRect(int x1, int y1, int x2, int y2, int color)
{
    for (int y = y1; y < y2; y++)
    {
        putPixel(x1, y, color);
        putPixel(x2, y, color);
    }
    for (int x = x1; x < x2; x++)
    {
        putPixel(x, y1, color);
        putPixel(x, y2, color);
    }
}

void fillRectTo(unsigned char *dst, int pitch, int x1, int y1, int x2, int y2,
                int color)
{
    for (int y = y1; y < y2; y++)
    {
        for (int x = x1; x < x2; x++)
        {
            putPixelTo(dst, pitch, x, y, color);
        }
    }
}

void fillRect(int x1, int y1, int x2, int y2, int color)
{
    for (int y = y1; y < y2; y++)
    {
        for (int x = x1; x < x2; x++)
        {
            putPixel(x, y, color);
        }
    }
}

void drawGlyphTo(unsigned char *dst, int pitch, int x, int y,
                 const unsigned char *glyph, int color)
{
    unsigned char *base = dst + y * pitch + x;
    for (int in_y = 0; in_y < 16; in_y++)
    {
        unsigned char line = glyph[in_y];
        if (line & 0x80)
            base[0] = color;
        if (line & 0x40)
            base[1] = color;
        if (line & 0x20)
            base[2] = color;
        if (line & 0x10)
            base[3] = color;
        if (line & 0x8)
            base[4] = color;
        if (line & 0x4)
            base[5] = color;
        if (line & 0x2)
            base[6] = color;
        if (line & 0x1)
            base[7] = color;
        base += pitch;
    }
}

void drawGlyph(int x, int y, const unsigned char *glyph, int color)
{
    for (int in_y = 0; in_y < fonts.Height; in_y++)
    {
        unsigned char line = glyph[in_y];
        int offy = y + in_y;
        if (line & 0x80)
            putPixel(x + 0, offy, color);
        if (line & 0x40)
            putPixel(x + 1, offy, color);
        if (line & 0x20)
            putPixel(x + 2, offy, color);
        if (line & 0x10)
            putPixel(x + 3, offy, color);
        if (line & 0x8)
            putPixel(x + 4, offy, color);
        if (line & 0x4)
            putPixel(x + 5, offy, color);
        if (line & 0x2)
            putPixel(x + 6, offy, color);
        if (line & 0x1)
            putPixel(x + 7, offy, color);
    }
}

void drawCharTo(unsigned char *dst, int pitch, int x, int y, char ch, int color)
{
    const unsigned char *font_data = &fonts.Bitmap[16 * fontsmap[ch]];
    drawGlyphTo(dst, pitch, x, y, font_data, color);
}

void drawChar(int x, int y, char ch, int color)
{
    const unsigned char *font_data = &fonts.Bitmap[16 * fontsmap[ch]];
    drawGlyph(x, y, font_data, color);
}

void drawTextToClr(unsigned char *dst, int pitch, int x, int y, const char *str,
                   int color, int back)
{
    fillRectTo(dst, pitch, x, y, x + strlen(str) * fonts.Width, fonts.Height,
               back);
    drawTextTo(dst, pitch, x, y, str, color);
}

void drawTextTo(unsigned char *dst, int pitch, int x, int y, const char *str,
                int color)
{
    while (*str)
    {
        drawCharTo(dst, pitch, x, y, *str++, color);
        x += 8;
    }
}

void drawText(int x, int y, const char *str, int color)
{
    while (*str)
    {
        drawChar(x, y, *str++, color);
        x += 8;
    }
}

void gotoxy(int x, int y)
{
    cur_x = x;
    cur_y = y;
}

void putchar(char ch, int color)
{
    if (ch == '\n')
    {
        fillRect(cur_x, cur_y, scr_x, cur_y + 16, PEN_BLACK);
        cur_x = 0;
        cur_y += fonts.Height;
    }
    else if (ch == '\t')
    {
        fillRect(cur_x, cur_y, cur_x + 32, cur_y + 16, PEN_BLACK);
        cur_x += 4 * fonts.Width;
    }
    else
    {
        fillRect(cur_x, cur_y, cur_x + 8, cur_y + 16, PEN_BLACK);
        drawChar(cur_x, cur_y, ch, color);
        cur_x += fonts.Width;
    }
    if (cur_x >= scr_x)
    {
        cur_x = 0;
        cur_y += fonts.Height;
    }
    if (cur_y + fonts.Height >= scr_y)
    {
        cur_y = 0;
    }
}

void printstr(const char *str, int color)
{
    while (*str)
    {
        putchar(*str++, color);
    }
}

void drawTextboxTo(unsigned char *buf, int pitch, int x0, int y0, int x1,
                   int y1, int c)
{
    drawLineTo(buf, pitch, x0 - 2, y0 - 3, x1 + 1, y0 - 3, PEN_DARK_CLAN);  // top
    drawLineTo(buf, pitch, x0 - 3, y0 - 3, x0 - 3, y1 + 1, PEN_DARK_CLAN);  // left
    drawLineTo(buf, pitch, x0 - 3, y1 + 2, x1 + 1, y1 + 2, PEN_WHITE);      // bottom
    drawLineTo(buf, pitch, x1 + 2, y0 - 3, x1 + 2, y1 + 2, PEN_WHITE);      // right
    drawLineTo(buf, pitch, x0 - 1, y0 - 2, x1 + 0, y0 - 2, PEN_BLACK);      // top2
    drawLineTo(buf, pitch, x0 - 2, y0 - 2, x0 - 2, y1 + 0, PEN_BLACK);      // left2
    drawLineTo(buf, pitch, x0 - 2, y1 + 1, x1 + 0, y1 + 1, PEN_LIGHT_GRAY); // btm2
    drawLineTo(buf, pitch, x1 + 1, y0 - 2, x1 + 1, y1 + 1, PEN_LIGHT_GRAY); // right2
    fillRectTo(buf, pitch, x0 - 1, y0 - 1, x1 + 0, y1 + 0, c);              // border
}

void drawWindowTo(unsigned char *buf, int pitch, int xsize, int ysize,
                  const char *title)
{
    static char closebtn[14][16] = {
        "OOOOOOOOOOOOOOO@", "OQQQQQQQQQQQQQ$@", "OQQQQQQQQQQQQQ$@",
        "OQQQ@@QQQQ@@QQ$@", "OQQQQ@@QQ@@QQQ$@", "OQQQQQ@@@@QQQQ$@",
        "OQQQQQQ@@QQQQQ$@", "OQQQQQ@@@@QQQQ$@", "OQQQQ@@QQ@@QQQ$@",
        "OQQQ@@QQQQ@@QQ$@", "OQQQQQQQQQQQQQ$@", "OQQQQQQQQQQQQQ$@",
        "O$$$$$$$$$$$$$$@", "@@@@@@@@@@@@@@@@"};
    int x, y;
    drawLineTo(buf, pitch, 0, 0, xsize, 0, PEN_LIGHT_GRAY);            // top
    drawLineTo(buf, pitch, 1, 1, xsize - 1, 1, PEN_WHITE);             // top2
    drawLineTo(buf, pitch, 0, 0, 0, ysize, PEN_LIGHT_GRAY);            // left
    drawLineTo(buf, pitch, 1, 1, 1, ysize - 1, PEN_WHITE);             // left2
    drawLineTo(buf, pitch, xsize - 1, 0, xsize - 1, ysize, PEN_BLACK); // right
    drawLineTo(buf, pitch, xsize - 2, 1, xsize - 2, ysize - 1,
               PEN_DARK_GRAY); // right2
    drawLineTo(buf, pitch, 0, ysize - 1, xsize, ysize - 1,
               PEN_BLACK); // bottom
    drawLineTo(buf, pitch, 1, ysize - 2, xsize - 1, ysize - 2,
               PEN_DARK_GRAY);                                          // bottom2
    fillRectTo(buf, pitch, 2, 2, xsize - 2, ysize - 2, PEN_LIGHT_GRAY); // body
    fillRectTo(buf, pitch, 3, 3, xsize - 3, 20, PEN_DARK_CLAN);         // header
    drawTextTo(buf, pitch, 24, 4, title, PEN_WHITE);

    fillRectTo(buf, pitch, xsize - 19, 5, xsize - 5, 18, PEN_LIGHT_GRAY);

    for (y = 0; y < 14; y++)
    {
        for (x = 0; x < 16; x++)
        {
            char c = closebtn[y][x];
            if (c == '@')
                c = PEN_BLACK;
            else if (c == '$')
                c = PEN_DARK_GRAY;
            else if (c == 'Q')
                c = PEN_LIGHT_GRAY;
            else
                c = PEN_WHITE;
            putPixelTo(buf, pitch, xsize + x - 20, y + 5, c);
        }
    }
}

int init_sheets(int x_size, int y_size)
{
    ctl = (SHTCTL *)mm_alloc_4k(sizeof(SHTCTL));
    if (ctl == 0)
    {
        return -1;
    }
    ctl->map = (unsigned char *)mm_alloc_4k(x_size * y_size);
    if (ctl->map == 0)
    {
        mm_free_4k(ctl, sizeof(MEMMAN));
        return -1;
    }
    ctl->xsize = x_size;
    ctl->ysize = y_size;
    ctl->top = -1;
    for (int i = 0; i < MAX_SHEETS; i++)
    {
        ctl->_sheets[i].flags = 0;
    }
    return 0;
}

SHEET *alloc_sheet()
{
    for (int i = 0; i < MAX_SHEETS; i++)
    {
        if (ctl->_sheets[i].flags == 0)
        {
            SHEET *sht = &ctl->_sheets[i];
            sht->flags = SHEET_IN_USE; /* 标记为正在使用*/
            sht->height = -1;          /* 隐藏 */
            return sht;
        }
    }
    return 0; /* 所有的SHEET都处于正在使用状态*/
}

void free_sheet(SHEET *sht)
{
    if (sht->height >= 0)
    {
        movez(sht, -1); /* 如果处于显示状态，则先设定为隐藏 */
    }
    sht->flags = 0; /* "未使用"标志 */
}

void sheet_setbuf(SHEET *sht, unsigned char *buf, int xsize, int ysize,
                  int col_inv)
{
    sht->buf = buf;
    sht->bxsize = xsize;
    sht->bysize = ysize;
    sht->col_inv = col_inv;
}

void movez(SHEET *sht, int height)
{
    int old = sht->height;
    // 边界处理
    if (height > ctl->top + 1)
        height = ctl->top + 1;
    if (height < -1)
        height = -1;      // -1隐藏
    sht->height = height; // 设定高度

    /* 下面主要是进行sheets[ ]的重新排列 */
    if (old > height)
    { /* 比以前低 */
        if (height >= 0)
        {
            /* 把中间的往上提 */
            for (int h = old; h > height; h--)
            {
                ctl->sheets[h] = ctl->sheets[h - 1];
                ctl->sheets[h]->height = h;
            }
            ctl->sheets[height] = sht;
        }
        else
        { /* 隐藏 */
            if (ctl->top > old)
            {
                /* 把上面的降下来 */
                for (int h = old; h < ctl->top; h++)
                {
                    ctl->sheets[h] = ctl->sheets[h + 1];
                    ctl->sheets[h]->height = h;
                }
            }
            ctl->top--; /* 由于显示中的图层减少了一个，所以最上面的图层高度下降 */
        }
        refresh_map(sht->vx0, sht->vy0, sht->vx0 + sht->bxsize,
                    sht->vy0 + sht->bysize, height);
        refresh(sht->vx0, sht->vy0, sht->vx0 + sht->bxsize,
                sht->vy0 + sht->bysize, height, height);
    }
    else if (old < height)
    { /* 比以前高 */
        if (old >= 0)
        {
            /* 把中间的拉下去 */
            for (int h = old; h < height; h++)
            {
                ctl->sheets[h] = ctl->sheets[h + 1];
                ctl->sheets[h]->height = h;
            }
            ctl->sheets[height] = sht;
        }
        else
        { /* 由隐藏状态转为显示状态 */
            /* 将已在上面的提上来 */
            for (int h = ctl->top; h >= height; h--)
            {
                ctl->sheets[h + 1] = ctl->sheets[h];
                ctl->sheets[h + 1]->height = h + 1;
            }
            ctl->sheets[height] = sht;
            ctl->top++; /* 由于已显示的图层增加了1个，所以最上面的图层高度增加 */
        }
        refresh_map(sht->vx0, sht->vy0, sht->vx0 + sht->bxsize,
                    sht->vy0 + sht->bysize, height);
        refresh(sht->vx0, sht->vy0, sht->vx0 + sht->bxsize,
                sht->vy0 + sht->bysize, height, height);
    }
}

void refresh_local(SHEET *sht, int bx0, int by0, int bx1, int by1)
{
    if (sht->height >= 0)
    { /* 如果正在显示，则按新图层的信息刷新画面*/
        refresh_map(sht->vx0 + bx0, sht->vy0 + by0, sht->vx0 + bx1,
                    sht->vy0 + by1, sht->height);
        refresh(sht->vx0 + bx0, sht->vy0 + by0, sht->vx0 + bx1, sht->vy0 + by1,
                sht->height, sht->height);
    }
}

// 预计算一遍所有像素属于的图层
void refresh_map(int vx0, int vy0, int vx1, int vy1, int h0)
{
    /* 如果refresh的范围超出了画面则修正 */
    if (vx0 < 0)
        vx0 = 0;
    if (vy0 < 0)
        vy0 = 0;
    if (vx1 > ctl->xsize)
        vx1 = ctl->xsize;
    if (vy1 > ctl->ysize)
        vy1 = ctl->ysize;

    for (int h = h0; h <= ctl->top; h++)
    {
        SHEET *sht = ctl->sheets[h];
        int sid = (int)(sht - ctl->_sheets);
        unsigned char *buf = sht->buf;
        unsigned char *map = ctl->map;

        /* 计算相对图层的disable_intpbox */
        int bx0 = vx0 - sht->vx0;
        int bx1 = vx1 - sht->vx0;
        int by0 = vy0 - sht->vy0;
        int by1 = vy1 - sht->vy0;
        if (bx0 < 0)
            bx0 = 0;
        if (by0 < 0)
            by0 = 0;
        if (bx1 > sht->bxsize)
            bx1 = sht->bxsize;
        if (by1 > sht->bysize)
            by1 = sht->bysize;

        for (int by = by0; by < by1; by++)
        {
            int vy = (sht->vy0 + by) * ctl->xsize + sht->vx0;
            int x_base = by * sht->bxsize;
            for (int bx = bx0; bx < bx1; bx++)
            {
                if (buf[x_base + bx] != sht->col_inv)
                {
                    map[vy + bx] = sid;
                }
            }
        }
    }
}

// 根据map的信息绘制像素
void refresh(int vx0, int vy0, int vx1, int vy1, int h0, int h1)
{
    /* 如果refresh的范围超出了画面则修正 */
    if (vx0 < 0)
        vx0 = 0;
    if (vy0 < 0)
        vy0 = 0;
    if (vx1 > ctl->xsize)
        vx1 = ctl->xsize;
    if (vy1 > ctl->ysize)
        vy1 = ctl->ysize;
    if (h1 > ctl->top)
        h1 = ctl->top;

    for (int h = h0; h <= h1; h++)
    {
        SHEET *sht = ctl->sheets[h];
        int sid = (int)(sht - ctl->_sheets);
        unsigned char *buf = sht->buf;
        unsigned char *map = ctl->map;

        /* 计算相对disable_intpbox */
        int bx0 = vx0 - sht->vx0;
        int bx1 = vx1 - sht->vx0;
        int by0 = vy0 - sht->vy0;
        int by1 = vy1 - sht->vy0;
        if (bx0 < 0)
            bx0 = 0;
        if (by0 < 0)
            by0 = 0;
        if (bx1 > sht->bxsize)
            bx1 = sht->bxsize;
        if (by1 > sht->bysize)
            by1 = sht->bysize;

        for (int by = by0; by < by1; by++)
        {
            int vy = sht->vy0 + by;
            int m_base = vy * ctl->xsize;
            int c_base = by * sht->bxsize;
            for (int bx = bx0; bx < bx1; bx++)
            {
                int vx = sht->vx0 + bx;
                if (map[m_base + vx] == sid)
                {
                    putPixel(vx, vy, buf[c_base + bx]);
                }
            }
        }
    }
}

void movexy(SHEET *sht, int vx0, int vy0)
{
    int old_vx0 = sht->vx0, old_vy0 = sht->vy0;
    sht->vx0 = vx0;
    sht->vy0 = vy0;
    if (sht->height >= 0)
    {
        // 旧的区域全部重新判断
        refresh_map(old_vx0, old_vy0, old_vx0 + sht->bxsize,
                    old_vy0 + sht->bysize, 0);
        // 新的区域只需要从当前层-1开始判断
        refresh_map(vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize, sht->height);
        // 重绘旧的区域
        refresh(old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize,
                0, sht->height - 1);
        // 新的区域只需要绘制新增的部分
        refresh(vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize, sht->height,
                sht->height);
    }
}

void drawCircleTo(unsigned char *buf, int pitch, int xc, int yc, int r, int color)
{
    // Bresenham画圆算法
    int x = 0, y = r, d = 3 - 2 * r;
    while (1)
    {
        putPixelTo(buf, pitch, xc + x, yc + y, color);
        putPixelTo(buf, pitch, xc + y, yc + x, color);
        putPixelTo(buf, pitch, xc + x, yc - y, color);
        putPixelTo(buf, pitch, xc + y, yc - x, color);
        putPixelTo(buf, pitch, xc - x, yc + y, color);
        putPixelTo(buf, pitch, xc - y, yc + x, color);
        putPixelTo(buf, pitch, xc - x, yc - y, color);
        putPixelTo(buf, pitch, xc - y, yc - x, color);
        if (x >= y)
            break;
        if (d < 0)
        {
            d = d + 4 * x + 6;
        }
        else
        {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}
