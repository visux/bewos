#ifndef __IO_H
#define __IO_H

#include<stdarg.h>

#define DIM_TAB 8 //definisco la dimensione del tab(tasto della tastiera) in spazi



extern int precisione;

void putk(char c);
int  printk(const char *string, ...);
void klear(void);
void screen_scroll(void);
void printok(void);
void goto_xy(unsigned int new_x, unsigned int new_y);
int get_cur_row_scr(void);
int get_cur_col_scr(void);
void set_6845(int reg, int val);
void set_cursor_pos(void);
void set_color(char text_color, char background_color);
void set_text_color(char text_color);
void set_background_color(char background_color);
void set_default_color(void);
char get_text_color(void);


int vsprintf(char *buf, const char *string, va_list args);
int sprintf(char *buf, const char *string, ...);

#endif
