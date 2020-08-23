
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
#include "keyboard.h"
#include "proto.h"
#include "font.h"
#include "glib.h"

/* #define __TTY_DEBUG__ */

int headerHeight = 3;
int colormode = PEN_DARK_GREEN;

/* local routines */
PRIVATE void w_copy(unsigned int dst, unsigned int src, int size);
PUBLIC void clear_screen(int pos, int len);
PUBLIC void out_char(CONSOLE *con, char ch);
/*****************************************************************************
 *                                init_screen
 *****************************************************************************/
/**
 * Initialize the console of a certain tty.
 * 
 * @param tty  Whose console is to be initialized.
 *****************************************************************************/
PUBLIC void init_screen(TTY *tty)
{
	int nr_tty = tty - tty_table;
	//tty->console = console_table + nr_tty;
	tty->console = console_table;

	int v_mem_size = V_MEM_SIZE; /* size of Video Memory */
	int size_per_con = v_mem_size;
	tty->console->con_size = SCR_SIZE;
	tty->console->is_full = 0;
	tty->console->cursor = headerHeight * SCR_WIDTH;

	if (nr_tty == 0)
	{
		//tty->console->cursor = disp_pos / 2;
		disp_pos = 0;
	}
	fillRect(0, 0, scr_x, scr_y, colormode);
}

void print_char(int cursor_x, int cursor_y, char c, char ch)
{
	int x = cursor_x * 8;
	int y = cursor_y * 16;
	drawCharClr(x, y, ch, c, colormode);
}

/*****************************************************************************
 *                                out_char
 *****************************************************************************/
/**
 * Print a char in a certain console.
 * 
 * @param con  The console to which the char is printed.
 * @param ch   The char to print.
 *****************************************************************************/
PUBLIC void out_char(CONSOLE *con, char ch)
{
	assert(con->cursor < con->con_size);

	/*
	 * calculate the coordinate of cursor in current console (not in
	 * current screen)
	 */
	int cursor_x = (con->cursor) % SCR_WIDTH;
	int cursor_y = (con->cursor) / SCR_WIDTH;

	switch (ch)
	{
	case '\n':
		con->cursor = SCR_WIDTH * (cursor_y + 1);
		break;
	case '\b':
		if (con->cursor > 0)
		{
			con->cursor--;
			int cursor_x = (con->cursor) % SCR_WIDTH;
			int cursor_y = (con->cursor) / SCR_WIDTH;
			print_char(cursor_x, cursor_y, PEN_WHITE, ' ');
		}
		break;
	default:
		print_char(cursor_x, cursor_y, PEN_WHITE, ch);
		con->cursor++;
		break;
	}

	if (con->cursor >= con->con_size)
	{
		cursor_x = (con->cursor) % SCR_WIDTH;
		cursor_y = (con->cursor) / SCR_WIDTH;
		int cp_orig = (headerHeight + cursor_y + 1) * SCR_WIDTH - SCR_SIZE;
		w_copy(headerHeight * SCR_WIDTH, cp_orig, SCR_SIZE - SCR_WIDTH);
		con->cursor = (SCR_SIZE - SCR_WIDTH) + cursor_x;
		clear_screen(con->cursor, SCR_WIDTH);
		if (!con->is_full)
			con->is_full = 1;
	}

	assert(con->cursor < con->con_size);

	while (con->cursor >= SCR_SIZE || con->cursor < 0)
	{
		clear_screen(con->cursor, SCR_WIDTH);
	}
}

/*****************************************************************************
 *                                clear_screen
 *****************************************************************************/
/**
 * Write whitespaces to the screen.
 * 
 * @param pos  Write from here.
 * @param len  How many whitespaces will be written.
 *****************************************************************************/
PUBLIC void clear_screen(int pos, int len)
{
	int cursor_x = (pos) % SCR_WIDTH;
	int cursor_y = headerHeight + (pos) / SCR_WIDTH;
	fillRect(cursor_x, cursor_y * 16, cursor_x + 8 * len, cursor_y * 16 + 16, colormode);
}

/*****************************************************************************
 *                            is_current_console
 *****************************************************************************/
/**
 * Uses `nr_current_console' to determine if a console is the current one.
 * 
 * @param con   Ptr to console.
 * 
 * @return   TRUE if con is the current console.
 *****************************************************************************/
PUBLIC int is_current_console(CONSOLE *con)
{
	return (con == &console_table[current_console]);
}

/*****************************************************************************
 *                                select_console
 *****************************************************************************/
/**
 * Select a console as the current.
 * 
 * @param nr_console   Console nr, range in [0, NR_CONSOLES-1].
 *****************************************************************************/
PUBLIC void select_console(int nr_console)
{
	if ((nr_console < 0) || (nr_console >= NR_CONSOLES))
		return;
	current_console = 0;
}

/*****************************************************************************
 *                                w_copy
 *****************************************************************************/
/**
 * Copy data in WORDS.
 *
 * Note that the addresses of dst and src are not pointers, but integers, 'coz
 * in most cases we pass integers into it as parameters.
 * 
 * @param dst   Addr of destination.
 * @param src   Addr of source.
 * @param size  How many words will be copied.
 *****************************************************************************/
PRIVATE void w_copy(unsigned int dst, unsigned int src, int size)
{
	for (int k = 0; k < size; k++)
	{
		int src_x = (src % SCR_WIDTH) * 8;
		int src_y = (src / SCR_WIDTH) * 16;
		int dst_x = (dst % SCR_WIDTH) * 8;
		int dst_y = (dst / SCR_WIDTH) * 16;
		for (int y = 0; y < 16; y++)
		{
			char *p = vram + (src_y + y) * scr_x + src_x;
			char *d = vram + (dst_y + y) * scr_x + dst_x;
			for (int x = 0; x < 8; x++)
				d[x] = p[x];
		}
		src++;
		dst++;
	}
}
