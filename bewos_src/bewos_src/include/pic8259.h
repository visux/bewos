#ifndef __PIC8259_H
#define __PIC8259_H

void Init_PIC8259(void);
void abilita_irq(unsigned char irq_num, unsigned char port_num);
void disabilita_irq(unsigned char irq_num, unsigned char port_num);
#endif
