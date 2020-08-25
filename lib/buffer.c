#include <buffer.h>

void fifo_init(FIFO_BUFFER *fifo, int size, unsigned char *buf)
{
    fifo->buf      = buf;
    fifo->p_head   = buf;
    fifo->p_tail   = buf;
    fifo->capacity = size;
    fifo->size     = 0;
    fifo->flags    = 0;
}

int fifo_push(FIFO_BUFFER *fifo, unsigned char data)
{
    if (fifo->size >= fifo->capacity)
    {
        fifo->flags |= FIFO_OVERFLOW;
        return -1;
    }

    *fifo->p_head++ = data;
    if (fifo->p_head == fifo->buf + fifo->capacity)
    {
        fifo->p_head = fifo->buf;
    }
    fifo->size++;
    return 0;
}

int fifo_pop(FIFO_BUFFER *fifo)
{
    if (fifo->size == 0) return -1;
    int data = *fifo->p_tail++;
    if (fifo->p_tail == fifo->buf + fifo->capacity) fifo->p_tail = fifo->buf;
    fifo->size--;
    return data;
}