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
#include "flappy.h"

int www = 140;
int hhh = 27;
char *namess = "rb";
unsigned char bmp[27][140] = {
	{231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231},
	{231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231},
	{231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231},
	{231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231},
	{231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 33, 33, 33, 33, 33, 33, 59, 59, 231, 231, 231, 231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 33, 33, 33, 33, 33, 33, 59, 59, 231, 231, 231, 231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 33, 33, 33, 33, 33, 33, 59, 59, 231, 231, 231, 231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231},
	{231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 33, 33, 33, 33, 33, 33, 59, 59, 231, 231, 231, 231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 33, 33, 33, 33, 33, 33, 59, 59, 231, 231, 231, 231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 33, 33, 33, 33, 33, 33, 59, 59, 231, 231, 231, 231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231},
	{231, 231, 231, 231, 231, 231, 231, 59, 59, 33, 33, 33, 33, 27, 27, 27, 27, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 33, 33, 33, 33, 27, 27, 27, 27, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 33, 33, 33, 33, 27, 27, 27, 27, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 231, 231, 231, 231, 231, 231},
	{231, 231, 231, 231, 231, 231, 231, 59, 59, 33, 33, 33, 33, 27, 27, 27, 27, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 33, 33, 33, 33, 27, 27, 27, 27, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 33, 33, 33, 33, 27, 27, 27, 27, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 231, 231, 231, 231, 231, 231},
	{231, 231, 231, 59, 59, 59, 59, 59, 59, 59, 59, 27, 27, 27, 27, 27, 27, 59, 59, 195, 195, 231, 231, 231, 231, 59, 59, 231, 231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 33, 33, 27, 27, 27, 27, 27, 27, 27, 27, 59, 59, 195, 195, 231, 231, 231, 231, 59, 59, 231, 231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 33, 33, 27, 27, 27, 27, 27, 27, 27, 27, 59, 59, 195, 195, 231, 231, 231, 231, 59, 59, 231, 231, 59, 59, 231, 231, 231, 231},
	{231, 231, 231, 59, 59, 59, 59, 59, 59, 59, 59, 27, 27, 27, 27, 27, 27, 59, 59, 195, 195, 231, 231, 231, 231, 59, 59, 231, 231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 33, 33, 27, 27, 27, 27, 27, 27, 27, 27, 59, 59, 195, 195, 231, 231, 231, 231, 59, 59, 231, 231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 33, 33, 27, 27, 27, 27, 27, 27, 27, 27, 59, 59, 195, 195, 231, 231, 231, 231, 59, 59, 231, 231, 59, 59, 231, 231, 231, 231},
	{231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 27, 27, 27, 27, 59, 59, 195, 195, 231, 231, 231, 231, 59, 59, 231, 231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 59, 59, 195, 195, 231, 231, 231, 231, 59, 59, 231, 231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 59, 59, 195, 195, 231, 231, 231, 231, 59, 59, 231, 231, 59, 59, 231, 231, 231, 231},
	{231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 27, 27, 27, 27, 59, 59, 195, 195, 231, 231, 231, 231, 59, 59, 231, 231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 59, 59, 195, 195, 231, 231, 231, 231, 59, 59, 231, 231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 59, 59, 195, 195, 231, 231, 231, 231, 59, 59, 231, 231, 59, 59, 231, 231, 231, 231},
	{231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 27, 27, 27, 27, 59, 59, 195, 195, 231, 231, 231, 231, 231, 231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 27, 27, 27, 27, 27, 27, 59, 59, 195, 195, 231, 231, 231, 231, 231, 231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 59, 59, 195, 195, 231, 231, 231, 231, 231, 231, 59, 59, 231, 231, 231, 231},
	{231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 27, 27, 27, 27, 59, 59, 195, 195, 231, 231, 231, 231, 231, 231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 27, 27, 27, 27, 27, 27, 59, 59, 195, 195, 231, 231, 231, 231, 231, 231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 59, 59, 195, 195, 231, 231, 231, 231, 231, 231, 59, 59, 231, 231, 231, 231},
	{231, 59, 59, 195, 195, 231, 231, 231, 231, 231, 231, 195, 195, 59, 59, 27, 27, 27, 27, 27, 27, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 27, 27, 27, 27, 27, 27, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 27, 27, 27, 27, 27, 27, 27, 27, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231},
	{231, 59, 59, 195, 195, 231, 231, 231, 231, 231, 231, 195, 195, 59, 59, 27, 27, 27, 27, 27, 27, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 27, 27, 27, 27, 27, 27, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 27, 27, 27, 27, 27, 27, 27, 27, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231},
	{231, 231, 231, 59, 59, 195, 195, 195, 195, 195, 195, 59, 59, 27, 27, 27, 27, 27, 27, 59, 59, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 195, 195, 231, 231, 231, 231, 231, 231, 195, 195, 59, 59, 27, 27, 27, 27, 59, 59, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 195, 195, 231, 231, 231, 231, 231, 231, 195, 195, 59, 59, 27, 27, 27, 27, 59, 59, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 59, 59},
	{231, 231, 231, 59, 59, 195, 195, 195, 195, 195, 195, 59, 59, 27, 27, 27, 27, 27, 27, 59, 59, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 195, 195, 231, 231, 231, 231, 231, 231, 195, 195, 59, 59, 27, 27, 27, 27, 59, 59, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 195, 195, 231, 231, 231, 231, 231, 231, 195, 195, 59, 59, 27, 27, 27, 27, 59, 59, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 59, 59},
	{231, 231, 231, 231, 231, 59, 59, 59, 59, 59, 59, 26, 26, 26, 26, 26, 26, 59, 59, 81, 81, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 26, 26, 26, 26, 59, 59, 81, 81, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 26, 26, 26, 26, 59, 59, 81, 81, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231},
	{231, 231, 231, 231, 231, 59, 59, 59, 59, 59, 59, 26, 26, 26, 26, 26, 26, 59, 59, 81, 81, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 26, 26, 26, 26, 59, 59, 81, 81, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 26, 26, 26, 26, 59, 59, 81, 81, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231},
	{231, 231, 231, 231, 231, 59, 59, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 59, 59, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 59, 59, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 231, 231, 231, 231, 195, 195, 59, 59, 26, 26, 26, 26, 26, 26, 26, 26, 59, 59, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 59, 59, 231, 231},
	{231, 231, 231, 231, 231, 59, 59, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 59, 59, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 59, 59, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 231, 231, 231, 231, 195, 195, 59, 59, 26, 26, 26, 26, 26, 26, 26, 26, 59, 59, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 59, 59, 231, 231},
	{231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 59, 59, 59, 59, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231, 231, 231},
	{231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 59, 59, 59, 59, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231, 231, 231},
	{231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231},
	{231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231},
	{231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231, 231}};

