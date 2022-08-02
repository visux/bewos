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
#include <blk.h>

struct blkdev *bdispositivi=NULL;

void blkdev_aggiungi(struct blkdev *bd)
{
	if ( !bd ) return;
	bd->succ=bdispositivi;
	bdispositivi=bd;
}

struct blkdev *blkdev_dai(char *nome)
{
	struct blkdev *r;

	for(r=bdispositivi; r; r=r->succ) {
		if ( !strcmp(nome, r->nome) )
			return r;
	}

	return NULL;
}
