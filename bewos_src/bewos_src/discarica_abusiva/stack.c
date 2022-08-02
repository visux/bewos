#include<stack.h>
#include<paging.h>
#include<string.h> //La includo per la define NULL

/* La regione dello stack deve avere nella PageTable un aspetto simile:

blank  --  high user stack pages  --  low user stack pages  --  blank  --  high kernel stack pages -- low kernel stack pages  --  blank

Per blank si intende una pagina not-present. Solo la parte "low" e' PRESENT
*/



struct stack_struct stack_info;


void n_stack_pages_reserved(void)
{
stack_info.user_stack_pages = USER_STACK_SIZE / PAGE_SIZE;
stack_info.kernel_stack_pages = KERNEL_STACK_SIZE / PAGE_SIZE;

stack_info.blank_page_1 = 0; 							//Primo Blank
stack_info.blank_page_2 = stack_info.user_stack_pages + 1;		//Secondo Blank
stack_info.blank_page_3 = stack_info.total_stack_pages;			//Terzo Blank
//blank_page_3 indica l' ultima pagina dello stack; per questo gli assegno proprio le pagine totali riservate allo stack 


stack_info.low_user_stack_page	    =   stack_info.blank_page_2 - 1;
stack_info.low_kernel_stack_page    =   stack_info.blank_page_3 - 1;

}



void total_pages_stack_reserved(void)
{
stack_info.total_stack_pages = stack_info.user_stack_pages + stack_info.kernel_stack_pages + 3; 
/*aggiungo 3 pagine Blank ovvero not-present che delimitano lo stack user e kernel*/
}


void reseve_pages_for_stack(void)
{
int i;
int n_pde;
void* virtual_addr_for_stack;


for(i = 0; i < stack_info.total_stack_pages; i++)
  {
	virtual_addr_for_stack = find_first_pte_free();

 	 if(virtual_addr_for_stack == (void*)-1) /* Se non ho trovato nessuna entry libera nella PageTable ...  */
	{
   		 n_pde = find_first_pde_free(); //mi trovo una entry libera nella pagedir

 		if(n_pde == -1) //Se non ho trovato entry libere nella pgdir ...
		 {
		 	printk("\nImpossibile continuare: memoria virtuale insufficiente");
			return; //Ritorno NULL
		}
	
	
/* Altrimenti alloco una nuova PageTable e modifico la PageDir in modo da mettere l' indirizzo della nuova PT allocata nell' entry della PD trovata libera */	

		else //Questo else non e' necessario ma rende il codice piu' chiaro
		  {
			virtual_addr_for_stack = (void*)phisic_alloc(4096);
			edit_pde(n_pde, virtual_addr_for_stack , USER_PAGES_FLAG);
		  }


	}

	if(i == 0)
		stack_info.start_stack_virtual_addr = virtual_addr_for_stack; //assegno l' indirizzo virtuale dell' inizio dello stack
	
	if(i == stack_info.total_stack_pages-1)
		stack_info.end_stack_virtual_addr = virtual_addr_for_stack; //assegno l' indirizzo virtuale della fine dello stack
		
	


if(i == stack_info.blank_page_1 || i == stack_info.blank_page_2 || i == stack_info.blank_page_3) 
/*significa che mi trovo nella situazione in cui la pagina deve essere settata come blank cioe' not-present*/
{
	mappage((unsigned int)virtual_addr_for_stack, 0, 0);
	stack_info.blank_addr = virtual_addr_for_stack;
}
	

else if(i == stack_info.low_user_stack_page)//altrimenti devo settare le pagine riservate allo stack come PRESENT
{
	mappage((unsigned int)virtual_addr_for_stack, 0, PRESENT);
	stack_info.low_stack_user_addr = virtual_addr_for_stack;
}
	

else if(i == stack_info.low_kernel_stack_page)
{
	mappage((unsigned int)virtual_addr_for_stack, 0, PRESENT);
	stack_info.low_stack_kernel_addr = virtual_addr_for_stack;
}

else
	mappage((unsigned int)virtual_addr_for_stack, 0, 0);
	

  }//Chiude il for
  
}//Chiude la funzione


void* alloc_user_stack(void)
{
void* physic_stack_addr;
physic_stack_addr = (void*)phisic_alloc(4096);

mappage((unsigned int)stack_info.low_stack_user_addr, physic_stack_addr, USER_PAGES_FLAG);

return stack_info.low_stack_user_addr + 4096; /*Ritorno lo stack, ho aggiunto la dimensione della pagina perche' lo stack va a Decrescere: LIFO*/
}


void* alloc_kernel_stack(void)
{
void* physic_stack_addr;
physic_stack_addr = (void*)phisic_alloc(4096);

mappage((unsigned int)stack_info.low_stack_kernel_addr, physic_stack_addr, KERNEL_PAGES_FLAG);

return stack_info.low_stack_kernel_addr + 4096; /*Ritorno lo stack, ho aggiunto la dimensione della pagina perche' lo stack va a
Decrescere: LIFO*/
}