//定义全局变量
int high, width;					  //边界
int bird_x, bird_y;					  //小鸟坐标
int bird_width = 25, bird_hight = 18; //小鸟长宽
int dist_down = 6, dist_up = 12;	  //小鸟下降距离和上升距离
int bird_status = 0;				  //小鸟状态

struct bar bars[3];
int bar_width = 52; //挡板宽度
int bar_num = 0;	//挡板数量
int bar_speed = 8;	//挡板移动速度

int score;			//得分
int gamestatus = 1; //游戏状态

unsigned char screen[600 * 800];
unsigned char bg[600 * 800];
unsigned char birdUP[23 * 32], birdRight[23 * 32], birdDown[23 * 32];
unsigned char bird[3][23 * 32];
unsigned char pipeUp[600 * 52];
unsigned char pipeDown[600 * 52];

void showbg() //背景整体左移，最后一列由新的背景替代
{
	unsigned char buf[1600];
	for (int i = 0; i < bar_speed; ++i)
	{
		for (int j = 0; j < 600; ++j)
		{
			buf[j * bar_speed + i] = bg[j * 800 + i];
		}
	}
	for (int i = 0; i < 800 - bar_speed; ++i)
	{
		for (int j = 0; j < 600; ++j)
		{
			bg[j * 800 + i] = bg[j * 800 + i + bar_speed];
		}
	}
	for (int i = 800 - bar_speed; i < 800; ++i)
	{
		for (int j = 0; j < 600; ++j)
		{
			bg[j * 800 + i] = buf[j * bar_speed + (i - 800 + bar_speed)];
		}
	}
	drawFrom(bg, 800, 0, 0, 800, 600);
}

void showpipe() // 在背景上增添柱子
{
	for (int i = 0; i < bar_num; ++i)
	{
		drawFrom(pipeUp, bar_width, bars[i].bar_x, bars[i].bar_yDown, bar_width, high - bars[i].bar_yDown);
		drawFrom(pipeDown, bar_width, bars[i].bar_x, 0, bar_width, bars[i].bar_yTop);
	}
}
void showbird() // 在背景上增添小鸟
{
	bird_status = (bird_status) % 3;
	drawFrom(bird[bird_status], 800, bird_x, bird_y, 32, 23);
}
void openImg(char *path, unsigned char *Img, int w, int h)
{
	int size[3];
	int fd = open(path, O_RDWR);
	int n = read(fd, size, 3);
	n = read(fd, Img, w * h);
	close(fd);
}

