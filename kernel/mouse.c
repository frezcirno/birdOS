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

#include "string.h"
#include "buffer.h"
#include "mouse.h"
#include "glib.h"

#define MOUSE_BUF_SIZE 128

int mouseevent = 0;
unsigned char mouse_buf[MOUSE_BUF_SIZE];
FIFO_BUFFER mouse_in;

int mouse_x, mouse_y;
MOUSE_DEC mdec;
// SHEET *mouse_sht;

void wait_KBC_sendready()
{
    /* 等待键盘控制电路准备完毕 */
    while ((in_byte(MOUSE_PORT_CMD) & MOUSE_NOT_READY) != 0)
        ;
}

void init_mouse()
{
    /* 初始化键盘控制电路 */
    wait_KBC_sendready();
    out_byte(MOUSE_PORT_CMD, MOUSE_WRITE_MODE);
    wait_KBC_sendready();
    out_byte(MOUSE_PORT_DATA, KBC_MODE);
    /* 激活鼠标 */
    wait_KBC_sendready();
    out_byte(MOUSE_PORT_CMD, KEYCMD_SENDTO_MOUSE);
    wait_KBC_sendready();
    out_byte(MOUSE_PORT_DATA, MOUSE_CMD_ENABLE);
    /* 顺利的话，键盘控制器会返回ACK(0xfa) */

    mouse_x = scr_x / 2;
    mouse_y = scr_y / 2;
    mdec.phase = 0;
    fifo_init(&mouse_in, MOUSE_BUF_SIZE, mouse_buf);

    // mouse_sht = alloc_sheet();
    // sheet_setbuf(mouse_sht, (unsigned char *)mm_alloc(8 * 16), 8, 16, 255); /* 透明色号255 */
    // fillRectTo(mouse_sht->buf, 8, 0, 0, 8, 16, 255);
    // drawGlyphTo(mouse_sht->buf, 8, 0, 0, cursor, PEN_WHITE);
    // movexy(mouse_sht, mouse_x, mouse_y);
    // movez(mouse_sht, ctl->top + 1);

    put_irq_handler(MOUSE_IRQ, mouse_handler);
	enable_irq(CASCADE_IRQ);
    enable_irq(MOUSE_IRQ);
}

void mouse_handler(unsigned int irq)
{
    fifo_push(&mouse_in, in_byte(MOUSE_PORT_DATA));
    mouseevent=mouse_read();
}

int mouse_decode(unsigned char dat)
{
    switch (mdec.phase)
    {
    case 0:
        /* 等待鼠标的0xfa的阶段 */
        if (dat == 0xfa)
        {
            mdec.phase = 1;
        }
        return 0;
    case 1:
        /* 等待鼠标第一字节的阶段 */
        if ((dat & 0xc8) == 0x08)
        {
            mdec.buf[0] = dat;
            mdec.phase = 2;
        }
        return 0;
    case 2:
        /* 等待鼠标第二字节的阶段 */
        mdec.buf[1] = dat;
        mdec.phase = 3;
        return 0;
    case 3:
        /* 等待鼠标第二字节的阶段 */
        mdec.buf[2] = dat;
        mdec.phase = 1;
        mdec.btn = mdec.buf[0] & 0x07;
        mdec.x = mdec.buf[1];
        mdec.y = mdec.buf[2];
        if ((mdec.buf[0] & 0x10) != 0)
        {
            mdec.x |= 0xffffff00;
        }
        if ((mdec.buf[0] & 0x20) != 0)
        {
            mdec.y |= 0xffffff00;
        }
        /* 鼠标的y方向与画面符号相反 */
        mdec.y = -mdec.y;
        return 1;
    default:
        return -1;
    }
}

int mouse_read()
{
    if (mouse_in.size)
    {
        disable_int();
        int i = fifo_pop(&mouse_in);
        enable_int();
        if (mouse_decode(i) != 0)
        {
            /* 鼠标指针的移动 */
            mouse_x += mdec.x;
            mouse_y += mdec.y;
            if (mouse_x < 0)
                mouse_x = 0;
            if (mouse_y < 0)
                mouse_y = 0;
            if (mouse_x >= scr_x)
                mouse_x = scr_x - 1;
            if (mouse_y >= scr_y)
                mouse_y = scr_y - 1;
            // movexy(mouse_sht, mouse_x, mouse_y);
            return 1;
        }
    }
    return 0;
}