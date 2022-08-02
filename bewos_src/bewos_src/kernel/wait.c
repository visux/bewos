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
* Descrizione: Definizioni di funzioni per la gestione di wait list
******************************************************************************/

#include <atomic.h>
#include <kmalloc.h>
#include <task.h>
#include <wait.h>

/*
 * Una wait list è un elenco di oggetti in attesa di un determinato evento. A
 * differenza di un timer, la cui scadenza è stabilita con certezza nel futuro
 * (vedere il file timer.c), un oggetto in una wait list viene processato solo
 * quando il chiamante lo ritiene più opportuno.
 * Ogni wait list è caratterizzata da una lista di oggetti in attesa e da un
 * handler comune a ogni oggetto in essa registrato. Per discriminare quale
 * oggetto prendere in considerazione, viene utilizzato un attributo di nome
 * data che rappresenta un puntatore ad una risorsa che verrà gestita dall'
 * handler di volta in volta. Essendo un puntatore a void, è possibile passare
 * all'handler qualsiasi valore utilizzando un semplice cast.
 */

/* Questa funzione permette di inizializzare una wait list */
int init_wait_list(struct wait *wait, void (*handler)(void *))
{
	/* Se il gestore è invalido, avvisa il chiamante e ritorna */
	if(!handler) {
		printk("Attenzione: nessun gestore per la wait list\n");
		return -1;
	}
	
	/* Inizializza la wait list */
	atomic_set(&wait->count, 0);
	wait->handler = handler;
	memset(&wait->objects, 0, sizeof(wait_obj_t));
	return 0;
}

/*
 * Questa funzione inserisce un nuovo oggetto in coda alla wait list.
 * Nota bene: la funzione non implementa meccanismi di sincronizzazione per
 * evitare race conditions. Tali operazioni sono a carico del chiamante.
 */
int wait_add_object(struct wait *wait, void *data)
{
	wait_obj_t *object, *this;
	
	/* Punta al primo oggetto della lista */
	this = &wait->objects;
	
	/* Alloca la memoria per l'oggetto. Ritorna in caso di errore */
	if(!(object = (wait_obj_t *)kmalloc(sizeof(wait_obj_t))))
		return -1;
	
	/* Inizializza l'oggetto */
	object->data = data;
	object->next = NULL;
	
	/* Scorri la lista fino a raggiungerne la coda */
	while(this->next)
		this = this->next;
	
	/* Inserisci l'oggetto in coda alla wait list */	
	this->next = object;
	/* Incrementa il numero di elementi in lista */
	atomic_inc(&wait->count);
	return 0;
}

/*
 * Questa funzione inserisce un nuovo oggetto in coda alla wait list.
 * A differenza della funzione wait_add_object, in questa circostanza viene
 * effettuato un controllo di ridondanza: se il valore passato come parametro
 * è identico all'attributo di un oggetto precedentemente registrato, si
 * ritorna immediatamente.
 * Nota bene: la funzione non implementa meccanismi di sincronizzazione per
 * evitare race conditions. Tali operazioni sono a carico del chiamante.
 */
int wait_add_unique(struct wait *wait, void *data)
{
	wait_obj_t *object, *this;
	
	/* Scorri la lista */
	for(this = &wait->objects; this->next; this = this->next) {
			
		/* 
		 * Se un oggetto contiene lo stesso valore del parametro data,
		 * ritorna senza inserirlo nella wait list
		 */
		if(!memcmp(this->next->data, data, sizeof(this->next->data)))
			return -1;
	
	}
	
	/* Alloca la memoria per l'oggetto. Ritorna in caso di errore */
	if(!(object = (wait_obj_t *)kmalloc(sizeof(wait_obj_t))))
		return -1;
	
	/* Inizializza l'oggetto */
	object->data = data;
	object->next = NULL;
	
	/*
	 * Inserisci l'oggetto in coda alla wait list.
	 * Il puntatore punta alla fine della lista, dato che nella precedente
	 * iterazione è già stato correttamente referenziato.
	 */	
	this->next = object;
	/* Incrementa il numero di elementi in lista */
	atomic_inc(&wait->count);
	return 0;
}

/*
 * Questa funzione sveglia uno o più oggetti presenti nella wait list. Gli
 * oggetti da prendere in considerazione sono quelli il cui attributo data è
 * uguale all'omonimo parametro passato dal chiamante.
 * Nota bene: la funzione non implementa meccanismi di sincronizzazione per
 * evitare race conditions. Tali operazioni sono a carico del chiamante.
 */
void wait_wakeup_object(struct wait *wait, void *data)
{
	wait_obj_t *this, *prev;	
		
	/* Scorri la lista. Ritorna immediatamente se è vuota */
	for(this = wait->objects.next, prev = &wait->objects;
		this; prev = this, this = this->next) {
			
		/*
		 * Verifica se l'attributo dell'oggetto e il parametro passato
		 * dal chiamante sono uguali. In caso affermativo, provvvedi a 
		 * invocare l'handler dell'oggetto in questione.
		 */
		if(!memcmp(this->data, data, sizeof(this->data))) {
			/* Esegui l'handler associato alla wait list */
			wait->handler(this->data);
			/* Rimuovi l'oggetto dalla lista */
			prev->next = this->next;
			/* Rilascia la memoria occupata dall'oggetto */
			kfree(this);
			/* Decrementa il numero di elementi in lista */
			atomic_dec(&wait->count);
		}
		
	}
	
}

/*
 * Questa funzione sveglia il primo oggetto presenti nella wait list.
 * Nota bene: la funzione non implementa meccanismi di sincronizzazione per
 * evitare race conditions. Tali operazioni sono a carico del chiamante.
 */
void wait_wakeup_next(struct wait *wait)
{
	wait_obj_t *this;
		
	/* Se la lista è invalida o vuota, non eseguire alcuna operazione */
	if((this = wait->objects.next)) {
		/* Esegui l'handler associato alla wait list */
		wait->handler(this->data);		
		/* Rimuovi l'oggetto dalla lista */
		wait->objects.next = this->next;
		/* Rilascia la memoria occupata dall'oggetto */
		kfree(this);
		/* Decrementa il numero di elementi in lista */
		atomic_dec(&wait->count);
	}

}

/*
 * Questa funzione sveglia tutti gli oggetti presenti nella wait list.
 * Nota bene: la funzione non implementa meccanismi di sincronizzazione per
 * evitare race conditions. Tali operazioni sono a carico del chiamante.
 */
void wait_wakeup_all(struct wait *wait)
{
	wait_obj_t *this;
	
	/* Scorri la lista fino alla fine */
	while((this = wait->objects.next)) {
		/* Esegui l'handler associato alla wait list */
		wait->handler(this->data);
		/* Rimuovi l'oggetto dalla lista */
		wait->objects.next = this->next;
		/* Rilascia la memoria occupata dall'oggetto */
		kfree(this);
		/* Decrementa il numero di elementi in lista */
		atomic_dec(&wait->count);
	}
	
}
