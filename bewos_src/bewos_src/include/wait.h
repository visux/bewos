#ifndef __WAIT_H
#define __WAIT_H

#include <atomic.h>

struct wait_obj {
	void *data;		/* Attributo dell'oggetto */
	struct wait_obj *next;	/* Puntatore al prossimo oggetto */
};

typedef struct wait_obj wait_obj_t;

struct wait {
	atomic_t count;			/* Numero di elementi in lista */
	void (*handler)(void *);	/* Handler della wait list */
	wait_obj_t objects;		/* Lista degli oggetti registrati */
};

extern int init_wait_list(struct wait *, void (*handler)(void *));
extern int wait_add_object(struct wait *, void *);
extern void wait_wakeup_object(struct wait *, void *);
extern void wait_wakeup_next(struct wait *);
extern void wait_wakeup_all(struct wait *);

static inline unsigned int waiting_objects(struct wait *wait)
{
	return atomic_read(&wait->count);
}

#endif
