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
 
 /******************************************INFO******************************
* Coders: Black (idea originale), DktrKranz (reimplementazione)
* Descrizione: Funzione Delay
******************************************************************************/

#include <delay.h>
#include <timer.h>
#include <task.h>

/* 
 * La funzione delay e' strettamente collegata con il pit8253. Per questo si
 * vedano anche i file pit8253.c e timer.c
 * Ipotizziamo di voler sospendere l' esecuzione del task per 150 millisecondi.
 * La funzione delay crea innanzitutto un evento timer e lo registra nella
 * lista dei timer attivi attraverso la funzione "add_timer" e poi si sospende.
 * Le informazioni di un evento timer sono memorizzate in una struttura come
 * questa (definita nel file timer.h):
 * struct timer {
 *	unsigned long delay;		// Espressa in colpi di clock
 *	void (*handler)(void *);	// L'handler del timer
 *	void *data;			// Parametro utilizzato dall'handler
 *	struct timer *next;		// Puntatore al prossimo elemento
 * }; 
 * Questa struttura viene inizializzata settando un opportuno ritardo (nel caso
 * dell'esempio 150 millisecondi), opportunamente convertito in colpi di clock
 * per mezzo della funzione "msecs_to_ticks". I campi handler e data contengono
 * rispettivamente il puntatore alla funzione che verrà richiamata quando il
 * lasso di tempo impostato sarà scaduto e un puntatore contenente un parametro
 * che può essere utilizzato nell'handler. Dato che potrebbero esserci piu'
 * elementi registrati nella lista dei timer, vi e' anche il campo *next", il
 * quale punta all'elemento successivo della lista.
 */

void do_wakeup_task(void *pid)
{
	wakeup_task((int)pid);
}

int delay(unsigned long ms)
{
	if(!add_timer(ms, do_wakeup_task, (void *)get_current_pid_task()))
		return -1;
	
	sleep_task(get_current_pid_task());
	return 0;
}
