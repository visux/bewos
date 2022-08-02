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
* Coders: Baldosoft, WMI
* Descrizione: shell di bewos
***********************************************************************************************/


#include <io_port.h>
#include <time.h>
#include <string.h>
#include <io.h> // set_text_color,set_default_color,goto_xy
#include <malloc.h>
#include <floppy.h>
#include <syscall.h>
#include <console.h>
#include <system.h>
#include <gdt.h>
#include <task.h>
#include <vfloppy.h>

#define X_BUFF 10
#define Y_BUFF 512
/* array di 10 stringhe lunghe 512 caratteri */

#define LINE_MAXLEN (X_BUFF * Y_BUFF + Y_BUFF) /* Lunghezza massima del prompt: Lunghezza massima comandi + Y_BUFF spazi bianchi */

#define HISTORY 5 // Numero di comandi da tenere in cronologia
#define PROMPT_TEXT "BeWos:> "

#define NUMERO_COMANDI 12
const char  *COMANDI[ NUMERO_COMANDI ] =
{
    "help",
    "tnx",
    "floppy",
    "data",
    "bewos",
    "clear",
    "reboot",
    "echo",
    "debug",	//for debug
    "ps",
    "kill",
    "killall"
};


void LeggiStringa( char [X_BUFF][Y_BUFF], char*);
void LeggiDaKeyboard(char* buffer, char *history[HISTORY]);
void Esegui( char [X_BUFF][Y_BUFF]);
void Help( const int );

struct console_s *cur_console; //global
task_t process_table[MAX_TASK]; //e' la tabella dei processi: global
pid_t free_pid;  //sara' sempre uguale al prossimo pid disponibile: global

void shell()
{
	if (!init_console) { // Questa nuova shell e' quella iniziale? se no cancello lo schermo
		klear();
	}
	init_console=0;
	char *Stringa,*Keyboard;
	char *prevcommands[HISTORY]; // Cronologia dei comandi
	char OraLogOn[ 20 ];
	time_t Data;
	int a;

	Stringa = ( char * ) malloc( X_BUFF * Y_BUFF * sizeof( char )); //Riserviamo spazio per la stringa
	Keyboard = ( char * ) malloc(LINE_MAXLEN);

	// Alloco lo spazio per le stringhe della cronologia
	for (a=0; a<HISTORY; a++) {
		prevcommands[a] = ( char * ) malloc(LINE_MAXLEN);
		prevcommands[a][0] = '\0'; // Imposto ogni valore della cronologia a stringa vuota
	}

	info_cur_console();
	Data = time( NULL );
	strftime( OraLogOn, 20, "%X %x", localtime( &Data ) );
	printk("Logon on: %s\n",OraLogOn);
	while(1)
	{
		memset( Stringa, 0, X_BUFF * Y_BUFF ); /* azzeriamo la stringhe della console */
		Keyboard[0] = '\0'; // Rendo il buffer per l'input da tastiera una stringa vuota

		LeggiDaKeyboard(Keyboard, prevcommands);   /* leggiamo le stringhe dalla tastiera */
		LeggiStringa(Stringa,Keyboard);

		if(! *Stringa ) continue; /* se il primo carattere della prima stringha e' nullo ricomincia il ciclo */
		Esegui( (char(*)[Y_BUFF])Stringa ); /* elaboriamo le stringhe immesse */
	}

	for (a=0; a<HISTORY; a++) {
		free(prevcommands[a]);
	}
	
	free(Stringa);
	free(Keyboard);

}

