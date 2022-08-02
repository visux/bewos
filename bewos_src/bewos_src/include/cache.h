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
* Descrizione: gestione della cache memory
***********************************************************************************************/
#ifndef __CACHE_H
#define __CACHE_H


#define PAGE_DISPLACEMENT	0xc0000000
#define PAGE_SHIFT	        12
#define PAGE_DIM	        (1UL<<PAGE_SHIFT)

//#define PG_riservata	0x01
#define PG_slab		0x02

/** conteggio delle pagine referenziate 
*/
#define get_page(p) ((p)->count++)
#define put_page(p) ((p)->count--)


#define __pa(x)		((unsigned long)(x)-PAGE_DISPLACEMENT)
#define __va(x)		((void *)((unsigned long)(x)+PAGE_DISPLACEMENT))

//#define allocazione_pagine() allocazione_pagine(0)

/** gestione della kmalloc per la cache
*/
#define page_cache(p) ((struct m_cache *)((p)->prec))
#define page_slab(p) ((struct m_slab *)((p)->succ))

/** definisce la struttura delle pagine 
*/
#define page_indirizzi(page) __va( ((page)-pfa) << PAGE_SHIFT )
#define virt_to_page(kaddr) (pfa + (__pa(kaddr) >> PAGE_SHIFT))

/** sistema buddy
*/
#define MAX_ORDER 10
extern struct pagine *pfa;
extern unsigned long nr_physpages;
extern unsigned long nr_freepages;

/** struttura principale della cache
 */
struct m_cache {
	struct m_cache *succ;
	char *nome; // nome cache  
	size_t dim; // dimensione dell'oggetto 
	size_t num_obj; // numero di oggetti per slab 
	int order; // altro: in caso di pagine libere
	struct m_slab *slab; // cache m_slab interna
}; 

/** struttura per allocazione di memoria tramite l'utilizzo di una cache
 */
struct m_slab {
	struct m_slab *succ; // elemento successivo
	void **obj; // prossimo oggetto libero 
};

/** struttura delle pagine in memoria secondaria
*/
struct pagine {
	struct pagine *prec,*succ;
	u_int32_t count;
	u_int32_t flags;
};

/** i membri dello struc devono essere come lo struc delle pagine
*/
struct buddy {
	struct pagine *prec, *succ;
};


/** prototipi delle funzioni che riguardano l'utilizzo del cache memory
*/
/** prototipo della funzione che gestiscce l'aggiunta della mem-cache
*/ 
int kmem_cache_aggiungi(struct m_cache *c); 
void *kmalloc_cache(size_t sz);
void __init kmalloc_init(void);
void *alloca(struct m_cache *c);
void libera_locazione(void *ptr);
void buddy_init(void);

//void *allocazione_pagine(int);
//void libera_pagine(void *, int);

#endif 
/** __CACHE_H
*/