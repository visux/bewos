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
* Descrizione: Impostazione del File System di tipo ext 2
***********************************************************************************************/
#include <ext2_file.h>
//#include <malloc.h>
#include <kernel.h>
#include <types.h>
#include <vfs.h>
#include <blk.h>
#include <stat.h>

// si possono modificare in base a quanto si gestisce
#define EXT2_MIN_BLOCK_SIZE 512
#define EXT2_SUPER_OK 4096
#define EXT2_ROOT_INO 4096
#define EXT2_N_BLOCKS 10
#define EXT2_NDIR_BLOCKS 10

/** prototipi delle funzioni dichiarate nella ext2.c
*/
void init_ext2(void);
int ext2_get_super(struct super *);
int ext2_leggi_inode(struct inode *);
struct inode *ext2_lookup(struct inode *, char *);

/** descrittore del file system - EXT2
*/
struct vfs_tipofs ext2_tipofs={
	.nome="ext2",
	.leggi_super=ext2_get_super,
};

/** operazioni al super blocco
*/
struct super_ops ext2_superops={
	.leggi_inode=ext2_leggi_inode,
};


/** serve per la copia dell'inode 
*/
struct inode_ops ext2_dir_iop={
	.lookup=ext2_lookup,
};

/** gestisce il lookup del nome in un inode
*/
struct inode *ext2_lookup(struct inode *inode, char *n)
{
	
	struct buffer *bh;
	ino_t inum;
	int x,y;
	int nlen=strlen(n);
	
	struct ext2_dir_logical_2 *dir;
	char *j;

	
	y=EXT2_NDIR_BLOCKS < inode->i_blocks ? EXT2_NDIR_BLOCKS : inode->i_blocks;

	for(x=0; x<y; x++) {
		
		//si cerca di preallocare il blocco
		if ( inode->u.ext2.blocco[x]==0 ) continue;

		// lettura del blocco
		bh=leggi_blk(inode->i_sb->s_dev, inode->u.ext2.blocco[x]);
                
		// ricerca l'istanza
		for(j=bh->b_buf; j<(bh->b_buf+bh->b_len); ) {
			//dir=(struct ext2_dir_logical_2 *)j;
			/*
			if ( (dir->name_len == nlen) &&
				!memcmp(dir->name, n, nlen) ) {
				inum=dir->inode;
				libera_memoria(bh);
				//return iget(inode->i_sb, inum);
			}*/
			//j+=dir->rec_len;
		}

		libera_blocco(bh);
	}

	return NULL;
}


/** inizializzazione del tipo di file system ext2
*/
void init_ext2(void)
{
	vfs_tipofs_aggiungi(&ext2_tipofs);
}
driver_init(init_ext2);

/** funzione in grado ricavarsi in superblocco della ext2
*/
int ext2_get_super(struct super *sb)
{
	
	struct ext2_super_blocco *s;
	struct buffer *bh;
	int conteggio, i;

	// setta la dimensione dei blocchi dei dispositivi
	if (set_dimblocchi(sb->s_dev, EXT2_MIN_BLOCK_SIZE)) {
		printk("EXT2: Non è in grado di settare la dimensione del blocco %d\n", EXT2_MIN_BLOCK_SIZE);
		return -1;
	}
	
	// legge nel super blocco
	if ( !(bh=leggi_blk(sb->s_dev,1)) ) {
		printk("EXT2: %s: Non è in grado di leggere il super blocco\n",
			sb->s_dev->nome);
		return -1;
	}
        
	 
	s=(struct ext2_super_blocco *) bh->b_buf;
        
	// Verifica del superblocco altrimenti da errore 
	if ( s->s_super_ok!=EXT2_SUPER_OK ) {
		printk("EXT2: Errore nel superblocco BAD block !\n");
		goto errore;
	}
	
	if ( s->s_log_block_size ) {
		printk("EXT2: solo la dimensione di 1KB è supportata \n");
		goto errore;
	}
	
	/* adesso dopo aver verificato se esistono errori può gestire la 
	struttura del super blocco VFS.h*/
	sb->s_blocksize=EXT2_MIN_BLOCK_SIZE;
	sb->s_ops=&ext2_superops;

	// ricava la struttura totale e quindi riempie 
	sb->u.ext2.s_sbh=bh;
	sb->u.ext2.s_es=s;
	sb->u.ext2.s_inodes_per_blocco=sb->s_blocksize / s->s_inode_size;
	
	sb->u.ext2.s_blocks_per_gruppo=s->s_blocks_per_gruppo;
	
	sb->u.ext2.s_inodes_per_gruppo=s->s_inodes_per_gruppo;
	sb->u.ext2.s_itabella_per_gruppo=s->s_inodes_per_gruppo / 
		sb->u.ext2.s_inodes_per_blocco;
	
	sb->u.ext2.s_desc_per_blocco=sb->s_blocksize / 
		sizeof(struct ext2_super_blocco);
	
	sb->u.ext2.s_gruppo_count=(s->s_blocchi_count - 
		s->s_primo_blocco_dati + 
		(sb->u.ext2.s_blocks_per_gruppo-1)) /
		sb->u.ext2.s_blocks_per_gruppo;
        
	// Legge nel descrittore del gruppo 
	conteggio = (sb->u.ext2.s_gruppo_count + sb->u.ext2.s_desc_per_blocco - 1) /
		sb->u.ext2.s_desc_per_blocco;
	
	sb->u.ext2.s_descriz_gruppo = kmalloc_cache(conteggio * sizeof(struct buffer *));
	//sb->u.ext2.s_descriz_gruppo = kmalloc(conteggio * sizeof(struct buffer *));
	for(i=0; i<conteggio; i++) {
		
		int j;
		sb->u.ext2.s_descriz_gruppo[i]=leggi_blk(sb->s_dev, i+2);
		if ( !sb->u.ext2.s_descriz_gruppo[i] ) {
			for(j=0; j<i; j++)
				libera_blocco(sb->u.ext2.s_descriz_gruppo[i]);
			printk("EXT2: errore di lettura del gruppo nel descrittore\n");
			goto errore;
		}
	}

	// disattivazione inode per il root 
	sb->s_root=daiInode(sb, EXT2_ROOT_INO);

	if ( !sb->s_root ) {
		printk("EXT2: Fallito il tentativo di inode da root\n");
		goto errore;
	}
	
	// da fare
	return 0;

    errore:
        //libera  memoria del blocco
	libera_blocco(bh);
        return -1;

}

