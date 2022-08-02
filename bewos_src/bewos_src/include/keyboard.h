#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#define BUF_SIZE 1024

void initKBD(void);
void isr_kbd(void);
char conv_scancode_to_char(char sc);
void disabilita_kbd(void);
void abilita_kbd(void);
void kb_wait(void);
void kb_ack(void);

void set_leds(void);
void enable_caps_led(void);
void enable_num_led(void);
void enable_scroll_led(void);
void disable_caps_led(void);
void disable_num_led(void);
void disable_scroll_led(void);
int get_caps_led(void);
int get_num_led(void);
int get_caps_led(void);


void press_enter(void);
void unpress_enter(void);
int get_press_enter(void);
void press_key(void);
void unpress_key(void);
int get_press_key(void);

void fflush(char *buffer);

extern char buffer_kbd[BUF_SIZE];
char block_clear_status;
char *block_video_mem;
extern unsigned int pos_buffer_kbd;


       /*TASTIERA ITALIANA*/

static unsigned char key_map[] = {
       0,   27,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0', '\'',    0,  127,    9,
     'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',
     'o',  'p',    0,  '+', '\n',    0,  'a',  's',
     'd',  'f',  'g',  'h',  'j',  'k',  'l',    0,
       0, '\\',    0,    0,  'z',  'x',  'c',  'v',
     'b',  'n',  'm',  ',',  '.',  '-',    0,  '*',
       0,   32,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '-',    0,    0,    0,  '+',    0,
       0,    0,    0,    0,    0,    0,  '<',    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0 };

static unsigned char shift_map[] = {
       0,   27,  '!',  '"',    0,  '$',  '%',  '&',
     '/',  '(',  ')',  '=',  '?',  '^',  127,    9,
     'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
     'O',  'P',    0,  '*',   13,    0,  'A',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L',    0,
       0,  '|',  '0',    0,  'Z',  'X',  'C',  'V',
     'B',  'N',  'M',  ';',  ':',  '_',    0,    0,
       0,   32,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,  '>',    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0 };


static unsigned char  pad_map[] = { '7', '8', '9', '0', '4', '5', '6', '0', '1', '2', '3', '0', '0' };
       

      /*TASTIERA INGLESE */

/*static unsigned char key_map[] = {
       0,   27,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0',  '-',  '=',  127,    9,
     'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',
     'o',  'p',  '[',  ']', '\n',    0,  'a',  's',
     'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',
    '\'',  '`',    0, '\\',  'z',  'x',  'c',  'v',
     'b',  'n',  'm',  ',',  '.',  '/',    0,  '*',
       0,   32,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '-',    0,    0,    0,  '+',    0,
       0,    0,    0,    0,    0,    0,  '<',    0,
       0,    0,    0,    0,    0,    0,    0,    0,
	   0 };

static unsigned char shift_map[] = {
       0,   27,  '!',  '@',  '#',  '$',  '%',  '^',
     '&',  '*',  '(',  ')',  '_',  '+',  127,    9,
	 'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
	 'O',  'P',  '{',  '}',   13,    0,  'A',  'S',
	 'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':',
     '"',  '~',  '0',  '|',  'Z',  'X',  'C',  'V',
     'B',  'N',  'M',  '<',  '>',  '?',    0,  '*',
       0,   32,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '-',    0,    0,    0,  '+',    0,
       0,    0,    0,    0,    0,    0,  '>',    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0 };*/

#endif
