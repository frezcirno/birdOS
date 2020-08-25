#pragma once

#define FIFO_OVERFLOW 0x1

// 循环队列缓冲区,
typedef struct s_fifo_buffer
{
    unsigned char *buf;
    unsigned char *p_head;
    unsigned char *p_tail;
    int capacity, size, flags;
} FIFO_BUFFER;

void fifo_init(FIFO_BUFFER *fifo, int size, unsigned char *buf);
int fifo_push(FIFO_BUFFER *fifo, unsigned char data);
int fifo_pop(FIFO_BUFFER *fifo);