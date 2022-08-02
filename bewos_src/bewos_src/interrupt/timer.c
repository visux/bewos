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
 
/************************************INFO*************************************
* Coders: DktrKranz
* Descrizione: Definizioni di funzioni per la gestione del timer interrupt
******************************************************************************/

#include <kmalloc.h>
#include <pic8259.h>
#include <system.h>
#include <task.h>
#include <time.h>
#include <timer.h>

/* Numero dei cicli di clock del processore */
static unsigned long timer_ticks;

/* Lista dei timer registrati */
static struct timer timer_list;

/*
 * Aggiunge un elemento alla lista dei timer registrati.
 * Ritorna il puntatore al timer appena registrato in caso di successo, NULL
 * nel caso in cui la registrazione non abbia avuto luogo.
 */
struct timer *add_timer(unsigned long ms, void(*handler)(void *), void *data)
{
	unsigned long flags;
	struct timer *this, *timer;
	
	/* Alloca la memoria per il nuovo timer */
	if(!(timer = kmalloc(sizeof(struct timer))))
		return NULL;
	
	/* Inizializza il timer */
	timer->delay = msecs_to_ticks(ms);
	timer->handler = handler;
	timer->data = data;
	timer->next = NULL;
	
	save_flags_irq(flags);
			
	/* Scorri la lista fino a raggiungerne la coda e inserisci il timer */
	for(this = &timer_list; this; this = this->next) {

		/* Se l'elemento successivo non esiste, siamo in coda */
		if(!this->next) {
			/*
			 * Aggiorna i puntatori in modo che l'ultimo elemento
			 * della lista sia il timer da registrare.
			 */
			this->next = timer;
			break;
		}

	}
		
	restore_flags(flags);
	return timer;
}

/*
 * Rimuove un elemento precedentemente registrato nella lista dei timer.
 * In caso il timer in questione non fosse presente nella lista, nessuna
 * operazione viene compiuta.
 */
void del_timer(struct timer *timer)
{
	unsigned long flags;
	struct timer *this, *prev;
	
	save_flags_irq(flags);
	/* Punta all'inizio della lista */
	prev = &timer_list;
	
	for(this = timer_list.next; this; this = this->next) {
		
		/*
		 * Per poter rimuovere il timer devono essere verificate due
		 * differenti condizioni. In primo luogo il timer non deve
		 * essere già scaduto (this->delay > 0), in modo da evitare di
		 * cancellare un elemento in uso. Infine il timer preso in
		 * considerazione deve essere quello passato per parametro.
		 */
		if(this->delay && this == timer) {
			prev->next = this->next;
			kfree(this);
			break;
		}
		
		prev = prev->next;
	}
	
	restore_flags(flags);
}

/*
 * Modifica un elemento precedentemente registrato nella lista dei timer.
 * In caso il timer in questione non fosse presente nella lista, nessuna
 * operazione viene compiuta.
 */
struct timer *mod_timer(struct timer *timer, unsigned long ms)
{
	struct timer *this;
	unsigned long flags;
	
	save_flags_irq(flags);
	
	for(this = timer_list.next; this; this = this->next) {
		
		/*
		 * Per poter modificare il timer devono essere verificate due
		 * differenti condizioni. In primo luogo il timer non deve
		 * essere già scaduto (this->delay > 0), in modo da evitare di
		 * alterare un elemento in uso. Infine l'ID del timer in
		 * questione deve essere identico al valore del parametro id.
		 */
		if(this->delay && this == timer) {
			/* Aggiorna il campo delay */
			this->delay = msecs_to_ticks(ms);
			break;
		}
		
	}
	
	restore_flags(flags);

	/*
	 * Il puntatore this assume il valore del timer modificato se l'oggetto
	 * passato come parametro è presente in lista ed è stato modificato,
	 * NULL in caso non fosse presente nella lista.
	 */
	return this;
}

/* Stampa l'ora in alto a destra dello schermo */
static void orologio(void)
{
	char Ora[20];
	char String[] = "Ora: ";
	char *PntString = String;
	char *Video;
	char *PntOra = Ora;
	time_t Data;
	
	Data = time(NULL);
	strftime(Ora, 20, "%X", localtime(&Data));
	Video = (char*)cur_console->init_video + 320 - 
		(strlen(PntString) + strlen(PntOra)) * 2;
	
	while(*PntString) {
		*Video++ = *PntString++;
		*Video++ = YELLOW_COLOR | (BLUE_COLOR  << 4);
	}
	
	while(*PntOra) {
		*Video++ = *PntOra++;
		*Video++ = YELLOW_COLOR | (BLUE_COLOR  << 4);
	}
	
}

/*
 * Questa funzione viene invocata direttamente dal timer interrupt.
 * Essa si occupa di porre in esecuzione i timer precedentemente registrati e
 * di aggiornare l'orologio.
 *
 * Implementazione:
 * Il PIT di B&Wos invia ogni 10ms un impulso(un interrupt timer); all'inizio
 * dell' isr (funzione di gestione) dell' interrupt timer viene controllata la
 * lista degli eventi timer. Per ogni elemento della lista viene controllato il
 * campo "delay". Se questo e' maggiore o uguale al valore di timer_ticks,
 * allora viene chiamata la funzione puntata da handler. Dopo aver compiuto
 * questa fase, il timer scaduto viene rimosso dalla lista.
 */
void isr_timer(void)
{
	struct timer *this;
	unsigned long flags;

	save_flags_irq(flags);
	
	/*
	 * Scorri la lista e per ogni elemento decrementa di un'unità il valore
	 * dell'attributo delay. Se esso raggiunge zero, il timer è scaduto e
	 * occorre mettere in esecuzione l'handler, se presente. Infine è
	 * necessario rimuovere l'elemento dalla lista e rilasciare la memoria.
	 */
	for(this = timer_list.next; this; this = this->next) {

		if(!(--this->delay)) {
			
			if(this->handler)
				this->handler(this->data);
			
			timer_list.next = this->next;
			kfree(this);
		}
	
	}
	
	/* Riabilita lo scheduler */
	enable_scheduler();

	/* Aggiorna l'orologio ogni secondo */
	if(!(++timer_ticks % MS_PIT))
		orologio();

	restore_flags(flags);
}

/* Questa funzione ritorna il numero di esecuzioni del timer interrupt */
inline unsigned long show_timer_ticks(void)
{
	return timer_ticks;
}

/*
 * Questa funzione è l'handler della system call sys_timer.
 * Riceve in ingresso il tipo di operazione da compiere, un puntatore al timer
 * da manipolare, l'handler e un puntatore a valore.
 * Ritorna un puntatore a timer o BAD_TIMER in caso di errore
 */
struct timer *sys_do_timer(int op, struct timer *timer, unsigned long ms,
			   void(*handler)(void *), void *data)
{

	switch(op) {
	case TIMER_ADD:
		/* Aggiunge un timer. Il parametro timer viene ignorato */
		return add_timer(ms, handler, data);
	case TIMER_DEL:
		/* Rimuove un timer. Solo il parametro timer è considerato */
		del_timer(timer);
		return NULL;
	case TIMER_MOD:
		/*
		 * Modifica un timer precedentemente registrato.
		 * I parametri handler e data vengono ignorati.
		 */
		return mod_timer(timer, ms);
		break;
	default:
		return BAD_TIMER;
	}

}
