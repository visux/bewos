/* Questo file, non piu' utilizzato e' una vecchia versione di driver/console.c */

#include<console.h>
#include<io.h>
#include<shell.h>
#include<signals.h>
#include<system.h> //Per le macro save_flags e restore_flags
#include<string.h> //la includiamo per la define NULL
#include<task.h>
#include<keyboard.h>
#include<time.h>
#include<syscall.h>

struct console_s console[N_CONSOLES]; //global

/* Current console: */
struct console_s *cur_console; //global



task_t process_table[MAX_TASK]; //global: e' la tabella dei processi

//La funzione sottostanti crea consoles virtuali. 
void make_console(unsigned int n)
{
	if(n >= 0 && n < N_CONSOLES)
	{
		if(n == 0)
		{
			console[n].init_video = MEM_VIDEO;
			console[n].pos_video = MEM_VIDEO; /*inizialmente la posizione assoluta corrente e' ugale proprio all' indirizzo
			 della memoria video*/
			console[n].x = 0;
			console[n].y = 0;
			console[n].attr = DEFAULT_TEXT_COLOR;
			console[n].key_leds = 0x2; //numpad attivo
			console[n].index_buffer = 0;
			memset(console[n].buffer_temp, 0, SCR_C*SCR_R);
		}
			
		else
		{
			console[n].init_video = MEM_VIDEO+VIDEO_MEM_SIZE*n;
			console[n].pos_video = MEM_VIDEO+VIDEO_MEM_SIZE*n; /*inizialmente la posizione assoluta corrente e' ugale
			 proprio all' indirizzo della memoria video*/
			console[n].x = 0;
			console[n].y = 0;
			console[n].attr = DEFAULT_TEXT_COLOR;
			console[n].key_leds = 0x2; //numpad attivo
			console[n].pid_task_shell = 0;
			console[n].index_buffer = 0;
			memset(console[n].buffer_temp, 0, SCR_C*SCR_R);
		}
	}
	
	else
	      printk("\nAttenzione: e' stato specificato un numero di console non valido. Specificare un numero di console >= 0 e < %d", \
			 N_CONSOLES);
}
/********************************************************************************
********************************************************************************/
//La funzione sottostante inizializza consoles virtuali
void init_consoles(void)
{
int i;

	for(i = 0; i < N_CONSOLES; i++)
		make_console(i);
		
	cur_console = &console[0];
	set_leds();

	klear();
	info_cur_console();
}
/********************************************************************************
********************************************************************************/
//La funzione sottostante inizializza la console corrente
void init_cur_console(void)
{
	cur_console->pos_video = cur_console->init_video; /*inizialmente la posizione assoluta corrente e' ugale proprio all' indirizzo
			 della memoria video*/
	cur_console->x = 0;
	cur_console->y = 0;
	cur_console->attr = DEFAULT_TEXT_COLOR;
	cur_console->key_leds = 0x2; //numpad attivo
	info_cur_console();
}
/********************************************************************************
********************************************************************************/
//La funzione sottostante setta l' origine della memoria video. In pratica setta un nuovo valore dell' inizio della memoria video.
static inline void set_origin(int n) {
   outportb(12,0x3d4);
   outportb(0xff&((cur_console->init_video-MEM_VIDEO)>>9),0x3d5);
    
   outportb(13,0x3d4);
   outportb(0xff&((cur_console->init_video-MEM_VIDEO)>>1),0x3d5);
   
}
/********************************************************************************
********************************************************************************/
//La funzione sottostante switcha(cambia) console
void switch_to_console(int n)
{
pid_t pid_shell_to_sleep;
long flags;
unsigned int i;

if(cur_console == &console[n])
	return; // 0=La console aperta è stata appena creata,1=già esisteva
	if(n >= 0 && n < N_CONSOLES)
	{	
		
		save_flags(flags); //salvo gli Eflags nella variabile flags attraverso la macro save_flags
		asm("cli"); //Disabilito gli interrupt
	
		/*mi prendo il pid del task shell da addormentare. Ricordo che quando si switcha console, il task shell corrente viene
		 addormentato e viene creato\"svegliato" il task shell della console a cui si desidera switchare*/
		pid_shell_to_sleep = cur_console->pid_task_shell;
		
		//switcho console
		cur_console = &console[n];   //imposto il nuovo valore di cur_console
		set_leds();	//imposto i leds
		set_origin(n);  //imposto l' origine della memoria video
		set_cursor_pos();  //imposto il cursore
		

		
		
 		//se pid_task_shell e' uguale a 0 vuol dire che non e' stato ancora creata una shell per la console corrente... quindi la creo (se cancellare lo schermo o no viene determinato da "char init_console" v.commento in main.c)
		if(cur_console->pid_task_shell == 0 || process_table[cur_console->pid_task_shell].state==KILLED) {
			cur_console->pid_task_shell = create_task(shell,"shell",USER_PRIVILEGE);
			process_table[cur_console->pid_task_shell].ppid=0; // Imposto come parent pid 0
			}  //creo il task shell
		
		else {//altrimenti vuol dire che e' gia stato creato e devo solo "svegliarlo"
		}
					wakeup_task(cur_console->pid_task_shell);
			
		
			/*Se il buffer della console al quale si e' switchato NON e' vuoto allora si stampa il buffer a video. Per il motivo per il
			 quale utilizziamo tale buffer e' spiegato nel file: console.h */
			if(cur_console->buffer_temp != NULL)
			{

				for(i = 0 ; cur_console->buffer_temp[i] != 0; i++) /* Stampiamo di volta in volta i caratteri all'
				 interno del buffer fino a quando esso non termina */
					putk(cur_console->buffer_temp[i]); 
					
			memset(cur_console->buffer_temp, 0, SCR_C*SCR_R);  //azzero il buffer
			cur_console->index_buffer = 0;  //azzero l' indice del buffer
					
			
			}
			
		restore_flags(flags);  //Ripristino gli Eflags
		
		//	asm("sti");	

	//	while(get_current_pid_task() != cur_console->pid_task_shell) ;
		
	//	sleep_task(pid_shell_to_sleep);	/*addormento il task shell della precedente console(ricordo che il pid e' stato messo nella
								 // variabile pid_shell_to_sleep prima di cambiare console */

		process_table[pid_shell_to_sleep].state = BLOCKED;    /* Blocco il task shell della precedente console(ricordo che il pid e'
		 stato messo nella  // variabile pid_shell_to_sleep prima di cambiare console */

	}
	else
	printk("Attenzione: e' stato specificato un numero di console non valido. Specificare un numero di console >= 0 e < %d", \
		N_CONSOLES); 
}
/********************************************************************************
********************************************************************************/
/* La funzione sottostante visualizza le informazioni(nella prima riga dello schermo) riguardanti la console corrente(informazioni tipo
 numled attivo\disattivo ecc..) */
