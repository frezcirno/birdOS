#include "type.h"
#include "stdio.h"

static unsigned long next = 1;

PUBLIC int rand(void)
{
    next = next * 1103515245 + 12345;
    return (unsigned int)(next / 65536) % RAND_MAX;
}

PUBLIC void srand(unsigned int seed)
{
    next = seed;
}