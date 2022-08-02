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
* Descrizione: Gestione delle directory d'entrata -logical entry
***********************************************************************************************/
#include <kernel.h>
#include <cache.h>
#include <vfs.h>

/** struttura che gestisce la cache alle directory-entry
*/
struct m_cache logical_cache={
	.nome="logical",
	.dim=sizeof(struct logical)
};
 
/** risolve un nome ad un inode
*/
struct inode *nome_inode(char *nome)
{
	struct inode *i;
	
	char *n;
	int stato,fine=0;

	if ( !nome ) return NULL;

	if ( *nome=='/' ) {
		nome++;
		//i=__this_task->root;
	}else{
		//i=__this_task->cwd;
	}
        
	/** verifica se esistono degli errori 
	*/
	if ( !i ) {
		printk("l'operazione verrà bloccata\n");
		return NULL;
	}
	if ( !i->i_iop ) {
		printk("non esiste l'inode\n");
		return NULL;
	}
	if ( !i->i_iop->lookup ) {
		printk("non esiste l'operazione di lookup all'inode\n");
		return NULL;
	}

	/** itera i componenti del nome del file e setta il lookup man mano va avanti
	 */
	for(n=nome,stato=1; ;n++) {
		if ( stato==0 ) {
			struct inode *itmp;
			if ( *n==0 || *n=='/' ) {
				if ( *n==0 ) fine=1;
				*n=0;
				stato=1;
				itmp=i;
				
				if ( !(itmp=i->i_iop->lookup(itmp,nome)) ) {
					return NULL;
				}
				if ( fine ) break;
				scriviInode(i);
			}
		}else if ( stato==1 ) {
			if ( *n==0 ) break;
			if ( *n!='/' ) {
				stato=0;
				nome=n;
			}
		}
	}

	return i;
}