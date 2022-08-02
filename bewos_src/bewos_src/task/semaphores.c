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
* Coders: Black, DktrKranz
* Descrizione: Definizioni di funzioni per la gestione dei semafori
******************************************************************************/ 

#include <atomic.h>
#include <kmalloc.h>
#include <semaphores.h>
#include <signals.h>
#include <system.h>
#include <task.h>
#include <wait.h>
 
/*
 * Quando n (n > 1) processi richiedono l'uso di una data risorsa non
 * condivisibile, � necessario regolarne l'accesso in maniera tale che:
 * - Nessun altro processo possa usare la risorsa se questa � gi� in uso
 * - La risorsa rimanga al processo che l'ha ottenuta finch� questo non decida
 *   di rilasciarla 
 * - Al rilascio, la risorsa venga resa disponibile agli altri processi
 * Il meccanismo software che realizza queste condizioni � il semaforo o mutex
 * Un mutex � costituito dai seguenti elementi:
 * - Una variabile di stato, chiamata variabile semaforica, che pu� assumere
 *   soltanto i valori 0 e 1
 * - Una lista di processi sospesi che attendono di usare la risorsa.
 * La variabile di stato ha il seguente significato:
 * - 0, Occupato. La risorsa � impegnata
 * - 1, Libero. La risorsa � disponibile
 * Un processo che vuole accedere ad una risorsa gestita da un mutex deve
 * eseguire in successione le seguenti istruzioni:
 * - down_sem(semaforo) 
 * - Manipolazione della sezione critica
 * - up_sem(semaforo)
 * Se down_sem trova la variabile di stato a 0, l'esecuzione del processo viene
 * sospesa e passa nella lista d'attesa del mutex. Se invece la variabile � a
 * 1, allora la risorsa � disponibile; in questo caso la routine down_sem porr�
 * tale variabile a 0 in modo da impedire ad altri processi di accedere alla
 * risorsa. Al rilascio della risorsa il processo chiama la routine up_sem che
 * provveder� a porre in stato READY (pronto) il primo task in attesa sulla
 * lista del semaforo e a porre la variabile di stato a 1.
 * In alcuni casi si pu� verificare che alla risorsa possano accedere un numero
 * limitato di processi contemporaneamente (p.e. perch� sono presenti pi� unit�
 * della risorsa). Il meccanismo software che gestisce questa situazione � il
 * semaforo n-ario. Un semaforo n-ario � un semaforo, la cui la variabile di
 * stato pu� assumere un intervallo di valori interi compresi fra 0 e n. L'uso
 * della risorsa porta al decremento della variabile, mentre il suo rilascio
 * ne comporta il suo incremento.
 * Lo stato di un semaforo n-ario � "libero" quando la variabile di stato �
 * maggiore di 0 e "occupato" quando � uguale a zero. Tale variabile viene
 * decrementata automaticamente di uno ad ogni chiamata di down_sem, aumentata
 * di uno ad ogni chiamata di up_sem.
 * Il mutex � un caso particolare del semaforo n-ario, in cui il numero massimo
 * di unit� � 1 (il mutex viene chiamato anche semaforo binario).
 */

static void do_wakeup(void *data)
{
	wakeup_task((pid_t)data);
}

/*
 * Questa funzione permette di allocare e inizializzare un semaforo. Il valore
 * espresso da value sono le risorse disponibili.
 */
sem_t *create_sem(int value)
{
	int i;
	sem_t* semaphore;
	
	/* Alloca la memoria per il semaforo. Ritorna NULL in caso di errore */
	if(!(semaphore = (sem_t *)kmalloc(sizeof(sem_t))))
		return NULL;
	
	/* Inizializzo la variabile di stato */
	atomic_set(&semaphore->resources, value);

	/*
	 * Inizializzo la lista dei task bloccati.
	 * Ritorna NULL in caso di errore.
	 */
	if(init_wait_list(&semaphore->waiting, do_wakeup)) {
		kfree(semaphore);
		return NULL;
	}

	/* Ritorna il semaforo appena creato */
	return semaphore;
}

