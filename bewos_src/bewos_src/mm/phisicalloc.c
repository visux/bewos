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

/******************************************INFO******************************************************
* Coders: Black
* Descrizione: Definizioni di funzioni per l'  allocazione\deallocazione della memoria fisica
*******************************************************************************************************/



#include<phisicalloc.h>
#include<phisicmem.h>

/********************************************************************************
********************************************************************************/
 //La funzione phisic_alloc alloca un area di memoria di dimenzione num_bytes e restituisce un puntatore di tipo void* il quale punta all'
 //inzio dell' area allocata
void* phisic_alloc(unsigned int num_bytes)
{
 void* addr;
 unsigned int num_pag;
 
 //traduciamo il numero di bytes in numero di pagine
 if(num_bytes % SIZE_PAGE == 0)
   num_pag = num_bytes / SIZE_PAGE;
else
   num_pag = num_bytes / SIZE_PAGE + 1;

addr = alloc_pages(num_pag);

return addr;
}
/********************************************************************************
********************************************************************************/
//La funzione phisic_free dealloca la memoria allocata con la funzione phisic_alloc
void phisic_free(void* addr)
{
  dealloc_pages(addr);
 }
