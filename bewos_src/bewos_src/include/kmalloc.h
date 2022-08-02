#ifndef __KMALLOC_H
#define __KMALLOC_H

void* kmalloc(unsigned int num_bytes);
void kfree(void* addr); 

#endif
