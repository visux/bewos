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

#include <types.h>
#include <phisicmem.h>
#include <kernel.h>
#include <cache.h>
#include <blk.h> 

/** cache del blocco (buffer) 
*/
struct m_cache blocco_cache={
	.nome="buffer",
	.dim=sizeof(struct buffer)
};

/** avvia l'inizializzazione dei blocchi
*/
void init_blocchi(void)
{
	kmem_cache_aggiungi(&blocco_cache);
}

/** setta i blocchi di un dispositivo (NOTE: La dimensione dei blocchi non è uguale a quella dei settori) 
*/
int set_dimblocchi(struct blkdev *dev, int dimensione)
{
	
	if ( dev->setdim > dimensione )
		return -1;

	// verifica la dimensione della pagina
	if ( dimensione > PAGE_DIM || dimensione < 512 || (dimensione & (dimensione-1)) )
		return -1;

	// non ha bisogno di essere cambiato 
	if ( dev->setdim*dev->count == dimensione )
		return 0;

	dev->count = dimensione/dev->setdim;
	
	return 0;
}

/** procedura in grado di liberare spazio dal blocco
*/
void libera_blocco(struct buffer *buf)
{
	if ( buf==NULL ) return;
	if ( !buf->b_count ) {
		printk("libera_blocco: Buffer libero!\n");
		return;
	}
	scrivi_buffer(buf);
}

/** legge un blocco dentro il buffer 
*/
struct buffer *leggi_blk(struct blkdev *dev, int b_logico)
{       
        // struttura dichiarata nella blk.h del buffer
	struct buffer *ret;

	
	if ( !(ret=kmalloc_cache(&blocco_cache)) )
		return NULL;

	ret->b_dev=dev;
	ret->b_block=b_logico;
	ret->b_buf=kmalloc_cache(dev->count*dev->setdim);
	//ret->b_buf=kmalloc(dev->count*dev->setdim);
	ret->b_flags=0;
	ret->b_count=1;

	if ( !ret->b_buf ) {
		libera_locazione(ret);
		//kfree(ret);
		return NULL;
	}

	// legge dal dispositivo
	if ( dev->ll_rw_blk(0,b_logico*dev->count,ret->b_buf,dev->count) ) {        libera_locazione(ret->b_buf);
	         libera_locazione(ret);
 		//kfree(ret->b_buf);
		//kfree(ret);
		return NULL;
	}

	ret->b_flags=BH_uptodate;
	ret->b_len=dev->count*dev->setdim;
	return ret;
}
