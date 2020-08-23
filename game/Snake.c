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
//#include <stdlib.h>
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
int MAX_WIDTH = 15;
int MAX_HEIGHT = 15;
char map[17][17];//地图
int SnakeLength;//蛇长度
int EndLength;
int dir[4][2] = { {-1,0},{1,0},{0,-1}, {0,1} };//运动方向记录
int pointX, pointY;
int gameStatus;
int eatFlag;//标志当前是否吃了点
struct BODY{
	int x;
	int y;
	int nowDir;
}snake[200];

void initSnake() {
	//初始化地图
	for (int i = 0; i < MAX_WIDTH + 2; ++i)map[0][i] = '*';
	for (int i = 0; i < MAX_WIDTH + 2; ++i)map[i][0] = '*';
	for (int i = 0; i < MAX_WIDTH + 2; ++i)map[MAX_HEIGHT+1][i] = '*';
	for (int i = 0; i < MAX_WIDTH + 2; ++i)map[i][MAX_WIDTH+1] = '*';
	for (int x = 1; x <= MAX_HEIGHT; ++x) {
		for (int y = 1; y <= MAX_WIDTH; ++y) {
			map[x][y] = ' ';
		}
	}
	//初始化蛇
	SnakeLength = 2;
	snake[0].x = 5; snake[0].y = 5; snake[0].nowDir = DOWN;
	snake[1].x = 4; snake[1].y = 5; snake[1].nowDir = DOWN;
	map[snake[0].x][snake[0].y] = 'O';
	map[snake[1].x][snake[1].y] = '|';
	//游戏初始状态
	gameStatus = 1;
	EndLength = 150;
	//第一个点生成
	while (1) {
		pointX = rand() % (MAX_HEIGHT + 1);
		pointY = rand() % (MAX_WIDTH + 1);
		if (map[pointX][pointY] == ' ') { break; }
	}
	//初始化键盘
	mykey_pressed=0;
	return;
}
int checkEnd() {//判断当前移动后是否结束游戏
	int nextX = snake[0].x + dir[snake[0].nowDir][0];
	int nextY = snake[0].y + dir[snake[0].nowDir][1];
	if (SnakeLength >= EndLength){gameStatus=0;return 1;}
	if (map[nextX][nextY] == ' ' || map[nextX][nextY] == 'X')return 0;
	gameStatus=0;
	return 1;
}
 void SnakeMove() {
	int exDir = snake[0].nowDir;
	int tmpDir;
	int tailX = snake[SnakeLength - 1].x;
	int tailY = snake[SnakeLength - 1].y;
	int tailDir = snake[SnakeLength - 1].nowDir;
	for (int i = 0; i < SnakeLength; ++i) {
		tmpDir = snake[i].nowDir;
		snake[i].x = snake[i].x + dir[snake[i].nowDir][0];
		snake[i].y = snake[i].y + dir[snake[i].nowDir][1];
		snake[i].nowDir = exDir;
		exDir = tmpDir;
	}
	//调整输出地图
	switch (snake[1].nowDir)
	{
	case UP:
	case DOWN: 
		{map[snake[1].x][snake[1].y] = '|'; break; }
	case LEFT:
	case RIGHT: 
		{map[snake[1].x][snake[1].y] = '-'; break; }
	}
	map[snake[0].x][snake[0].y] = 'O';
	if (eatFlag==0) {
		map[tailX][tailY] = ' ';
	}
	else {
		eatFlag = 0; 
		snake[SnakeLength].x = tailX;
		snake[SnakeLength].y = tailY;
		snake[SnakeLength].nowDir = tailDir;
		++SnakeLength; 
		if (checkEnd())gameStatus = 0;
	}
	return;
}


void Turn(int dir) {
	switch (dir)
	{
	case UP:
	{snake[0].nowDir = UP; break; }
	case DOWN:
	{snake[0].nowDir = DOWN; break; }
	case LEFT:
	{snake[0].nowDir = LEFT; break; }
	case RIGHT:
	{snake[0].nowDir = RIGHT; break; }
	}
	return;
}

void checkPoint() {
	if (snake[0].x == pointX && snake[0].y == pointY) {
		eatFlag = 1; 
		while (1) {
			pointX = rand() % (MAX_HEIGHT + 1);
			pointY = rand() % (MAX_WIDTH + 1);
			if (map[pointX][pointY] == ' ') { break; }
		}
	}
	return;
}

void printInfo(){
    printf("          ************************************          \n");
    printf("          *              Snake               *          \n");
    printf("          *   Control the snake to eat coin  *          \n");
    printf("          *                                  *          \n");
    printf("          *              Control:            *          \n");
    printf("          * Tap SPACE to turn the direction  *          \n");
    printf("          *          (anticlockwise)         *          \n");
    printf("          *                                  *          \n");
    printf("          ************************************          \n");
    printf("\n\n");
}


void Draw() {
	clear();
	printInfo();
	map[pointX][pointY] = 'X';
	for (int x = 0; x < MAX_HEIGHT + 2; ++x) {
	printf("          ");
		for (int y = 0; y < MAX_WIDTH + 2; ++y) {
			switch(map[x][y]){
			    case ' ':
			        printf(" ");
			        break;
			    case '*':
			        printf("*");
			        break;
			    case 'O':
			        printf("O");
			        break;
			    case 'X':
			        printf("X");
			        break;
			    case '|':
			        printf("|");
			        break;
			    case '-':
			        printf("-");
			        break;
			}
		}
		printf("\n");
	}
	printf("          ");
	printf("Score: %d\n",SnakeLength-2);
	return;
}

void checkInput(int fd_stdin,int fd_stdout)
{
    if (mykey_pressed==1) 
    {
	char option[10];
	if (mykey_pressed) 
	{
	mykey_pressed = 0;
	//char key = option[0];    //_gettch()可以用来监听键盘按键
		switch (snake[0].nowDir)
		{
		    case UP:
		        {Turn(LEFT); break; }
		    case DOWN:
			{Turn(RIGHT); break; }
	            case LEFT:
			{Turn(DOWN); break; }
		    case RIGHT:
			{Turn(UP); break; }
		}
			
	}
    }
}

void runSnake(int fd_stdin,int fd_stdout)
{
    initSnake();
    Draw();
    while (gameStatus)
    {
        SnakeMove();
	checkPoint();
	Draw();
        checkInput(fd_stdin,fd_stdout);
        if (checkEnd())break;
        milli_delay(600);
    }
    clear();
    return;
}