/*
 * Questa funzione permette di deallocare un semaforo creato precedentemente.
 * Nota bene: il chiamante deve garantire che nessuna risorsa stia utilizzando
 * il semaforo, dato che la memoria da lui occupata sta per essere rilasciata.
 */
void delete_sem(sem_t* semaphore)
{
	/*
	 * Sveglia tutti i processi in attesa, se presenti.
	 * Teoricamente questo non dovrebbe mai succedere dato che � molto
	 * facile creare delle race conditions risvegliando tutti i processi
	 * in attesa delle risorse, ma ci� � necessario per evitare deadlock.
	 */
	wait_wakeup_all(&semaphore->waiting);
	/* Libera la memoria occupata dal semaforo */
	kfree(semaphore);
}

/*
 * Questa funzione permette di decrementare il numero delle risorse disponibili
 * gestite dal semaforo scelto. Il semaforo deve essere stato precedentemente
 * inizializzato da una chiamata a create_sem().
 * Limitazioni: la funzione non pu� essere invocata in un interrupt context.
 */
void down(sem_t *semaphore)
{
	/* Ottieni il pid del processo richiedente */
	pid_t pid = get_current_pid_task();
	/* 
	 * Decrementa atomicamente il numero di risorse disponibili e ne
	 * controlla il valore. Se non c'� bisogno di bloccare alcun processo,
	 * ritorna immediatamente. Questo accade quando il numero di risorse
	 * disponibili � maggiore o uguale a zero e quindi nessun processo deve
	 * essere messo in attesa nella lista dei task bloccati.
	 */
	while(atomic_dec_and_test_neg(&semaphore->resources)) {
		unsigned long flags;

		/* Disabilita gli interrupt */
		save_flags_irq(flags);
		/*
		 * Tenta di aggiungere il pid del processo richiedente alla
		 * lista dei task bloccati. Se ci� non � possibile, non
		 * effettuare un nuovo inserimento per scongiurare un deadlock.
		 */
		wait_add_unique(&semaphore->waiting, (void *)pid);
		/* Incrementa il numero delle risorse disponibili */
		atomic_inc(&semaphore->resources);
		/* Riabilita gli interrupt */
		restore_flags(flags);
		/*
		 * Poni il processo in stato di sleep. Verr� risvegliato non
		 * appena la risorsa controllata dal semaforo sar� libera.
		 */
		sleep_task(pid);
	}

}

/*
 * Questa funzione permette di incrementare il numero delle risorse disponibili
 * gestite dal semaforo scelto. Il semaforo deve essere stato precedentemente
 * inizializzato da una chiamata a create_sem().
 * Limitazioni: la funzione non pu� essere invocata in un interrupt context.
 */
void up(sem_t *semaphore)
{
	/* Incrementa atomicamente il numero di risorse disponibili.*/
	atomic_inc(&semaphore->resources);
	/*
	 * Se almeno un task � presente nella lista dei processi in attesa
	 * della risorsa controllata dal semaforo, procedi a risvegliare
	 * il primo elemento della lista.
	 */
	if(waiting_objects(&semaphore->waiting)) {
		unsigned long flags;
		/* Disabilita gli interrupt */
		save_flags_irq(flags);
		/* Sveglia il primo processo in attesa del semaforo */
		wait_wakeup_next(&semaphore->waiting);
		/* Riabilita lo scheduler */
		enable_scheduler();
		/* Riabilita gli interrupt */
		restore_flags(flags);
	}

}

/*
 * Questa funzione � l'handler della syscall do_sem. Riceve come parametri
 * l'operazione da svolgere (DOWN_SEM o UP_SEM) e il semaforo su cui operare.
 */
void sys_do_sem(int op, sem_t *semaphore)
{
	
	switch(op) {
		case DOWN_SEM:
		/* Decrementa le risorse disponibili */
		down(semaphore);
		break;
	case UP_SEM:
		/* Incrementa le risorse disponibili */	
		up(semaphore);
		break;
	default:
		printk("Operazione sui semafori non valida\n");
	}
}
