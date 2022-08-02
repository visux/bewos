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
* Descrizione: Struttura dell'i-node
***********************************************************************************************/
#include <kernel.h>
#include <cache.h>
#include <vfs.h>

/** struttura che gestisce la cache all'inode
*/
struct m_cache inode_cache={
	.nome="inode",
	.dim=sizeof(struct inode)
};

/** ritrova in un inode già montato dal descrittore del fs e il numero dell'inode 
*/
struct inode *daiInode(struct super *sb, ino_t ino)
{
	struct inode *ret;

	if ( !sb || !sb->s_ops->leggi_inode ) return NULL;

	if ( !(ret=kmalloc_cache(&inode_cache)) )
	if ( !(ret=kmalloc(&inode_cache)) )
		return NULL;

	// ha bisogno di leggere l'inode
	ret->i_ino=ino;
	ret->i_sb=sb;

	if ( sb->s_ops->leggi_inode(ret) ) {
		libera_locazione(ret);
		//kfree(ret);
		return NULL;
	}

	ret->i_count=1;
	return ret;
}

/** rilascia un inode che può essere residente in memoria
*/
void scriviInode(struct inode *inode)
{
	if ( inode==NULL ) return;

	if ( !inode->i_count ) {
		printk("scriviInode: Inode liberato\n");
		return;
	}

	if ( --inode->i_count ) return;

	libera_locazione(inode);
	//kfree(inode);
}
