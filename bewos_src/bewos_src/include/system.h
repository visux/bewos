 
#ifndef SYSTEM_H
#define SYSTEM_H

//Salva gli EFLAGS sullo stack attraverso l' istruzione pushfl e li mette nella variabile x
#define save_flags(x)		__asm__ __volatile__("pushfl ; popl %0":"=g" (x): /* no input */)

//Salva gli EFLAGS sullo stack e disabilita gli interrupt
#define save_flags_irq(x)       __asm__ __volatile__("pushfl ; popl %0 ; cli":"=g" (x): :"memory")

//Mette sullo stack il valore della variabile x e setta gli EFLAGS con tale valore attraverso l' istruzione popfl
#define restore_flags(x) 	__asm__ __volatile__("pushl %0 ; popfl": :"g" (x):"memory", "cc")

//Mette il valore del registro CS nella variabile x    
#define read_cs_register(x) __asm__ __volatile__("push %%cs; pop %0":"=g"(x): /* no input */)

int read_cur_privilege();

#endif
