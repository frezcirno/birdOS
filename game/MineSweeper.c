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

int n = 11;
int Label[11][11]={{}};
int bomb[11][11]={{}};
int num[13][13]={{}};//指示周围雷的数量的数组
int jiemian[13][13]={{}};//13*13界面,中间的11*11有效
char str[11]={};
float lastangle[3] = {0, 0, 0};
int time[3] = {1, 25, 30};
char optionx[2] = "";
char optiony[2] = "";
int x;
int y;
int count=10;//雷的数量

PUBLIC void startMineSweeper(int fd_stdin, int fd_stdout)
{
	srand(get_ticks());
	for(int i=0;i<11;i++){
		for(int j=0;j<11;j++){
			bomb[i][j]=0;
		}
	}
	
	//初始化打印界面
	for(int i=0;i<13;i++){
		for(int j=0;j<13;j++){
			jiemian[i][j]=-1;
		}
	}
	
	
	//初始化指示数字
	for (int i = 0; i < 13; i++)                       
            {
                for (int j = 0; j < 13; j++)
                {

                    num[i][j] = 0;
                }
            }
            
	//初始化雷区
	while(count!=0){
		x=rand()%11;
		y=rand()%11;
		if(bomb[x][y]!=1){
			bomb[x][y]=1;
			for (int i = x; i <= x + 2; i++)
                    	{
                        	for (int j = y; j <= y + 2; j++)
                        	{
                           		num[i][j]++;
                        	}
                    	}
			count--;
		}
	}
	
	
	for (int i=0;i<11;i++){
		for (int j=0;j<11;j++){
			
		}
	}
	
	//打印雷区
	clear();
	printf("\n");
	printf("\n");
	printf("                                         Welcome to Minesweeper                    \n");
	printf("\n\n");
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
			if(jiemian[i+1][j+1]==0||jiemian[i][j]==-1){
				printf("     ");
			}
			else{
				switch(jiemian[i+1][j+1]){
				case 0:
				printf("     ");
				break;
				case 1:
				printf("1    ");
				break;
				case 2:
				printf("2    ");
				break;
				case 3:
				printf("3    ");
				break;
				case 4:
				printf("4    ");
				break;
				case 5:
				printf("5    ");
				break;
				case 6:
				printf("6    ");
				break;
				case 7:
				printf("7    ");
				break;
				case 8:
				printf("8    ");
				break;
				}
			}
		}
		
	}
	printf("\n");	
	for(int i=0;i<360;i=i+32){
		drawLine(205,150+i,645,150+i,PEN_WHITE);
	}
	for(int i=0;i<450;i=i+40){
		drawLine(205+i,150,205+i,502,PEN_WHITE);
	}
	
	

	for (int i=0;i<5;i++){
		drawLine(0,67+i,800,67+i,PEN_WHITE);
		drawLine(0,100+i,800,100+i,PEN_WHITE);
	}
	

	
	while (1){
		printf("\n                       enter 'q' to exit\n");
		printf("                       please input the x:");
		read(fd_stdin, optionx, 1);
		printf("                       please input the y:");
		read(fd_stdin, optiony, 1);
		switch(optionx[0]){
			
			case '1':
			x=1;
			break;
			
			case '2':
			x=2;
			break;
			case '3':
			x=3;
			break;
			case '4':
			x=4;
			break;
			case '5':
			x=5;
			break;
			case '6':
			x=6;
			break;
			case '7':
			x=7;
			break;
			case '8':
			x=8;
			break;
			case '9':
			x=9;
			break;
			case 'a':
			x=10;
			break;
			case 'b':
			x=11;
			break;	
			
			case 'q':
			return;		
		}
		switch(optiony[0]){
			
			case '1':
			y=1;
			break;
			
			case '2':
			y=2;
			break;
			case '3':
			y=3;
			break;
			case '4':
			y=4;
			break;
			case '5':
			y=5;
			break;
			case '6':
			y=6;
			break;
			case '7':
			y=7;
			break;
			case '8':
			y=8;
			break;
			case '9':
			y=9;
			break;	
			case 'a':
			y=10;
			break;
			case 'b':
			y=11;
			break;		
		}
		
		
		
		if(bomb[x-1][y-1]==1)
			jiemian[x][y]=-2;//代表这里是雷
		else chakan(x,y);
		
	//打印雷区
	clear();
	printf("\n");
	printf("\n");
	printf("                                         Welcome to Minesweeper                    \n");
	printf("\n\n");
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
			switch(jiemian[i+1][j+1]){
			case -2:
			printf("Bomb");
			printf(" ");
			break;
			case -1:
			printf("     ");
			break;
			case 0:
			printf("0    ");
			break;
			case 1:
			printf("1    ");
			break;
			case 2:
			printf("2    ");
			break;
			case 3:
			printf("3    ");
			break;
			case 4:
			printf("4    ");
			break;
			case 5:
			printf("5    ");
			break;
			case 6:
			printf("6    ");
			break;
			case 7:
			printf("7    ");
			break;
			case 8:
			printf("8    ");
			break;
			}
		}
	}
		printf("\n");
	
	
		for (int i=0;i<5;i++){
			drawLine(0,67+i,800,67+i,PEN_WHITE);
			drawLine(0,100+i,800,100+i,PEN_WHITE);
		}
		for(int i=0;i<360;i=i+32){
			drawLine(205,150+i,645,150+i,PEN_WHITE);
		}
		for(int i=0;i<450;i=i+40){
			drawLine(205+i,150,205+i,502,PEN_WHITE);
		}

	}
	
}

//递归翻开格子
void chakan(int a,int b){
	jiemian[a][b]=num[a][b];
	if(a!=0&&a!=12&&b!=0&&b!=12&&jiemian[a][b]==0){
		if(jiemian[a-1][b-1]==-1)chakan(a-1,b-1);
		if(jiemian[a-1][b  ]==-1)chakan(a-1,b  );
		if(jiemian[a-1][b+1]==-1)chakan(a-1,b+1);
		if(jiemian[a  ][b-1]==-1)chakan(a  ,b-1);
		if(jiemian[a  ][b+1]==-1)chakan(a  ,b+1);
		if(jiemian[a+1][b-1]==-1)chakan(a+1,b-1);
		if(jiemian[a+1][b  ]==-1)chakan(a+1,b  );
		if(jiemian[a+1][b+1]==-1)chakan(a+1,b+1);
	}
}

