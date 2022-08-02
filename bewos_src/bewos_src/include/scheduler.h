#ifndef __SCHEDULER_H
#define __SCHEDULER_H

void init_scheduler();
void scheduler(unsigned int* cur_stack);
void round_robin(void);
void enable_scheduler(void);
void disable_scheduler(void);
int get_scheduler(void);

extern char scheduler_active;

#endif
