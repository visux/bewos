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
* Descrizione: Dichiarazione del Virtual File System
***********************************************************************************************/

#ifndef __VFS_H__
#define __VFS_H__

#include <ext2_file.h>

/** Oggetto dell'inode 
*/
struct inode {
	// ricavato da: iget() 
	ino_t			i_ino;
	struct super		*i_sb;
	u_int32_t		i_count;

	// ricavato da:  super->leggi_inode 
	struct inode_ops	*i_iop;
	struct file_ops		*i_fop;
	umode_t			i_mode;
	uid_t			i_uid;
	gid_t			i_gid;
	loff_t			i_size;
	nlink_t			i_nlink;

	unsigned long		i_blocks;
        // serve per gestire liste concatenate
	union {
		struct ext2_in_info	ext2;
		char			pad[32];
	}u;
};

struct inode_ops {
	struct inode *(*lookup)(struct inode *, char *nome);
};

/** Directory entry - logical
*/
struct logical {
	struct inode		*d_ino;
	u_int32_t		d_count;
	const unsigned char	*d_name;
	size_t			d_len;
	u_int32_t		d_hash;
};


/** Superblocco
*/
struct super {
	struct super *prec,*succ;

	 //Non servono per la creazione del touch del File system 
	struct blkdev *s_dev;
	struct vfs_tipofs *s_tipo;

	// Si utilizza il get_super
	int s_blocksize;
	struct super_ops *s_ops;
	struct inode *s_root;

	union {
		struct ext2_info	ext2;
		char			pad[32];
	}u;
};

/** FS Tipo di file System
*/
struct vfs_tipofs {
	struct vfs_tipofs *succ;
	char *nome;
	int (*leggi_super)(struct super *s);
};


/** operazioni al superblocco
*/
struct super_ops {
	int (*leggi_inode)(struct inode *);
};

/** prototipi per la gestione del VFS
*/
void init_vfs(void);
void mount_vfs_root(void);

/** registrazione oggetto 
*/
void vfs_tipofs_aggiungi(struct vfs_tipofs *);

/** Interfaccia per l'i-node
*/
struct inode *daiInode(struct super *, ino_t);
void scriviInode(struct inode *);


#endif 

/**__VFS_H__ 
*/
