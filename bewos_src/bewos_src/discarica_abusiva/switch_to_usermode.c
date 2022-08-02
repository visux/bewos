#include<gdt.h>

void switch_to_usermode(unsigned int esp, unsigned int eip)
{
unsigned int flag = 0x00;
	
		asm(	"pushl %0\n"
			"pushl %1\n"
			"pushl %2\n"
			"pushl %3\n"
			"pushl %4\n"
			"iret\n"
			"popl %ax"
		 	"movw %%ax, %%ds\n"
			"movw %%ax,%%ss\n"
			"movw %%ax,%%es\n"
			"movw %%ax,%%fs\n"
			"movw %%ax,%%gs\n"
		 ::"m"(USER_DATA_SELECTOR), "m"(esp), "m"(flag), "m"(USER_CODE_SELECTOR), "m"(eip));

}
