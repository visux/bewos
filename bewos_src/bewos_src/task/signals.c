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

/******************************************INFO*******************************
* Coders: Black
* Descrizione: Definizioni di funzioni per la gestione dei segnali
******************************************************************************/

#include <scheduler.h>
#include <signals.h>
#include <syscall.h>
#include <system.h>
#include <task.h>

void sleep_task(int pid)
{
	unsigned long flags;

	/* Salvo gli eflags e disabilito gli interrupt */
	save_flags_irq(flags);

	/* Pongo lo stato del processo in BLOCKED */
	process_table[pid].state = BLOCKED;

	/*
	 * Se il pid del task da bloccare non e' quello del task corrente,
	 * ritorniamo per far continuare l'esecuzione del task corrente.
	 */
	if(pid != get_current_pid_task()) {
		/* Ripristino gli eflags */
		restore_flags(flags);
		return;
	}

	/*
	 * Abilito lo scheduler cosi' la schedulazione di un altro processo
	 * avviene immediatamente.
	 */
	enable_scheduler();

	/* Ripristino gli eflags */
	restore_flags(flags);
		
	/*
	 * Occorre ripetere il ciclo fino a quando lo stato del task corrente,
	 * impostato a BLOCKED, non subisce una variazione.
	 */
	while(process_table[pid].state==BLOCKED) {
		/*
		 * Corretto bug #1577257
		 * Questa istruzione impedisce l'ottimizzazione da parte del
		 * compilatore. In questo modo ad ogni ciclo viene confrontato
		 * l'esatto valore e non quello memorizzato in cache.
		 */
		asm("": : :"memory");
	}

}

void wakeup_task(int pid)
{

	if(process_table[pid].state == BLOCKED)
		/* Sveglio il processo attribuendo a status il valore READY */
		process_table[pid].state = READY; 

	enable_scheduler();
}
