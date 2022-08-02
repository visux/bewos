#ifndef __SEMAPHORES_H
#define __SEMAPHORES_H

#include <atomic.h>
#include <wait.h>

typedef struct {
	atomic_t resources;		/* Variabile di stato */
	struct wait waiting;		/* Lista dei task in attesa */
} sem_t;

sem_t *create_sem(int value);
/* Questa macro permette di creare con facilità un mutex */
#define create_mutex()	create_sem(1)
void delete_sem(sem_t *semaphore);

void down(sem_t *semaphore);
void up(sem_t *semaphore);

extern void sys_do_sem(int, sem_t *);

/* Queste sono le operazioni sui semafori riconosciute dall'apposita syscall */
#define DOWN_SEM	0x1
#define UP_SEM		0x2

/* I task a ring 3 devono usare queste macro per la gestione dei semafori */
#define down_sem(sem)	do_sem(DOWN_SEM, sem)
#define up_sem(sem)	do_sem(UP_SEM, sem)

#endif
