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
* Descrizione: Definizioni di funzioni che gestiscono la KeyBoard
***********************************************************************************************/



#include<irq.h>
#include<pic8259.h>
#include<io.h>
#include<io_port.h>
#include<keyboard.h> //La includiamo per la mappa dei caratteri e altre dichiarazioni
#include<shell.h>
#include<console.h>
#include<syscall.h>
#include<system.h> //Per le macro save_flags e restore_flags


#define LSHIFT 0x2a 		 //scancode del tasto shift sinistro
#define RSHIFT 0x36 		 //scancode del tasto shift destro
#define ALT 0x38		//scancode del tasto alt

#define ENTER 0x1c 		 //scancode del tasto Invio
#define SPACE 0x39  		 //scancode del tasto spazio
#define BACKSPACE 0xE   	 //scancode del tasto backspace
#define KEY_RELEASE 0X80  	 /*scancode generato al rilascio di un tasto. 0x80 in decimale �128. Ricordo che se uno scancode �maggiore o uguale a 128 significa che un tasto �stato rilasciato, altrimenti premuto */

#define CAPS_LED 0x3A
#define NUM_LED 0x45
#define SCROLL_LED 0x46

#define KBD_ACK  0xFA

#define ACTIVATED 1
#define DISACTIVATED 0


unsigned char scancode; //Ci servir�per contenere lo scancode del tasto digitato
unsigned char key;      //Ci servir�per la conversione dallo scancode al carattere

static unsigned char *current_map; //conterra' l'indirizzo della mappa dei caratteri

char buffer_kbd[BUF_SIZE];  //global. Buffer contenente i caratteri digitati sulla keyboard
unsigned int pos_buffer_kbd; //global. Indice corrente del buffer_kbd

static int active_press_enter; 
static int active_press_key; //viene settato ad 1 quando si preme un tasto della tastiera(solo se il tasto e' un carattere o un numero)
static int npadch = 0; /*utilizziamo questa variabile per stampare caratteri utilizzando alt-nnn, dove nnn indica un numero della key-pad
Tenendo premuto il tasto ALT e digitando i numeri nnn sul keypad dovremo essere in grado di visualizzare il carattere relativo al
 codice ASCII nnn */
 static int alt_flag = 0;
 static int shift_flag =0;
 
 struct console_s *cur_console; //global

/* Caratteri ascii associati a tasti particolari: - Per questi tasti vengono generati in sequenza piu' scancodes
 *	SCANCODE	ASCII	TASTO FRECCIA
 *	224,72		24	Freccia su
 *	224,80		25	Freccia giu'
 *	224,77		26	Freccia destra
 *	224,75		27	Freccia sinistra
 *	224,83		127	Del (Canc)
 *	224,79		128	Fine
 *	224,71		129	Home
 */
#define SCANCODE_EXT_FIRSTSTAGE 224
#define SCANCODE_ARROW_UP 72
#define SCANCODE_ARROW_DOWN 80
#define SCANCODE_ARROW_RIGHT 77
#define SCANCODE_ARROW_LEFT 75
#define SCANCODE_DEL 83
#define SCANCODE_FINE 79
#define SCANCODE_HOME 71
#define ASCII_ARROW_UP 24
#define ASCII_ARROW_DOWN 25
#define ASCII_ARROW_RIGHT 26
#define ASCII_ARROW_LEFT 27
#define ASCII_DEL 127
#define ASCII_FINE 128
#define ASCII_HOME 129
unsigned int ext_flag = 0; // Verra' attivata quando si e' ricevuto uno SCANCODE_EXT_FIRSTSTAGE

