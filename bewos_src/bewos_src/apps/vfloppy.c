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
* Coders: WMI
* Descrizione: Funzioni per gestire un floppy virtuale (Ram Disk)
***********************************************************************************************/

#include<string.h>
#include<task.h>
#include<system.h> // Per save_flags e restore_flags
#define VFLOPPY_NUMSECT 2880 // Numero di settori
#define VFLOPPY_DIMSECT 512  // Dimensione in byte di un settore
#define isREADONLY  	0    // 0=READ/WRITE,1=READ ONLY

static void *memory; // Puntatore alla memoria del floppy virtuale
#define VFLOPPY_ATTIVO 1 // Flag_Attivo (se questa flag non e' settata non si puo' fare nessuna operazione di lettura/scrittura)
#define VFLOPPY_BUSY 2 // Flag_Occupato (se questa flag e' settata non si puo' fare nessuna operazione di lettura/scrittura)
#define VFLOPPY_READONLY 4 // Se e' settata il contenuto del floppy non puo' essere modificato
char vfloppy_flags;
// ATTENZIONE!!! Per leggere e scrivere i dischi usare le syscalls vfloppy_raw_read,vfloppy_raw_write

void vfloppy_init() { // Funzione di inizializzazione del vfloppy (viene richiamata dal main.c)
memory=(void *)kmalloc(VFLOPPY_NUMSECT*VFLOPPY_DIMSECT); // Alloco la memoria destinata al contenuto del floppy virtuale

vfloppy_flags=VFLOPPY_ATTIVO | (isREADONLY*VFLOPPY_READONLY); // Inizializzo le flags a: ATTIVO e se isREADONLY==1 anche a sola lettura
}

/* Codici di errore di vfloppy_raw_read:
0: Tutto ok
1:Occupato
2:Non attivo
3:Numero di settore non valido
*/

int vfloppy_rd(int blocknum,int buffer) { // Usare la funzione vfloppy_raw_read (int blocknum,void *buffer) (e' una syscall)
long sys_eflags; // In questa variabile salvero' gli eflags

// Se il numero di settore non e' valido torno il codice di errore
if (blocknum>=VFLOPPY_NUMSECT || blocknum<0) // Il numero di settore non puo' essere uguale a VFLOPPY_NUMSECT perche' in un disco di 3 settori, i settori sono 0,1,2 e non deve essere <0
	return 3;

// Se il flag attivo non e' settato torno il codice di errore
if (!(vfloppy_flags & VFLOPPY_ATTIVO))
	return 2;

// Se il flag occupato e' settato torno il codice di errore
if ((vfloppy_flags & VFLOPPY_BUSY))
	return 1;

// Le condizioni permettono l'esecuzione dell'operazione di lettura

// Per imposatare le vfloppy_flags devo disabilitare gli interrupts quindi: salvo gli eflags nell variabile sys_eflags e disbilito gli interrupts
save_flags(sys_eflags);
asm("cli");
vfloppy_flags=VFLOPPY_BUSY|VFLOPPY_ATTIVO|(isREADONLY*VFLOPPY_READONLY); // Imposto le flags di vfloppy
// Una volta completata l'operazione riattivo gli interrupts:
restore_flags(sys_eflags);  //Ripristino gli Eflags
// asm("cli"); <-- GLI INTERRUPTS VENGONO RIPRISTINATI DALLA MACRO PRECEDENTE "restore_flags"


memcpy((void *)buffer,(void *)(memory+(blocknum*VFLOPPY_DIMSECT)),VFLOPPY_DIMSECT); // Copio il contenuto del settore richiesto nel contenuto del buffer passato come argomento


// Per imposatare le vfloppy_flags devo disabilitare gli interrupts quindi: salvo gli eflags nell variabile sys_eflags e disbilito gli interrupts
save_flags(sys_eflags);
asm("cli");
vfloppy_flags=VFLOPPY_ATTIVO|(isREADONLY*VFLOPPY_READONLY); // Imposto le flags di vfloppy
// Una volta completata l'operazione riattivo gli interrupts:
restore_flags(sys_eflags);  //Ripristino gli Eflags
// asm("cli"); <-- GLI INTERRUPTS VENGONO RIPRISTINATI DALLA MACRO PRECEDENTE "restore_flags"

return 0;
}

/* Codici di errore di vfloppy_raw_write:
0: Tutto ok
1:Occupato
2:Non attivo
3:Numero di settore non valido
4:Read-only
*/

int vfloppy_wt(int blocknum,int buffer) { // Usare la funzione vfloppy_raw_write (int blocknum,void *buffer) (e' una syscall)
long sys_eflags; // In questa variabile salvero' gli eflags

// Se il floppy e' di sola lettura torno il codice di errore
if ((vfloppy_flags & VFLOPPY_READONLY))
	return 4;

// Se il numero di settore non e' valido torno il codice di errore
if (blocknum>=VFLOPPY_NUMSECT || blocknum<0) // Il numero di settore non puo' essere uguale a VFLOPPY_NUMSECT perche' in un disco di 3 settori, i settori sono 0,1,2 e non deve essere <0
	return 3;

// Se il flag attivo non e' settato torno il codice di errore
if (!(vfloppy_flags & VFLOPPY_ATTIVO))
	return 2;

// Se il flag occupato e' settato torno il codice di errore
if ((vfloppy_flags & VFLOPPY_BUSY))
	return 1;

// Le condizioni permettono l'esecuzione dell'operazione di scrittura

// Per imposatare le vfloppy_flags devo disabilitare gli interrupts quindi: salvo gli eflags nell variabile sys_eflags e disbilito gli interrupts
save_flags(sys_eflags);
asm("cli");
vfloppy_flags=VFLOPPY_BUSY|VFLOPPY_ATTIVO|(isREADONLY*VFLOPPY_READONLY); // Imposto le flags di vfloppy
// Una volta completata l'operazione riattivo gli interrupts:
restore_flags(sys_eflags);  //Ripristino gli Eflags
// asm("cli"); <-- GLI INTERRUPTS VENGONO RIPRISTINATI DALLA MACRO PRECEDENTE "restore_flags"


memcpy((void *)(memory+(blocknum*VFLOPPY_DIMSECT)),(void *)buffer,VFLOPPY_DIMSECT); // Copio il contenuto del buffer passato come argomento nel contenuto del settore richiesto


// Per imposatare le vfloppy_flags devo disabilitare gli interrupts quindi: salvo gli eflags nell variabile sys_eflags e disbilito gli interrupts
save_flags(sys_eflags);
asm("cli");
vfloppy_flags=VFLOPPY_ATTIVO|(isREADONLY*VFLOPPY_READONLY); // Imposto le flags di vfloppy
// Una volta completata l'operazione riattivo gli interrupts:
restore_flags(sys_eflags);  //Ripristino gli Eflags
// asm("cli"); <-- GLI INTERRUPTS VENGONO RIPRISTINATI DALLA MACRO PRECEDENTE "restore_flags"

return 0;
}
