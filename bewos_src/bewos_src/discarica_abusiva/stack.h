#define USER_STACK_SIZE		8		//Dimensione in KB
#define KERNEL_STACK_SIZE		128		//Dimensione in KB




struct stack_struct
{
unsigned int user_stack_pages;
unsigned int kernel_stack_pages;
unsigned int total_stack_pages;

void* start_stack_virtual_addr;
void* end_stack_virtual_addr;

unsigned int low_user_stack_page;
unsigned int low_kernel_stack_page;
void* low_stack_user_addr;
void* low_stack_kernel_addr;

unsigned int blank_page_1;
unsigned int blank_page_2;
unsigned int blank_page_3;
void* blank_addr;

};

extern struct stack_struct stack_info;
