#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"
#include "glib.h"
#include "math.h"

int clockx = 400;
int clocky = 300;
int clockw = 400;
int clockh = 400;

int clockr = 200;
int textr = 185;
int secr = 175;
int minr = 170;
int hourr = 130;

void drawArrow(float angle, int length, int color);

void clock(int fd_stdin, int fd_stdout)
{
    fillRect(0, 0, scr_x, scr_y, PEN_BLACK);
    drawCircle(clockx, clocky, clockr, PEN_BLUE);
    drawCircle(clockx, clocky, clockr - 2, PEN_BLUE);
    for (int i = 0; i < 12; i++)
    {
        int vx = sin(30 * i * PI / 180) * textr;
        int vy = -cos(30 * i * PI / 180) * textr;
        int x = clockx + vx;
        int y = clocky + vy;
        char text[10];
        sprintf(text, "%2d", i ? i : 12);
        drawText(x - 8, y - 8, text, PEN_WHITE);
    }
    int quit = 0;
    int now, last = 0;
    int time[3] = {1, 25, 30};
    float lastangle[3] = {0, 0, 0};
    int mode = 2;
    char cmd[20];
    read(fd_stdin, cmd, 10);
    while (!quit)
    {
        if (key_pressed)
        {
            read(fd_stdin, cmd, 1);
            switch (cmd[0])
            {
            case 'q':
                quit = 1;
                break;
            case 'h':
                mode = 0;
                break;
            case 'm':
                mode = 1;
                break;
            case 's':
                mode = 2;
                break;
            case '+':
                time[mode]++;
                break;
            case '-':
                time[mode]--;
                break;
            default:
                break;
            }
        }
        int now = get_ticks();
        if (now - last >= HZ)
        {
            last = now;
            time[2]++;
            if (time[2] >= 60)
            {
                time[2] -= 60;
                time[1]++;
            }
            if (time[1] >= 60)
            {
                time[1] -= 60;
                time[0]++;
            }
            if (time[0] >= 24)
            {
                time[0] -= 24;
            }
            drawArrow(lastangle[2], secr, PEN_BLACK);
            drawArrow(lastangle[1], minr, PEN_BLACK);
            drawArrow(lastangle[0], hourr, PEN_BLACK);
            drawArrow(lastangle[2] = time[2] * 2 * PI / 60, secr, PEN_DARK_GREEN);
            drawArrow(lastangle[1] = time[1] * 2 * PI / 60, minr, PEN_LIGHT_YELLOW);
            drawArrow(lastangle[0] = time[0] * 2 * PI / 24, hourr, PEN_DARK_RED);
            char text[10];
            sprintf(text, "%02d:%02d:%02d", time[0], time[1], time[2]);
            int textx = clockx;
            int texty = clocky + clockh / 2;
            int ltx = textx - 8 * 8 / 2;
            int lty = texty - 8;
            fillRect(ltx, lty, ltx + 8 * 8, lty + 16, PEN_BLACK);
            drawText(ltx, lty, text, PEN_WHITE);
            drawCircle(clockx, clocky, 1, PEN_RED);
            drawCircle(clockx, clocky, 2, PEN_RED);
        }
    }
}

void drawArrow(float angle, int length, int color)
{
    int vx = sin(angle) * length;
    int vy = -cos(angle) * length;
    drawLine(clockx, clocky, clockx + vx, clocky + vy, color);
}