void LeggiDaKeyboard(char* buffer, char *history[HISTORY]) {
	char *tmpBuffer = ( char * ) malloc(LINE_MAXLEN);
	unsigned char key;
	unsigned int inCrono = HISTORY; // Contiene HISTORY se non e' attualmente visualizzata una strina della cronologia, altrimenti l'offset all'interno di history[]
	if (cur_console->x != 0) printk("\n");
	printk(PROMPT_TEXT "\n");
	goto_xy(strlen(PROMPT_TEXT), cur_console->y - 1);
	int offs=0,len=0,k,j,startline = cur_console->y, righe_occupate = 1;
	int offs_backup; // Mi servira' per memorizzare la posizione del cursore durante la visualizzazione della cronologia
	cur_console->block_videomem = cur_console->pos_video; /*dopo aver stampato il prompt, inizializzo
	 block_videomem. Per maggiori informazioni si veda il file console.h */

#define ASCII_ARROW_UP 24
#define ASCII_ARROW_DOWN 25
#define ASCII_ARROW_RIGHT 26
#define ASCII_ARROW_LEFT 27
#define ASCII_DEL 127
#define ASCII_FINE 128
#define ASCII_HOME 129

	do {
		key = getche();
		if (key != ASCII_ARROW_UP && key != ASCII_ARROW_DOWN && key != ASCII_ARROW_RIGHT && key != ASCII_ARROW_LEFT) inCrono = HISTORY;
		switch (key)
		{
			case '\b': // BACKSPACE
				if (len==0 || offs==0) break;
				offs--;
				for (k = 0; k < offs; k++) tmpBuffer[k] = buffer[k];
				for (k = offs + 1; k < len; k++) tmpBuffer[k - 1] = buffer[k];
				len--;
				memcpy(buffer, tmpBuffer, len);
				break;
			
			case ASCII_ARROW_RIGHT:
				if (offs!=len) offs++;
				break;
			
			case ASCII_ARROW_LEFT:
				if (offs!=0) offs--;
				break;
			
			case '\n': // INVIO
				goto LeggiDaKeyboard_Done;
				break;
			
			case ASCII_DEL:
				if (len==0 || offs >= len ) break;
				for (k = 0; k < offs; k++) tmpBuffer[k] = buffer[k];
				for (k = offs + 1; k < len; k++) tmpBuffer[k - 1] = buffer[k];
				len--;
				memcpy(buffer, tmpBuffer, len);
				break;
			
			case ASCII_HOME:
				offs = 0;
				break;
			
			case ASCII_FINE:
				offs = len;
				break;
			
			case ASCII_ARROW_UP:
				if (inCrono == HISTORY) // Se la stringa attualmente visualizzata e' quella in fase di scrittura la salvo
				{
					strcpy(tmpBuffer, buffer);
					offs_backup = offs;
				}
				do {
					if (inCrono == 0) break;
					inCrono--;
				} while (history[inCrono][0]=='\0' && inCrono > 0);
				if (history[inCrono][0]=='\0') break; // Se non sono state trovate stringhe non vuote esco
				strcpy(buffer, history[inCrono]);
				len = strlen(buffer);
				offs = len;
				break;
			
			case ASCII_ARROW_DOWN:
				do {
					if (inCrono == HISTORY) break;
					inCrono++;
				} while (history[inCrono][0]=='\0' && inCrono < HISTORY);
				if (history[inCrono][0]=='\0') break; // Se non sono state trovate stringhe non vuote esco
				if (inCrono == HISTORY) { // Se scendendo siamo tornati alla stringa in fase di scrittura...
					strcpy(buffer, tmpBuffer); // ...la riprendo da tmpBuffer
				} else {// altrimenti procedo prendendola dalla cronologia
					strcpy(buffer, history[inCrono]);
				}
				len = strlen(buffer);
				offs = (inCrono == HISTORY) ? offs_backup : len;
				break;
			
			case '\t': break; // Il TAB viene ignorato
			
			default:
				for (k = 0; k < offs; k++) tmpBuffer[k] = buffer[k];
				tmpBuffer[offs] = key;
				for (k = offs; k < len; k++) tmpBuffer[k + 1] = buffer[k];
				offs++; len++;
				memcpy(buffer, tmpBuffer, len);
		}
	buffer[len] = '\0';

	// Ricalcolo il numero di righe occupate (considerando anche PROMPT_TEXT)
	righe_occupate = ((strlen(PROMPT_TEXT) + len)/SCR_C)+1;
	goto_xy(0, startline);
	for (j=0; j < righe_occupate; j++) printk("\n");
	startline = cur_console->y - righe_occupate;
	
	for (k=0; k < righe_occupate; k++) clear_line(startline+k);
	
	// Aggiorno la visualizzazione
	goto_xy(0, startline);
	printk(PROMPT_TEXT "%s", buffer);
	
	k = strlen(PROMPT_TEXT) + offs; // Posizione del cursore rispetto al primo carattere di PROMPT_TEXT
	goto_xy(k%SCR_C, startline+(k/SCR_C));
	} while (1);

LeggiDaKeyboard_Done: // Procedura per uscire dall'aquisizione caratteri dalla keyboard
	free(tmpBuffer);
	
	if (buffer[0] != '\0')
	{ // Se il comando immesso non e' una stringa vuota e non e' uguale al precedente (TODO) vuota lo salvo in cronologia
		for (k=1; k < HISTORY; k++) strcpy(history[k-1], history[k]); // Slitto indietro tutte le righe in cronologia cancellando la prima
		strcpy(history[HISTORY-1], buffer); // Aggiungo la nuova riga appena digitata in cronologia
	}
}

