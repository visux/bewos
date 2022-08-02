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
* Descrizione: Virtual File System
***********************************************************************************************/
#include <kernel.h>
#include <cache.h>
#include <types.h>
#include <vfs.h>

struct vfs_tipofs *fs_types=NULL;
struct super *superblocco=NULL;

/* varie tipologie di slab caches */
extern struct m_cache inode_cache;
extern struct m_cache logical_cache;
 
/** Inizializza tutto l'aspetto del livello vfs 
*/
void init_vfs(void)
{
	kmem_cache_aggiungi(&inode_cache);
	kmem_cache_aggiungi(&logical_cache);
}

/** aggiunge il tipo di file system
*/
void vfs_tipofs_aggiungi(struct vfs_tipofs *tipof)
{
	if ( !tipof ) return;
	tipof->succ=fs_types;
	fs_types=tipof;
}

/** Monta il file System da root
*/
void mount_vfs_root(void)
{
	
	static struct super sb;
	char *tipo="ext2";
	char *dev="floppy0";
	
	
	if ( !(sb.s_tipo=get_tipofs_del_vfs(tipo)) ) {
		printk("vfs: impossibile verificare il tipo di file system %s\n", tipo);
		return;
	}

	
	if ( !(sb.s_dev=blkdev_dai(dev)) ) {
		printk("vfs: non conosce il blocco del dispositivo%s\n", dev);
		return;
	}

	sb.s_blocksize=0;
/*
	if ( sb.s_type->leggi_super(&sb) ) {
		printk("vfs: errore nel montare il file system da root\n");
		return;
	}
        */
	//superblocks=&sb;

	// Setup the tasks structures 
	//__this_task->root=superblocks->s_root;
	//__this_task->cwd=superblocks->s_root;
}

/** verifica il tipo di file system dell'oggetto identificato
*/
struct vfs_tipofs *get_tipofs_del_vfs(char *nome)
{
	struct vfs_tipofs *verifica;
	
	for(verifica=fs_types; verifica; verifica=verifica->succ) {
		if ( !strcmp(nome, verifica->nome) )
			return verifica;
	}

	return NULL;
}

