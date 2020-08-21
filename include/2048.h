#include "type.h"
#include "stdio.h"

void initData(void);
void morge2048(void);
void printNums2048(void);
int isAlive2048(void);
int canEliminate2048(void);
int zeroNum2048(void);
void addrandom2048(void);
void move2048(void);
void merge2048(void);
PUBLIC void start2048Game(int fd_stdin, int fd_stdout);