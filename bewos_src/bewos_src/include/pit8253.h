#ifndef __PIT8253_H
#define __PIT8253_H

#include <task.h>

#define HZ 100
#define MS_PIT (1000 / HZ)	//indica ogni quanti millisecondi il pit invia un interrupt timer
#define TIMER_FREQ 1193180L
#define TIMER_COUNT ((unsigned) (TIMER_FREQ/HZ))


#define PIT_CHANNEL_0 0x40
#define PIT_CHANNEL_1 0x41
#define PIT_CHANNEL_2 0x42
#define PIT_CONTROL 0x43
#define PIT_CONTROL_DATA 0x36

void Init_PIT8253(void);

#endif