// Trasforma una stringa in un comando suddividendo nome del comando e parametri
void LeggiStringa( char Stringa[X_BUFF][Y_BUFF], char* Input) {
unsigned int x,y;  
unsigned int flag;

for( x = 0, y = 0, flag = 0; x <= X_BUFF && y <= Y_BUFF; ) {
	Stringa[x][y] = *Input++;
	if( Stringa[x][y] == '\b' ) {
		if(x == 0 && y == 0) continue;
		Stringa[x][y] = 0;
	
		if( y == 0 ) {
			x--;
			y = strlen( Stringa[x] );
			Stringa[x][y] = 0;
			continue;
		} else {
			y--;
			Stringa[x][y] = 0;
			continue;
		}
	}
	if(Stringa[x][y]=='"') {
		if(flag==1) flag=0; else flag=1;
		y--;
	}
	if( Stringa[x][y] == '\0' ) break;
	if (Stringa[x][y] == ' ' && strspn(Stringa[x]," ") != strlen(Stringa[x]) && flag == 0 ) {
		Stringa[x][y] = 0;
		x++;
		y = 0;
	} else {
		y++;
	}
}
if(y >= Y_BUFF)	printk("\n");
}


void Esegui( char Stringa[X_BUFF][Y_BUFF] )
{

int NumeroComando;

// Variabili usate dalle routines
int x;
char c;
pid_t task_index;
unsigned int counter;
time_t DataCorrente;

    for( NumeroComando = 0 ; NumeroComando < NUMERO_COMANDI; NumeroComando++ )
        if( strcasecmp( COMANDI[ NumeroComando ], Stringa[0] ) == 0 ) break;

/* confronta le stringhe immesse con i comandi disponibili */

    if( NumeroComando == NUMERO_COMANDI )
    {
        printk("Shell: Comando \"%s\" non trovato. \nDigitare \"help\" per l'elenco dei comandi disponibili\n", Stringa[0] );
        return;
    }

    switch( NumeroComando )
    {
        case 0:
            if(! *Stringa[1] )
            {
	    	set_text_color(CYAN_COLOR);	//Colore Cyan
                printk("\n\t|Comando|\t\t"); printk("\t\t |Descrizione|\n");
		set_default_color(); //Reimposto il colore di default
                printk("   -------------------------------------------------------------------------\n");
                printk("\n\thelp\t\t\t    Visualizza informazioni sui comandi");
                printk("\n\techo\t\t\t    Visualizza un messaggio");
                printk("\n\ttnx\t\t\t     Ringraziamenti");
                printk("\n\tfloppy\t\t\t  Visualizza il tipo di floppy inserito");
                printk("\n\tdata\t\t\t    Visualizza/modifica la data di sistema");
		printk("\n\tps\t\t\t      Visualizza la lista dei processi");
		printk("\n\tkill\t\t\t    Uccide un processo");
		printk("\n\tkillall\t\t\t Uccide processi in base al loro nome");
                printk("\n\tbewos\t\t\t   Visualizza la versione di B&Wos");
                printk("\n\tclear\t\t\t   Pulisce lo schermo");
                printk("\n\treboot\t\t\t  Riavvia il computer");
                printk("\n\n   -------------------------------------------------------------------------");
                printk("\n\n");
            }
            else
            {
                 int Comando;

                 for( Comando = 0 ; Comando < NUMERO_COMANDI ; Comando++ )
                     if( strcasecmp( COMANDI[ Comando ], Stringa[1] ) == 0 ) break;

                if( Comando == NUMERO_COMANDI )
                {
                   printk("Shell: Help: Comando \"%s\" non trovato. \nDigitare \"help\" per l'elenco dei comandi disponibili\n", Stringa[1] );
                   return;
                }
                
                Help( Comando );
            }
            
            break;
        
        case 1: // tnx
            printk("\nVogliamo ringraziare tutti coloro che ci hanno aiutato e che ci aiuteranno nell'impresa. Thanks !!\n\n\n");
            break;

        case 2: // floppy
        {
            unsigned char TipoFloppy;
            unsigned char NumeroDrive;
            if(! *Stringa[1] ) // Se non sono stati passati argomenti
            {
                printk("Shell: Il comando \"%s\" richiede un parametro\n", COMANDI[ NumeroComando ] );
                Help( NumeroComando );
                return;
            }
            
            if( strlen( Stringa[1] ) > 1 || *Stringa[1] > '2' || *Stringa[1] < '0' ) // Se l'argomento non e' valido (per essere valido deve essere un valore numerico maggiore o uguale a 0 e minore o uguale a 2) 
            {
                printk("Shell: Sintassi errata - Il numero del drive puo' essere da 0,1 o 2\n" );
                Help( NumeroComando );
                return;
            }
            
            NumeroDrive = (*Stringa[1]-'0'); //conv_string_int( Stringa[1] ) Salvo in NumeroDrive il numero del drive richiesto dall'utente

		/*	In un computer possono esserci fino a due lettori di floppy (drive 1 e drive 2)
			In bewos e' anche presente un ram disk con le stesse caratteristiche di un floppy da 1.44 (drive 0)
		*/
	    if (NumeroDrive==0) { // Drive 0 e' l'unita' virtuale
		printk("%s (Unita' virtuale - Ram disk)\n",S_FLOPPY[4] ); // Stampo la descrizione di un floppy di tipo 4 cioe' un floppy da 3,5 pollici di 1,44 mb
		return;
	    }

	    // Gli altri drives sono reali e quindi interrogo il driver del floppy (floppy.c) per sapere il tipo di drive
            TipoFloppy = GetTypeFloppy( NumeroDrive );
            printk("Drive %i: ", NumeroDrive );
	    if(TipoFloppy>0) // Se TipoFloppy e' maggiore di 0 
                printk("%s (Unita' reale)\n",S_FLOPPY[ TipoFloppy ] );
            else // Se TipoFloppy e' uguale a 0
                printk("Nessun floppy inserito o drive inesistente\n");

            break;
        }

        case 3: // data
        {
            DataCorrente = time( NULL );
            printk("Data: %s", ctime( &DataCorrente ) );
            break;
        }
        
        case 4: // bewos
            printk("\n\t\t   Black & White Operating System v. 0.2\n\n\n");
            break;
        
        case 5: // clear
            klear();
            break;
        
        case 6: // reboot
            outportb( 0xFE, 0x64 );
            break;
        
        case 7: // echo
            if( *Stringa[1] == 0 ) {
				printk("Shell: Il comando \"%s\" richiede un parametro\n", COMANDI[ NumeroComando ] );
                Help( NumeroComando );
                break;
            }
            
            for( counter = 1 ; Stringa[counter][0] ; printk("%s ",Stringa[counter++]));
            
			printk("\n");
            break;
		
		/** Per inserire una funzione di debug decommentare questa zona di codice e inserirla di seguito
	    case 8: // debug
		    printk("Questa �la funzione di debug\n");
		    break;
		*/
				
	    case 9: // ps
		    c=(strcmp(Stringa[1],"-e")==0); // Comparo il primo argomento scritto dall'utente con "-e", ricordo che strcmp torna 0 se le stringhe sono uguali quindi facendo (strcmp(Stringa[1],"-e")==0) avro' 0 se le stringhe sono diverse, 1 se sono uguali quindi: se e' 0 si viualizzano solo i processi della console corrente, se 1 tutti
		    set_text_color(CYAN_COLOR);	//Colore Cyan
		    printk("\n\tNome\t  Pid     PPid       Stato\t  Privilegio  Console\n"); // Stampo l'intestazione delle colonne
		    set_default_color(); //Reimposto il colore di default
		    
		    //Stampo la lista dei processi
		    for(task_index = (!c); task_index < free_pid; task_index++) {/* 
			task_index = (!c) significa:
				se c=1, cioe' voglio visualizzare tutti i processi, task_index verra' inizializzato a 0
				se c=0, cioe' voglio visualizzare solo i processi della console corrente, task_index verra' inizializzato a 1, in modo da saltare il processo con pid 0, cioe' il task "Idle"
			free_pid contiene sempre un pid non usato, quindi se ho i processi 0 e 1, per esempio, free_pid=2
			posso usare quindi free_pid come modo per sapere quante entry ci sono nella process_table */
			if (c || process_table[task_index].console==cur_console) { /*
				Grazie a questa condizione decido se un processo deve essere visualizzato o meno:
				se c==1, cioe' voglio stampare tutti i processi in esecuzione in tutte le consoles, la condizione sara' vera e quindi il processo considerato verra' stampato
				invece, se c==0, verifico se la console a cui appartiene il processo e' quella corrente, se e' cosi' la condizione sara' vera e quindi il processo considerato verra' stampato
				*/

				// Nome
				printk("\n\t%s", process_table[task_index].name); // Stampo il nome del processo

				// Pid
				goto_xy(21, cur_console->y); // Porto la posizione di stampa alla colonna 21
				printk(" %d",   process_table[task_index].pid); // Stampo il pid del processo

				// PPid
				goto_xy(30, cur_console->y); // Porto la posizione di stampa alla colonna 30
				if (task_index) // if (task_index > 0) cioe' se il processo considerato non ha come pid 0, cioe' non e' il task idle
					printk("%d", process_table[task_index].ppid); // Stampo il pid del processo parent 
				else
					printk("N/A"); // Altrimenti (cioe' se sto considerando il task Idle) stampo "N/A"

				// Stato
				goto_xy(33, cur_console->y); // Porto la posizione di stampa alla colonna 33
				switch (process_table[task_index].state) { // Con questo switch stampo la condizione del processo considerato
					case READY:	printk("\tReady"); break;	// Ready
					case RUNNING:	printk("\tRunning"); break;	// Running
					case BLOCKED:	printk("\tBlocked"); break;	// Blocked
					case KILLED:	printk("\tKilled"); break;	// Killed
					default:	printk("\tSconosciuto");	// Per valori non validi
				}

				// Privilegio
				goto_xy(48, cur_console->y); // Porto la posizione di stampa alla colonna 48
				switch (process_table[task_index].privilege) { // Con questo switch stampo il privilegio del processo
					case SUPERVISOR_PRIVILEGE:	printk("\tSupervisor"); break;	// Supervisor
					case USER_PRIVILEGE:		printk("\tUser"); break;	// User
					default:			printk("\tSconosciuto");	// Per valori non validi
				}

				// Console
				goto_xy(68, cur_console->y); // Porto la posizione di stampa alla colonna 68
					
					if (task_index) // if (task_index > 0) cioe' se il processo considerato non ha come pid 0, cioe' non e' il task idle
						printk("%d", process_table[task_index].console-console); /* Per calcolare a che console appartene il processo basta leggere il puntatore process_table[task_index].console, e sottrarre l'indirizzo base della tabella delle consoles*/
					else
						printk("N/A"); // Altrimenti (cioe' se sto considerando il task Idle) stampo "N/A"
					}

		    }
		    printk("\n\n");
			break;
		
		case 10: // kill
		    if (*Stringa[1] != 0 && (Stringa[1][0]>='0' && Stringa[1][0]<='9') && ((Stringa[1][1]>='0' && Stringa[1][1]<='9') || Stringa[1][1]==0) && ((Stringa[1][2]>='0' && Stringa[1][2]<='9') || Stringa[1][2]==0) && Stringa[1][3]==0) { // Con questa condizione verifico che il primo parametro sia un valore numerico non maggiore di 999
				task_index=conv_string_int(Stringa[1]); // Salvo in task_index il valore sotto forma di intero immesso dall'utente come argomento
				set_text_color(RED_COLOR); // Colore rosso
				if (task_index==0) { // Faccio in modo che se l'utente vuole killare il task Idle non puo'
					printk("Non si puo' uccidere il processo 'Idle'\n");
				} else if (process_table[task_index].state==RUNNING) { // Cosi' impedisco che venga ucciso il processo in fase di esecuzione (cioe' quello che sta eseguendo "kill")
					printk("Non si puo' uccidere il processo in esecuzione\n");  
				} else if (process_table[task_index].state!=READY && process_table[task_index].state!=BLOCKED) { /* Quindi in definitiva si possono uccidere soltanto i processi in stato READY o BLOCKED, uso questo elseif per evitare che processi che non siano in uno di questi due stati venga ucciso */
					printk("Il processo n. %d non esiste o e' stato gia' ucciso\n",task_index);
				} else { // Il processo puo' essere killato
					kill_process(task_index); // Killo tramite la syscall "kill_process" il processo
					set_text_color(GREEN_COLOR); // Colore Verde
					printk("Il processo \"%s\" (%d) e' stato ucciso\n",process_table[task_index].name,task_index);
				}
				set_default_color(); // Imposto il colore di default
		    } else { // Se non sono stati passati argomenti a kill oppure non sono argomenti validi
	            printk("Shell: Sintassi errata\n" );
	            Help( NumeroComando );
		    }
			break;
		
		case 11: // killall
			x=1; // Posizione in Stringa[] del nome del processo da uccidere. La imposto a 1 in modo da indicare il primo parametro passato a killall
			if (strcmp(Stringa[1],"-I")==0 || strcmp(Stringa[1],"--ignore-case")==0) {
				// Se il primo parametro e' -I o --ingore-case, imposto l'offset in Stringa[] del nome alla posizione successiva (la 2)
				x=2;
			}
			/* Tramite la variabile x posso quindi sapere anche se devo fare una comparazione case-sensitive(x=1) o non case-sensitive (x=2) */
			
			if (Stringa[x]) { // Se ho una stringa di nome valida
				c=0; // c tiene il conto del numero di processi uccisi
				if (x==1) { // Case sensitive
					for (task_index=1;task_index< free_pid; task_index++) { // Eseguo la ricerca partendo da PID 1, perche' non posso uccidere il task idle (PID=0)
						if (strcmp(Stringa[x],process_table[task_index].name)==0) {
							if (process_table[task_index].state==READY || process_table[task_index].state==BLOCKED) {
								kill_process(task_index);
								// debug :: printk("Il processo \"%s\" (%d) e' stato ucciso\n",process_table[task_index].name,task_index);
								c++;
							}
						}
					}
				} else if (x==2) { // Non case sensitive
					for (task_index=1;task_index< free_pid; task_index++) { // Eseguo la ricerca partendo da PID 1, perche' non posso uccidere il task idle (PID=0)
						if (strcasecmp(Stringa[x],process_table[task_index].name)==0) {
							if (process_table[task_index].state==READY || process_table[task_index].state==BLOCKED) {
								kill_process(task_index);
								// debug :: printk("Il processo \"%s\" (%d) e' stato ucciso\n",process_table[task_index].name,task_index);
								c++;
							}
						}
					}
			}
			
			
			if (c==0) {
				printk("Nessun processo chiamato \"%s\"\n",Stringa[x]);
			}
			
			} else {
				printk("Shell: Sintassi errata\n" );
	            Help( NumeroComando );
			}
		
			break;
	   
        
        default:
            printk("Shell: Comando non ancora implementato\n");
    }
}