/********************************************************************************
********************************************************************************/
void initKBD(void)
{
long flags;
		save_flags(flags); //salvo gli Eflags nella variabile flags attraverso la macro save_flags
		asm("cli"); //Disabilito gli interrupt

insert_irq(isr_kbd, 1);   //1 identifica l' interrupt della tastiera
//abilita_irq(1,0x21);     //Abilitiamo l' irq della tastiera
//abilita_kbd();
pos_buffer_kbd=0; 		     //Inizializzo l'indice del buffer
disabilita_kbd();	    //la keyboard e' disattivata (per memorizzare del testo basta utilizzare le funzioni scank... che attivano la keyboard.)
active_press_enter = 0;  //Questa varibile indica se e' stato premuto il tasto invio: se si e'  posto a 1, altrimenti e' posto a 0
active_press_key = 0;
current_map = key_map;  //current_map contiene l' indirizzo della mappa dei caratteri

		restore_flags(flags);  //Ripristino gli Eflags
}
/********************************************************************************
********************************************************************************/
void isr_kbd(void)
{
int i;
char temp;





//se pos_buffer_kbd che utilizziamo come indice all' interno del buffer supera le dimensione del buffer stesso azzeriamo tale indice
if(pos_buffer_kbd>=BUF_SIZE)
	{
	fflush(buffer_kbd);
	pos_buffer_kbd=0;
	}



scancode = inportb(0x60); //Assegno alla variabile 'scancode' lo scancode relativo al tasto premuto prelevabile dalla porta 0x60
//disabilita_kbd(); //Ogni volta che si legge uno scancode bisogna disabilitare ed attivare la tastiera affinch�si possano
//kb_wait();
//abilita_kbd();    //accettare gli eventuali successivi scancode
//outportb(0x20, 0x20); /*Utilizziamo questa istruzione per informare il PIC del completamento dell' interrupt. Questa istruzione
			// viene denominata EOI(End Of Interrupt) */

/* flush the keyboard controller */
	 do
	{
		temp = inportb(0x64);
		if((temp & 0x01) != 0)
		{
			(void)inportb(0x60);
			continue;
		}
	} while((temp & 0x02) != 0);
			
						
			
/*setto la mappa dei caratteri*/

//se il il caps e' attivo e non e' stato premuto il tasto shift....
if(get_caps_led() == ACTIVATED && shift_flag == DISACTIVATED)
	current_map = shift_map;
//se il caps e' attivo ed e' stato premuto il tasto shift...
else if(get_caps_led() == ACTIVATED && shift_flag == ACTIVATED)
	current_map = key_map;
//se il caps non e' attivo ed e' stato premuto il tasto shift...
else if(shift_flag == ACTIVATED)
	current_map = shift_map;	
//se il caps non e' attivo e non e' stato premuto il tasto shift...
else
	current_map = key_map;
	
/* ..........*/

//printk("<%d>", (int)scancode);

 if (ext_flag) { /* Gestione degli scancodes estesi (224.x) */
	ext_flag = 0; // Azzero il contatore degli stages degli scancode estesi
	if (scancode >= 0x80) return; // Se si tratta di un tasto rilasciato semplicemente ignoro la su pressione
	switch (scancode) {
		case SCANCODE_ARROW_UP: key = ASCII_ARROW_UP; break;
		case SCANCODE_ARROW_DOWN: key = ASCII_ARROW_DOWN; break;
		case SCANCODE_ARROW_RIGHT: key = ASCII_ARROW_RIGHT; break;
		case SCANCODE_ARROW_LEFT: key = ASCII_ARROW_LEFT; break;
		case SCANCODE_DEL: key = ASCII_DEL; break;
		case SCANCODE_FINE: key = ASCII_FINE; break;
		case SCANCODE_HOME: key = ASCII_HOME; break;
	}
	buffer_kbd[pos_buffer_kbd] = key; //Memorizzo il tasto digitato in un buffer
	pos_buffer_kbd++; //quindi incremento l' indice
	press_key(); /*chiamo la funzione che setta ad 1 la variabile active_press_key. Cio' sta a significare che e' stato premuto un carattere o un numero*/
	return;
 }

 switch(scancode)
 {
 
 case SCANCODE_EXT_FIRSTSTAGE:
	ext_flag = 1;
	return; // Attendo il secondo scancode
 
//Se �stato premuto lo shift(destro o sinistro) allora setto la current_map con la mappa dei caratteri maiuscoli 
 case RSHIFT:
 case LSHIFT:
 shift_flag = ACTIVATED;
 
 if(get_caps_led() == ACTIVATED)
  current_map = key_map;
 else
  current_map = shift_map;
  return;
break;

//se e' stato rilasciato il tasto shift(destro o sinistro) setto la current_map con la mappa dei caratteri minuscoli
case RSHIFT|KEY_RELEASE:
case LSHIFT|KEY_RELEASE:
 shift_flag = DISACTIVATED;

 if(get_caps_led() == ACTIVATED)
  current_map = shift_map;
 else
  current_map = key_map;
  return;
break;




 case ENTER: //Se viene premuto enter

buffer_kbd[pos_buffer_kbd] = '\n';
pos_buffer_kbd++;
press_enter(); //E' stato premuto invio. La funzione setta active_press_enter a 1

return;
break;

 case BACKSPACE:



buffer_kbd[pos_buffer_kbd-1] = '\b'; //siccome e' stato cancellato un carattere, imposto nel buffer(al posto di quel carattere) l' escape '\b'

press_key();
return;
break;




case ALT:
alt_flag = ACTIVATED;
return;
break;

case ALT|KEY_RELEASE:
alt_flag = DISACTIVATED;
if(npadch != 0)
{
	key = npadch;
	npadch = 0;
}
else
return;
break;


case CAPS_LED:
if(get_caps_led() == ACTIVATED)
	disable_caps_led();
else
	enable_caps_led();
	
info_cur_console();
return;
break;


case NUM_LED:
if(get_num_led() == ACTIVATED)
	disable_num_led();
else
	enable_num_led();
	
info_cur_console();
return;
break;


case SCROLL_LED:
if(get_scroll_led() == ACTIVATED)
	disable_scroll_led();
else
	enable_scroll_led();
	
info_cur_console();
return;
break;



		
 /*i seguenti scancode sono gli scancode dei numeri del tasterino numerico*/
case 0x47:
case 0x48:
case 0x49:
case 0x4B:
case 0x4C:
case 0x4D:
case 0x4F:
case 0x50:
case 0x51:
case 0x52:
if(get_num_led() == ACTIVATED)  //se il led del tasterino numerico e' acceso..
{
	current_map = pad_map; //cambiamo mappa dei caratteri
	scancode -= 0x47;
key = conv_scancode_to_char(scancode);
}
else return;
break;



//per default assumiamo tutti gli altri caratteri della tastiera(a,b,c, ecc..)
default:

if(scancode >= KEY_RELEASE) //se si tratta di un tasto rilasciato
	return; //ritorniamo... semplicemente

key = conv_scancode_to_char(scancode);  //altrimenti effettuiamo la conversione
break;





}  //chiude lo switch

//Se il tasto Alt e' premuto
if(alt_flag == ACTIVATED)
{


if(current_map == pad_map)		//se e' stato premuto un numero sul tasterino numerico ...
{
	npadch = npadch * 10 + (key-'0'); //con key-'0' si traforma il  carattere(key) in un intero
	return;
}

else if(key >= '0' && key <= '9')		//se e' stato premuto un numero sulla tastiera(non sul tasterino numerico)
{
	switch_to_console((int)key-'0');	//a switch_to_console passo il carattere convertito in intero
	return;
}
	
}






buffer_kbd[pos_buffer_kbd] = key; //Memorizzo il tasto digitato in un buffer
pos_buffer_kbd++; //quindi incremento l' indice

press_key(); /*chiamo la funzione che setta ad 1 la variabile active_press_key. Cio' sta a significare che e' stato premuto un carattere o
 un numero*/

//disabilita_kbd();

	
}//chiude la funzione
/********************************************************************************
********************************************************************************/
char conv_scancode_to_char(char sc)
{
char ch;
	ch = current_map[sc]; //Converto lo scancode in un carattere
	if(ch == 0)	//se nella mappa caratteri non vi e' corrispondenza tra scancode e carattere
		return;	//ritorno
	return ch;
}
/********************************************************************************
********************************************************************************/
//Viene disabilita la ricezione degli scancode ponendo il bit 7 a 1 sulla porta 0x61
void disabilita_kbd(void)
{

unsigned char x;
x=inportb(0x61);
outportb(x|0x80, 0x61);
disabilita_irq(1, 0x21); //disabilito anche l' irq associato alla keyboard

}
/********************************************************************************
********************************************************************************/
//Viene abilitata la ricezione degli scancode ponendo il bit 7 a 0 sulla porta 0x61
void abilita_kbd(void)
{

unsigned char x;
x=inportb(0x61);
outportb(x&0x7f, 0x61);
abilita_irq(1,0x21); //abilito anche l' irq associato alla keyboard

}
/********************************************************************************
********************************************************************************/
void kb_wait(void) {
    int  i;

       while((inportb(0x64) & 0x02) != 0) ;
	
}
/********************************************************************************
********************************************************************************/
void kb_ack(void) {
    int  i,value;


    for(i=0; i<0x1000; i++) {
        value = inportb(0x60);
        if (value == KBD_ACK) 
            break;
    
    }
}
/********************************************************************************
********************************************************************************/
void set_leds(void)
{
        kb_wait();
        outportb(0xED, 0x60);
        kb_ack();
        kb_wait();
        outportb(cur_console->key_leds, 0x60);
        kb_ack();
}
/********************************************************************************
********************************************************************************/
void enable_caps_led(void)
{
cur_console->key_leds |=  0x4;
set_leds();
}
/********************************************************************************
********************************************************************************/
void enable_num_led(void)
{
cur_console->key_leds |=  0x2;
set_leds();
}
/********************************************************************************
********************************************************************************/
void enable_scroll_led(void)
{
cur_console->key_leds |=  0x1;
set_leds();
}
/********************************************************************************
********************************************************************************/
void disable_caps_led(void)
{
cur_console->key_leds ^=  0x4;
set_leds();
}
/********************************************************************************
********************************************************************************/
void disable_num_led(void)
{
cur_console->key_leds ^=  0x2;
set_leds();
}
/********************************************************************************
********************************************************************************/
void disable_scroll_led(void)
{
cur_console->key_leds ^=  0x1;
set_leds();
}
/********************************************************************************
********************************************************************************/
int get_caps_led(void)
{
if((cur_console->key_leds&0x4) == 0x4)
	return ACTIVATED;
else 
	return DISACTIVATED;
}
/********************************************************************************
********************************************************************************/
int get_num_led(void)
{

if((cur_console->key_leds&0x2) == 0x2)
	return ACTIVATED;
else 
	return DISACTIVATED;
}
/********************************************************************************
********************************************************************************/
int get_scroll_led(void)
{
if((cur_console->key_leds&0x1) == 0x1)
	return ACTIVATED;
else 
	return DISACTIVATED;
}
/********************************************************************************
********************************************************************************/
void press_enter(void)
{

active_press_enter= ACTIVATED;

}
/********************************************************************************
********************************************************************************/
void unpress_enter(void)
{

active_press_enter = DISACTIVATED;

}
/********************************************************************************
********************************************************************************/
int get_press_enter(void)
{

return active_press_enter;

}
/********************************************************************************
********************************************************************************/
void press_key(void)
{

active_press_key = ACTIVATED;

}
/********************************************************************************
********************************************************************************/
void unpress_key(void)
{

active_press_key = DISACTIVATED;

}
/********************************************************************************
********************************************************************************/
int get_press_key(void)
{

return active_press_key;

}
/********************************************************************************
********************************************************************************/
void fflush(char *buffer)
{
int i;

for(i=0;i<BUF_SIZE;i++) //inizializzo il buffer
 buffer[i]=0;
}
