#ifndef __CONSOLE_H
#define __CONSOLE_H


#define N_CONSOLES   8	//Numero di console possibili
#define SCR_C 80 //Definisco le colonne dello schermo
#define SCR_R 25 //Definisco le righe dello schermo
#define MEM_VIDEO 0xb8000		    //indirizzo da dove iniza la memoria video
#define VIDEO_MEM_SIZE (SCR_R * SCR_C * 2)  //moltiplichiamo per 2 perche' bisogna tenere anche conto del colore



#define BLACK_COLOR 0x0
#define BLUE_COLOR 0x1
#define GREEN_COLOR 0x2
#define CYAN_COLOR 0x3
#define RED_COLOR 0x4
#define MAGENTA_COLOR 0x5
#define BROWN_COLOR 0x6
#define WHITE_COLOR 0x7
#define YELLOW_COLOR 0xE

#define BRIGHT_COLOR 0x8

#define BLINKING 0x70

#define DEFAULT_TEXT_COLOR		WHITE_COLOR

char init_console; // Variabile che viene usata solo al processo di avvio che indica alla console che verrà creata di non fare klear, poichè deve visualizzare le informazioni sull'avvio di bewos


struct console_s {
unsigned long init_video; //Indirizzo iniziale della memoria video
unsigned long pos_video; //posizione assoluta corrente all' interno della memoria video

unsigned long block_videomem;	/* Questa variabile conterra' l' indirizzo della memoria video oltre il quale non e' possibile cancellare caratteri in esso contenuti. Praticamente utilizzo questa variabile in modo da tenere conto fin dove e' possibile cancellare(tramite il tasto Backspace) i caratteri dalla memoria video. In particolare non dovrebbe essere possibile cancellare la memoria video dopo il prompt. Questo e' il motivo per il quale tale variabile viene inizializzata con la memoria video corrente subito dopo aver stampato a video il prompt di BeWos. Quando viene premuto backspace si controlla se l' indirizzo dal quale si vuole cancellare il carattere e' uguale a tale variabile; in questo caso(se e' uguale) il carattere non viene cancellato. Si veda anche i file keyboard.c(lo switch\case Backspace) e il file shell.c*/

unsigned long x, y; //coordinate correnti all' interno della memoria video
unsigned long attr; //attributi video
unsigned int key_leds; //valore da mandare alla porta 0x60 per settare i leds della tastiera
unsigned short pid_task_shell; /*conterra' il pid del task shell creato per la console. Ricordo infatti che vi e' una shell per ogni
 console*/

char buffer_temp[SCR_C*SCR_R];	/* Questo e' il buffer che conterra' i caratteri che si dovranno stampare sulla console. Questo buffer viene utilizzato per un motivo molto semplice. Un task creato in una console deve stampare il proprio output in QUELLA determinata console. In pratica se un task viene creato nella console numero 1 DEVE stampare il proprio output nella console numero 1. Se durante l' esecuzione del task(che mettiamo il caso stampa qualcosa a video) si cambia console passando alla console 2, l' output del task(creato nella console 1) dovra' essere messo nel buffer_temp della console numero 1. Quando si ri-switchera' di nuovo alla console numero 1 si stampera' a video il buffer. In pratica basta fare un semplice controllo in printk.c e console.c (per maggiori info si consiglia di vedere tali file) */

unsigned int index_buffer;	//viene utilizzato come indice nel buffer

};





extern struct console_s console[N_CONSOLES];
extern struct console_s *cur_console;


void make_console(unsigned int n);
void init_consoles(void);
void init_cur_console(void);
void switch_to_console(int n);
void info_cur_console(void);


#endif

