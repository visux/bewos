/* Black & White Operating System
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details (in the COPYING file).
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 
 /******************************************INFO**********************************************
* Coders: Black
* Descrizione: Definizioni di funzioni per la stampa a video
***********************************************************************************************/



#include<io.h>
#include<stdarg.h>
#include<string.h>
#include<keyboard.h>
#include<console.h>
#include<syscall.h>
#include<gdt.h>
#include<system.h>

int precisione;



struct console_s *cur_console;




int printk(const char *string, ...) {

char buf[1024];
long flags;


    precisione=4;  //impostiamo la precisione di default per i numeri in virgola mobile
    va_list args;
    int len;
    char *ch;


    va_start(args, string);
    len = vsprintf(buf,string,args); /*La funzione vsprintf trasforma la stringa data in ingresso in una stringa di formato e mette l' output in
     un buffer. Se ad esempio passiamo "valore, %d", in buf  verra' messo "valore, 8" (ipotizzando che alla funzione sia stato passato il
     valore 8). Siccome la printk riceve un numero di parametri variabile(non definito) si utilizzano opportune macro per poter prendere tali
     valori(parametri) dallo stack. Tali macro sono definite in stdarg.h. Per maggiori informazioni si veda la documentazione al riguardo. */
    va_end(args);
    
		
		
    //se printk e' chiamata da un task in user mode allora per stampare i caratteri utilizzo una syscall
    if(read_cur_privilege() == USER_PRIVILEGE)
    	puts(buf);	/* Stampo la stringa. Ricordo che tale syscall e' definita in syscall.c. In user mode ho bisogno di una syscall perche' c'
	 e' il bisogno di disabilitare gli interrupt prima di stampare una stringa; in user mode, infatti, non e' possibile disabilitare gli
	 interrupt. Utilizzando una syscall scendo in kernel mode e posso disabilitarli :) */
	
    else   /*altrimenti(se sono in kernel mode) stampo la stringa chiamando direttamente la funzione che opera in kernel mode, senza
     eseguire alcuna syscall(cioe' senza chiamare puts che entra in kernel mode e a sua volta richiama sys_puts)*/
     sys_puts(buf);
     
     

    
    
    return len;
}
/********************************************************************************
********************************************************************************/
/* Ricorda: X indica la posizione nella riga, quindi la colonna :)
 Y la posizione nella colonna, quindi la riga
 la x indica sempre la larghezza, quindi il "numero" della colonna
mentre la y l'altezza, quindi il numero della riga
*/
void putk(char c) {
    switch (c) {
    case '\n':
        cur_console->y++; //siccome bisogna andare su una nuova riga incremento la variabile che tiene conto delle righe
        cur_console->x=0; //e inizializzo x
        break;
    case '\t':
        cur_console->x += DIM_TAB;
        break;
    case '\a':
        //beep(750,20);
        break;
    default:
        *(char*)cur_console->pos_video = c; //metto il carattere nella memoria video
        *(char*)(cur_console->pos_video+1) = cur_console->attr; //al byte successivo metto gli attributi
     
        cur_console->x++; //poiche' ho stampato un carattere incremento la variabile x
    }

    //controllo se sono stati stampati un numero di caratteri pari o maggiore alla larghezza dello schermo
    if (cur_console->x >= SCR_C) {
        cur_console->y++;  //se e' cosi' allora incremento la variabile che tiene conto delle righe
        cur_console->x = 0; //e inizializzo x
    }
    
    //controllo se sono state stampate un numero di righe pari o maggiore alla lunghezza dello schermo
       if (cur_console->y>=SCR_R) {
        screen_scroll(); //se e' cosi chiamo la funzione screen_scroll
	cur_console->y = SCR_R -1;
	
	cur_console->block_videomem -= SCR_C * 2; /*Siccome lo scroll decrementa di una riga la memoria video, facciamo in modo che 
	block_videomem punti ad una riga in meno. Ricordo che si moltiplica per 2 poiche' bisogna tenere conto anche degli attributi. Per
	 maggiori informazioni su tale variabile si veda il file console.h*/
	 
	 //info_cur_console();   //Ad ogni scroll stampo le informazioni della console corrente in alto alla prima riga
    }

                      

   goto_xy(cur_console->x, cur_console->y);

}
/********************************************************************************
********************************************************************************/
//Cancella una riga dallo schermo
void clear_line(int row_to_delete)
{
int spazi_stampati;
	
	//se non e' una riga valida , ritorno
	if(row_to_delete<0 && row_to_delete > SCR_R-1)
		return;

	char* line = (char*)cur_console->init_video + (row_to_delete * SCR_C * 2);
  
	//stampo degli spazi. In particolare stampo un numero di spazi pari alla larghezza dello schermo
	for(spazi_stampati = 0; spazi_stampati < SCR_C; spazi_stampati++)
	{
		*line++ = ' ';
		*line++ = DEFAULT_TEXT_COLOR;
 	}
}
/********************************************************************************
********************************************************************************/
//Gestisce lo scroll dello schermo nel caso in cui le righe stampate sono piu di quelle che pu�contenere in una schermata
void screen_scroll(void)
 {
char *i;
int k;
char*  videomem_max_in_una_schermata = (char*)(cur_console->init_video + VIDEO_MEM_SIZE-SCR_C*2);


     for (i=(char*)cur_console->init_video+(SCR_C*2*2); i < videomem_max_in_una_schermata; i++) /*Scorro tutta la memoria video di una
       schermata ad eccezione delle prime due righe*/

      *i = i[SCR_C * 2]; /*Copio man mano tutta la memoria video */


  clear_line(SCR_R - 1);	//cancello l' ultima riga
}
/********************************************************************************
********************************************************************************/
//la funzione sottostante ci posiziona in un punto dello schermo secondo le coordinate date in ingresso
void goto_xy(unsigned int new_x, unsigned int new_y)
{	
	//se le coordinate non sono valide, ritorno
            if (new_x>=SCR_C || new_y>=SCR_R)
                return;
		
            cur_console->x = new_x;
            cur_console->y = new_y;
		
	//imposto la posizione corrente della memoria video secondo i parametri che la funzione ha ricevuto in ingresso
            cur_console->pos_video=cur_console->init_video + ((new_y*SCR_C+new_x)*2);

	       set_cursor_pos();  //aggiorno il cursore
}
/********************************************************************************
********************************************************************************/
//La funzione ritorna la riga del carattere corrente
int get_cur_row_scr(void)
{
return cur_console->y; 
}
/********************************************************************************
********************************************************************************/
 //La funzione ritorna la colonna del carattere corrente.*/
