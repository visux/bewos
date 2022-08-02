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
* Descrizione: Definizioni di funzioni per l' allocazione\deallocazione della memoria
*******************************************************************************************************/



#include<paging.h>
#include<phisicalloc.h>

/*************************************************************************************************************************************
 Quando si e' in paging mode la malloc funziona in questo modo: 

1) Si allocano fisicamente num_bytes richiesti con l' allocatore fisico
2) L' indirizzo fisico restituito dall' allocatore fisico deve essere inserito in una PageTable
3) Ritornare l' indirizzo virtuale che altro non e' che le coordinate(pde/pte/off) che individuano l' indirizzo fisico inserito nella PageTable al punto 2

 ***************************************************************************************************************************************/

 
void* malloc(unsigned int num_bytes)
{
 void* phisic_addr;
 void* virtual_addr;
 void* addr_new_pte;
 int pde, pte;
 unsigned long* pd_addr = 0;

 phisic_addr = (void*)phisic_alloc(num_bytes); //alloco fisicamente num_bytes


 virtual_addr = find_first_pte_free();


  if(virtual_addr == (void*)-1) /* Se non ho trovato nessuna entry libera nella PageTable ...  */
   {

 
    pde = find_first_pde_free(); //mi trovo una entry libera nella pagedir

 	if(pde == -1) //Se non ho trovato entry libere nella pgdir ...
  	   return NULL; //Ritorno NULL
	   
/* altrimenti alloco una nuova PageTable e modifico la PageDir in modo da mettere l' indirizzo della nuova PT allocata nell' entry della PD trovata libera */	

addr_new_pte = (void*)phisic_alloc(4096);
edit_pde(pde, addr_new_pte , USER_PAGES_FLAG);


    pte = 0; //poiche' ho allocato una nuova PageTable la sua prima entry libera sara' di indice 0

    virtual_addr = make_virtual_address(pde, pte, 0); //creo l' indirizzo virtuale
   }

 mappage((unsigned int)virtual_addr, phisic_addr,USER_PAGES_FLAG); /* chiamo la funzione che inserisce l' indirizzo fisico resituito
  dall' allocatore fisico nelle coordinate PDE / PTE espresse dall' indirizzo virtuale creato(quindi del tipo pde/pte/off) dato in ingresso */


return virtual_addr; //Ritorno l' indirizzo virtuale



}


//La funzione che segue dealloca la memoria occupata con la malloc()
void free(void* addr)
{
free_pages(addr);
}


