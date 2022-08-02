#ifndef __IRQ_H
#define __IRQ_H

#define NUM_IRQ 16

void gestisci_irq(unsigned int esp);
void insert_irq(void (*func)(), int pos);

void (*handler_vector_function[NUM_IRQ])(void);

#endif
