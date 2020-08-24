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

int hhh, lll;
void menu()
{
	printf("*******************************\n");
	printf("*** Welcome to Gobang game！***\n");
	printf("****      1.Enter Game     ****\n");
	printf("****      0.Quit Game      ****\n");
	printf("*******************************\n");
	printf("Please input 0 or 1:");
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
	printf("Computer Move:>\n");
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

void PlayerMove(char board[ROW][COL], int row, int col, int fd_stdin, int fd_stdout)
{
	int x = 0;
	int y = 0;

	printf("Player Move:>\n");
	printf("Please input your piece's coordinate(%d,%d): >", row, col);
	while (1)
	{
		char buf[10];
		read(fd_stdin, buf, 10);
		atoi(buf, &x);
		read(fd_stdin, buf, 10);
		atoi(buf, &y);
		if (x >= 1 && x <= row && y >= 1 && y <= col)
		{
			if (board[x - 1][y - 1] == ' ')
			{
				board[x - 1][y - 1] = 'x';
				break;
			}

			else
			{
				printf("The coordinate is already occupied！\n");
				printf("Please re-enter:>");
				continue;
			}
		}

		else
		{
			printf("Illegal coordinate！\n");
			printf("Please re-enter:>");
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
char IsWin1(char board[ROW][COL], int row, int col)
{
	int i = 0;
	int j = 0;
	for (i = 0; i < row; i++)
	{
		for (j = 0; j < col - 4; j++)
		{
			if (board[i][j] == board[i][j + 1] && board[i][j + 1] == board[i][j + 2] && board[i][j + 2] == board[i][j + 3] && board[i][j + 3] == board[i][j + 4] && board[i][j] != ' ')
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
			if (board[i][j] == board[i + 1][j] && board[i + 1][j] == board[i + 2][j] && board[i + 2][j] == board[i + 3][j] && board[i + 3][j] == board[i + 4][j] && board[i][j] != ' ')
			{
				return board[i][j];
			}
		}
	}

	// 斜线上五子连成一线，赢家产生
	for (i = 0; i < row - 4; i++)
	{
		if (board[i][i] == board[i + 1][i + 1] && board[i + 1][i + 1] == board[i + 2][i + 2] && board[i + 2][i + 2] == board[i + 3][i + 3] && board[i + 3][i + 3] == board[i + 4][i + 4] && board[i][i] != ' ')
		{
			return board[i][i];
		}

		if (board[i][i + 4] == board[i + 1][i + 3] && board[i + 1][i + 3] == board[i + 2][i + 2] && board[i + 2][i + 2] == board[i + 3][i + 1] && board[i + 3][i + 1] == board[i + 4][i] && board[i][i + 4] != ' ')
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
	printf("Please input the size of chessboard you want (length and width no more than 100)：");
	char buf[10];
	read(fd_stdin, buf, 10);
	atoi(buf, &hhh);
	read(fd_stdin, buf, 10);
	atoi(buf, &lll);
	int ret = 0;
	char board[ROW][COL] = {' '};
	InitBoard(board, hhh, lll);

	// 下棋
	while (1)
	{
		ComputerMove(board, hhh, lll); //电脑走
		ret = IsWin1(board, hhh, lll);
		if (ret != ' ')
		{
			break;
		}
		clear();					   //清屏，优化界面
		DisplayBoard(board, hhh, lll); //打印棋盘
		printf("\n");
		PlayerMove(board, hhh, lll, fd_stdin, fd_stdout); //玩家走
		ret = IsWin1(board, hhh, lll);
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
		printf("It ends in a draw!\n");
		DisplayBoard(board, hhh, lll); //打印棋盘
	}
	else if (ret == 'x')
	{
		printf("Player win!\n");
		DisplayBoard(board, hhh, lll); //打印棋盘
	}
	else if (ret == '0')
	{
		printf("Computer win!\n");
		DisplayBoard(board, hhh, lll); //打印棋盘
	}
	return;
}
int runWZQ(int fd_stdin, int fd_stdout)
{
	hhh = ROW;
	lll = COL;
	char input[10];
	do
	{
		clear();
		menu();
		//scanf("%d", &input);
		read(fd_stdin, input, 1);
		switch (input[0])
		{
		case '1':
			game(fd_stdin, fd_stdout);
			break;
		case '0':
			printf("Quit game\n");
			break;
		default:
			printf("Error input\n");
		}
	} while (input);
	return;
}
