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
#include "2048.h"
#include "glib.h"
#include "math.h"

int n = 11;
int Label[11][11]={{}};
int bomb[11][11]={{}};
int num[13][13]={{}};//指示周围雷的数量的数组
int jiemian[11][11]={{}};//11*11游戏界面
char str[11]={};
float lastangle[3] = {0, 0, 0};
int time[3] = {1, 25, 30};

PUBLIC void start2048Game(int fd_stdin, int fd_stdout)
{
	for(int i=0;i<11;i++){
		for(int j=0;j<11;j++){
			jiemian[i][j]=-1;
		}
	}
	clear();
	printf("\n\n\n");
	printf("                          1    2    3    4    5    6    7    8    9    10   11");
	for(int i=0;i<11;i++){
		printf("\n\n");
		switch(i){
			case 0:
			printf("                       1  ");
			break;
			case 1:
			printf("                       2  ");
			break;
			case 2:
			printf("                       3  ");
			break;
			case 3:
			printf("                       4  ");
			break;
			case 4:
			printf("                       5  ");
			break;
			case 5:
			printf("                       6  ");
			break;
			case 6:
			printf("                       7  ");
			break;
			case 7:
			printf("                       8  ");
			break;
			case 8:
			printf("                       9  ");
			break;
			case 9:
			printf("                       10 ");
			break;
			case 10:
			printf("                       11 ");
			break;
		}
		
		for(int j=0;j<11;j++){
			if(jiemian[i][j]==-1){
				printf("x    ");
			}
		}
		
	}
	drawLine(10,10,70,70,PEN_LIGHT_YELLOW);
	while(1){}
}