/** funzione in grado di scorrere nell'inode per ricavarsi tramite get(), l'inode interessato
*/
int ext2_leggi_inode(struct inode *inode)
{
	
	unsigned long blocco_gruppo;
	unsigned long descriz_gruppo, descriz;
	unsigned long displacement, blocco;
	struct ext2_descriz_gruppo *gdesc;
	struct ext2_inode *raw;
	struct buffer *b;
        
	
	// 1. cerca il numero di i-node 
	if ( inode->i_ino > inode->i_sb->u.ext2.s_es->s_inodes_count ) {
		printk("EXT2: errore nell'inode %u\n", inode->i_ino);
		return -1;
	}

	
	// 2. calcola in numero di blocco del gruppo
	blocco_gruppo = (inode->i_ino) / inode->i_sb->u.ext2.s_inodes_per_gruppo;
	if ( blocco_gruppo >= inode->i_sb->u.ext2.s_gruppo_count ) {
		printk("EXT2:il tentativo di impostare il gruppo è fallito %lu\n", blocco_gruppo);
		return -1;
	}

	// 3. ottiene il descrittore del gruppo del blocco in considerazione
	
	descriz_gruppo = blocco_gruppo / inode->i_sb->u.ext2.s_desc_per_blocco;
	descriz = blocco_gruppo % inode->i_sb->u.ext2.s_desc_per_blocco;
	b=inode->i_sb->u.ext2.s_descriz_gruppo[descriz_gruppo];
	gdesc = (struct ext2_super_blocco *)b->b_buf;
	
	// 4. ottiene il corretto blocco dalla tabella degli inode
	displacement = inode->i_ino % inode->i_sb->u.ext2.s_inodes_per_gruppo;
	//blocco = gdesc[descriz].bg_inode_table + (displacement / inode->i_sb->s_blocksize);
	
	if ( !(b=leggi_blk(inode->i_sb->s_dev, blocco)) ) {
		printk("EXT2: Non può leggere il blocco dell'inode inode=%lu, blocco=%lu\n",
			inode->i_ino, blocco);
		return -1;
	}

	
	// 5. ottiene il puntatore all'inode
	displacement &= inode->i_sb->s_blocksize-1;
	displacement *= inode->i_sb->u.ext2.s_es->s_inode_size;
	raw           = (struct ext2_inode *)(b->b_buf + displacement);
	
	// 6. copia l'inode
	
	inode->i_fop=NULL;
	/*
	if ( S_ISDIR(raw->i_mode) ) {
		//inode->i_iop=&ext2_dir_iop;
	}else{
		inode->i_iop=NULL;
	}
*/
	/*
	inode->i_mode=raw->i_mode;
	inode->i_uid=raw->i_uid;
	inode->i_gid=raw->i_gid;
	inode->i_size=raw->i_size;
	inode->i_nlink=raw->i_links_count;
	inode->i_blocks=raw->i_blocks;
	*/
/*
	for(blocco=0; blocco<EXT2_N_BLOCKS; blocco++) {
		inode->u.ext2.blocco[blocco] = raw->i_block[blocco];
	}
*/
	//termina liberando poi il blocco
	libera_blocco(b);
	return 0;
}