void info_cur_console(void)
{
int x_temp, y_temp, n;
char attr_temp;


/*Salvo il valore di x, y e attr della console corrente*/
x_temp = cur_console->x;
y_temp = cur_console->y;
attr_temp = cur_console->attr;


	goto_xy(0,0);  //mi posizione all' inizio della memoria video della console corrente(in pratica mi posiziono all' inizio della prima riga)

	set_color(YELLOW_COLOR, BLUE_COLOR);  //imposto il colore: Giallo su Blu


	for(n=0; n<SCR_C; n++)	//inizializzo con degli spazi la riga nella quale scrivere le informazioni
		printk(" ");
	
	goto_xy(0,0);  //e mi posizione nuovamente ad inizio riga


	//Mi prendo il numero della console corrente
	for(n=0; n < N_CONSOLES; n++)
	{
		if(cur_console == &console[n])
			break;
	}


	//Stampo il numero della console corrente
	printk("N. Console: %d", n);

	
	printk("    ");


	/*Stampo informazioni riguardante il CapsLed*/
	printk("Caps: ");

	if(get_caps_led() == 1)
		printk("Attivo");
	else
		printk("Disatt.");

	
	printk("    ");


	/*Stampo informazioni riguardante il NumLed*/
	printk("Num: ");

	if(get_num_led() == 1)
		printk("Attivo");
	else
		printk("Disatt.");
	

	printk("    ");


	/*Stampo informazioni riguardante lo ScrollLed*/
	printk("Scroll: ");

	if(get_scroll_led() == 1)
		printk("Attivo");
	else
		printk("Disatt.");



	if(x_temp == 0 && y_temp == 0)  //Se prima di stampare le informazioni mi trovavo gia' ad inizio riga
		goto_xy(0, 2); /* mi posiziono alla seconda riga(perche' nella prima riga sono state scritte le informazioni) dal quale poi
		 saranno stampati tutti gli altri caratteri */

	else //altrimenti...
  		goto_xy(x_temp, y_temp);  //mi posiziono alla "posizione" che c' era prima della stampa delle informazioni


	cur_console->attr = attr_temp;  //reimposto gli attributi iniziali(che c' erano prima di aver stampato le informazioni)
}
