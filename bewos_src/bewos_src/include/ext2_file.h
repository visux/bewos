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
* Coders: Demone^ & Matrixina
* Descrizione: Dichiarazione delle informazioni dell'ext2
***********************************************************************************************/

#ifndef __EXT2_FILE_H__
#define __EXT2_FILE_H__

#include<types.h>


/** struttura ext2 superblocco di appoggio
*/
struct ext2_super_blocco {
	// dimensione dell'i-node
	  unsigned long s_inode_size;
	// Numero di blocchi in un gruppo
	  unsigned long s_blocks_per_gruppo;
	// Numero di inode in un gruppo 
	  unsigned long s_inodes_per_gruppo;
	// Numero di blocchi
	  unsigned long s_blocchi_count;	
	// Numero del primo blocco del dato
	  unsigned long s_primo_blocco_dati;	
	// log dimensione
	  unsigned long s_log_block_size;	
	// verifica del blocco   
	  unsigned long s_super_ok;	
	// conteggio degli i-node  
	  unsigned long s_inodes_count;	
	  
	  
};

/** struttura dell'ext 2
*/
struct ext2_info {
	// Numero degli inode per un blocco
	  unsigned long s_inodes_per_blocco;
	// Numero di blocchi in un gruppo
	  unsigned long s_blocks_per_gruppo;
	// Numero di inode in un gruppo 
	  unsigned long s_inodes_per_gruppo;
	// Numero degli inode nella tebella dei blocchi per un gruppo   
	  unsigned long s_itabella_per_gruppo;	
	// Numero di gruppi nel descrittore del blocco 
	  unsigned long s_count;	
	// Numero descrittori per gruppo nel blocco 
	  unsigned long s_desc_per_blocco;	
	// Numero dei gruppi nel file system   
	  unsigned long s_gruppo_count;	
	// struttura del buffer del blocco 
	  struct buffer *s_sbh;	
	// Struttura del blocco stesso
	  struct ext2_super_blocco *s_es;	
	// descrizione del gruppo nel buffer
	struct buffer **s_descriz_gruppo;
};

/** max 15 blocchi per l'int 32
*/
struct ext2_in_info {
	u_int32_t blocco[15];
};

#endif 
/** __EXT2_FILE_H__ 
*/
