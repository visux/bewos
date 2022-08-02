#ifndef __TIMER_H
#define __TIMER_H

#include <pit8253.h>

struct timer {
	unsigned long delay;		/* Espressa in colpi di clock */
	void (*handler)(void *);	/* L'handler del timer */
	void *data;			/* Parametro utilizzato dall'handler */
	struct timer *next;		/* Puntatore al prossimo elemento */
};

#define BAD_TIMER	(struct timer *)0xbad66666

/* Converte un ritardo espresso in millisecondi in colpi di clock */
static inline unsigned long msecs_to_ticks(unsigned long msecs)
{
	return (unsigned long)(msecs / MS_PIT);
}

extern struct timer *add_timer(unsigned long, void(*handler)(void *), void *);
extern void del_timer(struct timer *);
extern struct timer *mod_timer(struct timer *, unsigned long);

extern void isr_timer(void);
extern inline unsigned long show_timer_ticks(void);


extern struct timer *sys_do_timer(int, struct timer *, unsigned long,
				  void(*handler)(void *), void *);

/* Queste sono le operazioni sui timer riconosciute dall'apposita syscall */
#define TIMER_ADD	0x1
#define TIMER_DEL	0x2
#define TIMER_MOD	0x3

/* I task in user space devono usare queste macro per la gestione dei timer */
#define register_timer(ms, handler, data)	do_timer(TIMER_ADD, 0, ms, handler, data)
#define unregister_timer(timer)			do_timer(TIMER_DEL, timer, 0, NULL, NULL)
#define modify_timer(timer, ms)			do_timer(TIMER_MOD, timer, ms, NULL, NULL)

#endif
