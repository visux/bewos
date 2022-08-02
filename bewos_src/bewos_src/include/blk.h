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
* Descrizione: Dichiarazione variabili blocchi dell'ext2
***********************************************************************************************/
#ifndef __BLK_H
#define __BLK_H

// struttura del buffer utilizzata nella blk.c
struct buffer {
	
	struct blkdev 	*b_dev;
	block_t 	b_block;
	char 		*b_buf;
	size_t		b_len;
	u_int32_t 	b_flags;
	u_int32_t	b_count;
};
// costanti utilizzate poi per l'update delle entry e del buffer
#define BH_uptodate	0x01
#define BH_dirty	0x02

#define ricava_buffer(buf)	((buf)->b_count++)
#define scrivi_buffer(buf)	((buf)->b_count--)

/** struttura per i dispositivi a blocchi
*/
struct blkdev {
	// usata solo dal sub system(Dal sistema generale)
	struct blkdev *succ;

	// codifica driver 
	char *nome;
	int setdim;
	int (*ll_rw_blk)(int, block_t, char *, size_t);

	size_t count;
};

/** procedure e funzioni per i Blocchi e il buffer della cache 
*/
void init_blocchi(void);
int set_dimblocchi(struct blkdev *, int);

/** struttura che servirà per la lettura del blocco e passando come parametri il dispositivo e un numero intero del dispositivo
*/
struct buffer *leggi_blk(struct blkdev *, int);

#endif 
/** __BLK_H
*/
