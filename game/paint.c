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

int currentColor = PEN_BLACK;
enum
{
    LINE,
    RECT,
    CIRCLE
} currentShape;

void paint(int fd_stdin, int fd_stdout)
{
    fillRect(0, 0, scr_x, scr_y, PEN_WHITE);

    while (!quit)
    {
    }
}