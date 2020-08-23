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
//#include <stdio.h>
//#include <stdlib.h>
//#include <time.h>
//#include <string.h>
//#include <conio.h>

#define ROW 100
#define COL 100

int hhh,lll;
void menu()
{
	printf("*******************************\n");
	printf("****  欢迎来到五子棋游戏！ ****\n");
	printf("****      1.进入游戏       ****\n");
	printf("****      0.退出游戏       ****\n");
	printf("*******************************\n");
	printf("请输入0或1:");
	return;
}

//初始化
void InitBoard(char board[ROW][COL], int row, int col)
{
	int i = 0;
	int j = 0;

	for (i = 0; i < row; i++)
	{
		for (j = 0; j < col; j++)
		{
			board[i][j] = ' ';
		}
	}
	return;
}

//打印棋盘 
void DisplayBoard(char board[ROW][COL], int row, int col)
{
	int i = 0;
	int j = 0;

	for (i = 0; i < row; i++)
	{
		printf("  %d ", i + 1); 
	}
	printf("\n");
	printf("|");
	for (j = 0; j < col; j++)
	{
		printf("---|"); 
	}
	printf("\n");

	for (i = 0; i < row; i++)
	{
		printf("|");
		for (j = 0; j < col; j++)
		{
			printf(" %c |", board[i][j]); 
		}
		printf(" %d ", i + 1); 
		printf("\n");
		printf("|");
		for (j = 0; j < col; j++)
		{
			printf("---|"); 
		}
		printf("\n");
	}
	return;
}

void ComputerMove(char board[ROW][COL], int row, int col)
{
	int x = 0;
	int y = 0;
	printf("电脑走:>\n");
	while (1)
	{
		x = rand() % row;
		y = rand() % col;

		if (board[x][y] == ' ')
		{
			board[x][y] = '0';
			break;
		}
		else
		{
			continue;
		}
	}
	return;
}


void PlayerMove(char board[ROW][COL], int row, int col,int fd_stdin,int fd_stdout)
{
	int x = 0;
	int y = 0;

	printf("玩家走:>\n");
	printf("请输入坐标(%d,%d): >", row, col);
	while (1)
	{
		//scanf("%d %d", &x, &y);
		read(fd_stdin, row, 3);
		read(fd_stdin, col, 3);
		if (x >= 1 && x <= row && y >= 1 && y <= col)
		{
			if (board[x - 1][y - 1] == ' ')
	 		{
				board[x - 1][y - 1] = 'x';
				break;
			}
	
			else
			{
				printf("该坐标已经被占用\n");
				printf("请重新输入:>");
				continue;
			}
		}

		else
		{
			printf("坐标非法\n");
			printf("请重新输入:>");
			continue;
		}
	}
	return;
}

int IsFull(char board[ROW][COL], int row, int col)
{
	int i = 0;
	int j = 0;

	for (i = 0; i < row; i++)
	{
		for (j = 0; j < col; j++)
		{
			if (board[i][j] == ' ')
			{
				return 0;
			}
		}
	}

	return 1;
}

//判断输赢
char IsWin(char board[ROW][COL], int row, int col)
{
	int i = 0;
	int j = 0;
	for (i = 0; i < row; i++)
	{
		for (j = 0; j < col - 4; j++)
		{
			if (board[i][j] == board[i][j + 1]
				&& board[i][j + 1] == board[i][j + 2]
				&& board[i][j + 2] == board[i][j + 3]
				&& board[i][j + 3] == board[i][j + 4]
				&& board[i][j] != ' ')
			{
				return board[i][j];
			}
		}
	}

	// 竖线上五子连成一线，赢家产生
	for (j = 0; j < col; j++)
	{
		for (i = 0; i < row - 4; i++)
		{
			if (board[i][j] == board[i + 1][j]
				&& board[i + 1][j] == board[i + 2][j]
				&& board[i + 2][j] == board[i + 3][j]
				&& board[i + 3][j] == board[i + 4][j]
				&& board[i][j] != ' ')
			{
				return board[i][j];
			}
		}
	}

	// 斜线上五子连成一线，赢家产生
	for (i = 0; i < row - 4; i++)
	{
		if (board[i][i] == board[i + 1][i + 1]
			&& board[i + 1][i + 1] == board[i + 2][i + 2]
			&& board[i + 2][i + 2] == board[i + 3][i + 3]
			&& board[i + 3][i + 3] == board[i + 4][i + 4]
			&& board[i][i] != ' ')
		{
			return board[i][i];
		}

		if (board[i][i + 4] == board[i + 1][i + 3]
			&& board[i + 1][i + 3] == board[i + 2][i + 2]
			&& board[i + 2][i + 2] == board[i + 3][i + 1]
			&& board[i + 3][i + 1] == board[i + 4][i]
			&& board[i][i + 4] != ' ')
		{
			return board[i][i + 4];
		}
	}

	//游戏平局
	if (IsFull(board, row, col))
	{
		return 'p';
	}

	//游戏结束
	return ' ';

}
void game(int fd_stdin, int fd_stdout)
{
	srand((unsigned int)time(NULL));
	printf("请输入你想要的棋盘大小（长宽不超过100）：");
	//scanf("%d", &hhh);
	//scanf("%d", &lll);
	read(fd_stdin, hhh, 3);
	read(fd_stdin, lll, 3);
	int ret = 0;
	char board[ROW][COL] = { 0 };
	InitBoard(board, hhh, lll);

	// 下棋
	while (1)
	{
		ComputerMove(board,hhh, lll); //电脑走
		ret = IsWin(board, hhh, lll);
		if (ret != ' ')
		{
			break;
		}
		clear(); //清屏，优化界面
		DisplayBoard(board, hhh, lll); //打印棋盘
		printf("\n");
		PlayerMove(board, hhh, lll,fd_stdin, fd_stdout); //玩家走
		ret = IsWin(board, hhh, lll);
		if (ret != ' ')
		{
			break;
		}
		DisplayBoard(board, hhh, lll); //打印棋盘
		printf("\n");
	}
	clear();
	// 判断输赢或平局
	if (ret == 'p')
	{
		printf("平局!\n");
		DisplayBoard(board, hhh,lll); //打印棋盘
	}
	else if (ret == 'x')
	{
		printf("玩家赢!\n");
		DisplayBoard(board, hhh, lll); //打印棋盘
	}
	else if (ret == '0')
	{
		printf("电脑赢!\n");
		DisplayBoard(board,hhh, lll); //打印棋盘
	}
	printf("输入任意键继续\n");
	
	return;
}
int runWZQ(int fd_stdin, int fd_stdout) {
	hhh = ROW; lll = COL;
	int input = 0;
	do
	{
		clear();
		menu();
		//scanf("%d", &input);
		read(fd_stdin, input, 1);
		switch (input)
		{
		case 1:
			game(fd_stdin, fd_stdout);
			break;
		case 0:
			printf("退出游戏\n");
			break;
		default:
			printf("输入错误\n");
		}
	} while (input);
	return;
}
