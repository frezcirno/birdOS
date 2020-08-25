#pragma once

#define MOUSE_PORT_DATA 0x60
#define MOUSE_PORT_CMD  0x64

#define MOUSE_CMD_ENABLE 0xf4 // Enable Packet Streaming

#define MOUSE_NOT_READY     0x02
#define MOUSE_WRITE_MODE    0x60
#define KBC_MODE            0x47
#define KEYCMD_SENDTO_MOUSE 0xd4

typedef struct s_mouse_dec
{
    unsigned char buf[3], phase;
    int x, y, btn;
} MOUSE_DEC;

extern int mouse_x, mouse_y;
extern int mouseevent;

void wait_KBC_sendready();
void init_mouse();
void mouse_handler(unsigned int irq);
int mouse_read();