struct bar newbar()
{
	struct bar newbar;
	newbar.bar_x = width;								  //挡板坐标
	newbar.bar_dist = 50 + rand() % 20;					  //挡板开口宽度
	newbar.bar_yDown = rand() % (high - newbar.bar_dist); //挡板开口
	newbar.bar_yTop = newbar.bar_yDown - newbar.bar_dist;
	return newbar;
}
void startup() //数据初始化
{
	mykey_pressed = 0;
	high = 600; //初始化边界
	width = 800;

	bird_y = high / 2; //初始化小鸟坐标
	bird_x = 20;

	bars[0] = newbar();
	bar_num = 1;
	score = 0; //得分
	unsigned char buf[27 * 140];
	openImg("/rb", buf, 140, 27); //初始化图片
	for (int i = 1; i < 33; i++)
	{
		for (int j = 2; j < 25; j++)
		{
			bird[0][(j - 2) * 32 + (i - 1)] = buf[j * 140 + i];
		}
	}
	for (int i = 53; i < 85; i++)
	{
		for (int j = 2; j < 25; j++)
		{
			bird[1][(j - 2) * 32 + (i - 53)] = buf[j * 140 + i];
		}
	}
	for (int i = 106; i < 138; i++)
	{
		for (int j = 2; j < 25; j++)
		{
			bird[2][(j - 2) * 32 + (i - 106)] = buf[j * 140 + i];
		}
	}
	openImg("/sky", bg, 800, 600);
	openImg("/wall", pipeUp, 52, 600);
	for (int i = 0; i < 52 * 600; ++i)
	{
		pipeDown[i] = pipeUp[52 * 600 - i - 1];
	}
	gamestatus = 1;
}
void show() //显示界面
{
	showbg();
	showpipe();
	showbird();
	drawFrom(screen, 800, 0, 0, 800, 600);
}
void updateWithoutInput() //与用户输入无关的更新
{
	if (bird_x + bird_width > bars[0].bar_x && bird_x < bars[0].bar_x + bar_width)
	{
		if ((bird_y > bars[0].bar_yTop) && (bird_y + bird_hight < bars[0].bar_yDown))
		{ //小鸟未碰到挡板
		}
		else
		{
			gamestatus = 0; //小鸟没没通过挡板开口小鸟死亡
		}
	}

	if (bird_y < high - dist_down)
	{
		bird_y += dist_down;
	}
	else
	{
		gamestatus = 0; //触地死亡
	}

	for (int i = 0; i < bar_num; ++i)
	{ //挡板移动
		bars[i].bar_x -= bar_speed;
	}
	if (bars[0].bar_x + bar_width <= 0) //挡板离开边界
	{
		bar_num--;
		score++;
		if (bar_num == 0)
		{
			bars[0] = newbar();
			bar_num++;
		}
		else if (bar_num == 1)
		{
			bars[0] = bars[1];
		}
		else if (bar_num == 2)
		{
			bars[0] = bars[1];
			bars[1] = bars[2];
		}
	}
	if (bar_num < 3 && bars[bar_num - 1].bar_x < 150 && rand() % 2 == 0) //产生新的挡板
	{
		bars[bar_num] = newbar();
		bar_num++;
	}
}
void updateWithInput(int fd_stdin, int fd_stdout) //与用户输入有关的更新
{
	if (mykey_pressed == 1)
	{
		mykey_pressed = 0;
		if ((bird_y > 0) && (bird_y <= high)) //小鸟向上跳
			bird_y = max(bird_y - dist_up, 0);
		bird_status++;
	}
	else
	{
		bird_status = 0;
	}
}

PUBLIC void waitkey()
{
	// milli_delay(10000);
	mykey_pressed = 0;
	while (mykey_pressed == 0)
	{
		/* code */
	}
	clear();
}

PUBLIC void startflappyBird(int fd_stdin, int fd_stdout)
{
	openImg("/start", screen, 800, 600);
	drawFrom(screen, 800, 0, 0, 800, 600);
	// milli_delay(1000);
	startup(); //数据初始化
	while (gamestatus)
	{
		show();								  //显示界面
		updateWithoutInput();				  //与用户输入无关的更新
		updateWithInput(fd_stdin, fd_stdout); //与用户输入有关的更新
											  // milli_delay(600);
	}
	printf("score:%d \n", score);
	waitkey();
}