int get_cur_col_scr(void)
{
return cur_console->x;
}
/********************************************************************************
********************************************************************************/
//Funzione di gestione del controller video (6845)
   void set_6845(int reg, int val) {
    const int vid_port = 0x3D4;

    outportb (reg,    vid_port);  // low byte
    outportb (val>>8, vid_port + 1);

    outportb (reg+1,  vid_port);  // high byte
    outportb (val,    vid_port + 1);
}
/********************************************************************************
********************************************************************************/
//Aggiorna il cursore
void set_cursor_pos(void) {
    set_6845(14, (cur_console->init_video-MEM_VIDEO)/2 + cur_console->y*SCR_C+cur_console->x);
}
/********************************************************************************
********************************************************************************/
//Funzine che "pulisce" lo schermo
void klear(void)
{

int i;

//pulisco lo schermo
for(i = 2; i < SCR_R; i++) // salto le prime die righe
clear_line(i);

goto_xy(0, 2); // Sposto il cursore alla prima riga dopo le intestazione (la terza)

}
/********************************************************************************
********************************************************************************/
//stampa un OK o un GO (o qualche altra cosa) alla fine della riga corrente. Se non c' e' abbastanza spazio per stampare la stringa a fine riga vado a capo e la stampo(questo serve per evitare che alcuni caratteri vengano stampati alla fine di una riga e gli altri all' inizio della riga successiva)
void printok(void)
{
char vecchio_colore = get_text_color();
char string [] = "[Go]";
int distanza = (SCR_C - cur_console->x);
int len = strlen(string) + 1;  /*il motivo per il quale aggiungo 1 alla lunghezza della stringa e' dovuto al fatto che voglio stampare la
   stringa alla fine della riga, ma lasciando uno spazio dalla fine dello schermo(monitor) .... O_o*/
  
  if(distanza > len) 
 {
goto_xy(SCR_C - len, cur_console->y);

 }
 
 else
  printk("\n");

set_text_color(GREEN_COLOR + BRIGHT_COLOR);	//Colore verde chiaro
printk("%s\n", string);
set_text_color(vecchio_colore); //Reimposto il colore di default

}
/********************************************************************************
********************************************************************************/
//Di seguito sono definite alcune funzioni per il settaggio del colore del testo e dello sfondo

/*Nota: l' attributo del colore e' di un byte. 0-3 bit indicano il colore del testo. 4-6 bit indicano il colore dello sfondo. l' ultimo bit(bit 7) e'
 posto a 0. In pratica i primi 4 bit indicano il colore del testo e gli altri tre bit indicano il colore dello sfondo. l' ultimo bit e' sempre 0 */
 
 //La funzione che segue modifica sia il colore del testo che dello sfondo
void set_color(char text_color, char background_color)
{
	cur_console->attr = text_color | (background_color << 4);

}

//La funzione che segue modifica solo il colore del testo
void set_text_color(char text_color)
{
char background_color = cur_console->attr & 0x70;	//Mi prendo il valore corrente del colore dello sfondo
//printk("%d", background_color);
	cur_console->attr = text_color | background_color;  	//modifico il colore del testo

	
}

// La seguente funzione ritorna il colore del testo attualmente in uso
char get_text_color(void) {
	return cur_console->attr;
}

//La funzione che segue modifica solo il colore dello sfondo
void set_background_color(char background_color)
{
char text_color = cur_console->attr & 0x7;	//Mi prendo il valore corrente del colore del testo

	cur_console->attr = text_color | (background_color << 4);	        //modifico il colore dello sfondo
}

//La funzione sottostante setta, come colore, il colore di default
void set_default_color(void)
{
cur_console->attr = DEFAULT_TEXT_COLOR;
}

void print_last_line(char attribute, char * text) {
	char* dest = cur_console->init_video + (80*24*2);
	do
	{
		*dest++ = *text;
		*dest++ = attribute;
	} while (*++text);
}