void Help( const int NumeroComando )
{
    printk("Comando \"%s\": ", COMANDI[ NumeroComando ] );

    switch( NumeroComando )
    {
	case 0:
		printk("Visualizza la funzione del comando e la sua sintassi.\n");
		printk("Sintassi: help [ nome comando ]\n");
		break;
	
	case 1:
		printk("Visualizza i ringraziamenti\n");
		printk("Sintassi: tnx.\n");
		break;
	
	case 2:
		printk("Visualizza il tipo di floppy inserito nel drive n (0,1 o 2)\n");
		printk("Sintassi: floppy n\n");
		break;
	
	case 3:
		printk("Visualizza la data e l'ora di sistema\n");
		printk("Sintassi: data\n");
		break;
	
	case 4:
		printk("Visualizza la versione di BeWos\n");
		printk("Sintassi: bewos\n");
		break;
	
	case 5:
		printk("Pulisce lo schermo\n");
		printk("Sintassi: clear\n");
		break;
	
	case 6:
		printk("Riavvia il computer\n");
		printk("Sintassi: reboot\n");
		break;
	
	case 7:
		printk("Mostra il messaggio immesso dalla shell\n");
		printk("Sintassi: echo <messaggio>\n");
		break;
	
	case 8:
		printk("Crea un task di debug\n");
		printk("Sintassi: debug\n");
		break;
	
	case 9:
		printk("Visualizza la lista dei tasks di questa console o di tutte (-e)\n");
		printk("Sintassi: ps [-e]\n");
		break;
		
	case 10:
		printk("Uccide un processo\n");
		printk("Sintassi: kill PID\n");
		break;
			
	case 11:
		printk("Uccide tutti i processi con il nome specificato\n");
		printk("Sintassi: killall [-I,--ingore-case] nome\n");
		break;

	default:
		printk("Shell: Help: Comando \"%s\" non documentato\n");
    }
}
