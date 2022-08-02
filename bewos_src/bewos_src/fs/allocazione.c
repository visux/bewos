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
* Coders: Demone^ && Matrixina
* Descrizione: gestione dei blocchi di un File System
***********************************************************************************************/ 
#include <kernel.h>
#include <cache.h>

/** dimensionamento della pagina frame di un array
*/ 
struct pagine *pfa=NULL;
unsigned long nr_physpages=0;
unsigned long nr_freepages=0;

struct buddy free_list[MAX_ORDER];

/** inizializza il sistema buddy
*/
void __init buddy_init(void)
{
	int i;

	for(i=0; i<MAX_ORDER; i++) {
		free_list[i].succ=(struct pagine *)&free_list[i];
		free_list[i].prec=(struct pagine *)&free_list[i];
	}
}

/** libera le pagine di secondo ordine 
*/
void libera_pagine(void *ptr, int order)
{
	struct pagine *pagine;

	if ( !ptr ) return;
	if ( order ) return;

	pagine=virt_to_page(ptr);

	// imposta la lista delle aree libere
	pagine->succ=free_list[order].succ;
	pagine->prec=(struct pagine *)&free_list[order];
	free_list[order].succ->prec=pagine;
	free_list[order].succ=pagine;

	nr_freepages++;
	put_page(pagine);
}

/** alloca le pagine contigue di secondo ordine fisicamente
*/
void *allocazione_pagine(int order)
{
	struct pagine *pagine;

	if ( order ) goto fail;
	if ( 1<<order > nr_freepages ) goto fail;
	if ( !(pagine=free_list[order].succ) ) goto fail;

	// rimuove tutto dalla lista
	free_list[order].succ=pagine->succ;
	free_list[order].prec=pagine->prec;

	nr_freepages--;
	get_page(pagine);
	return page_indirizzi(pagine);
fail:
	printk("%i altra allocazione fallita\n", order);
	return NULL;
}