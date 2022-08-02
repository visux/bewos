#ifndef __ATOMIC_H
#define __ATOMIC_H

typedef struct { volatile int value; } atomic_t;

/* Inizializza la variabile atomica scelta con il valore desiderato */
#define ATOMIC_INIT(val)		{ (val) }

/* Legge il valore della variabile atomica scelta */
#define atomic_read(atomic)		((atomic)->value)

/* Imposta il valore della variabile atomica scelta con il valore desiderato */
#define atomic_set(atomic, val)		(((atomic)->value) = (val))

/* Incrementa di un'unità il valore della variabile atomica scelta */
static inline void atomic_inc(atomic_t *atomic)
{
	asm volatile (
		"LOCK incl %0 \n"
		: : "m"(atomic->value)
	);
}

/* Decrementa di un'unità il valore della variabile atomica scelta */
static inline void atomic_dec(atomic_t *atomic)
{
	asm volatile (
		"LOCK decl %0 \n"
		: : "m"(atomic->value)
	);
}

/* Somma al valore della variabile atomica scelta il numero desiderato */
static inline void atomic_add(atomic_t *atomic, int val)
{
	asm volatile (
		"LOCK addl %1, %0 \n"
		: : "m"(atomic->value), "ir"(val)
	);
}

/* Sottrae al valore della variabile atomica scelta il numero desiderato */
static inline void atomic_sub(atomic_t *atomic, int val)
{
	asm volatile (
		"LOCK subl %1, %0 \n"
		: : "m"(atomic->value), "ir"(val)
	);
}

/*
 * Incrementa di un'unità il valore della variabile atomica scelta
 * Restituisce 1 se la variabile atomica è positiva, 0 altrimenti
 */
static inline int atomic_inc_and_test(atomic_t *atomic)
{
	unsigned char test;	
	
	asm volatile (
		"LOCK incl %1 \n"
		"setg %0 \n"
		: "=qm"(test)
		: "m"(atomic->value)
	);
	return test;
}

/*
 * Decrementa di un'unità il valore della variabile atomica scelta
 * Restituisce 1 se la variabile atomica è positiva, 0 altrimenti
 */
static inline int atomic_dec_and_test(atomic_t *atomic)
{
	unsigned char test;
	
	asm volatile (
		"LOCK decl %1 \n"
		"setg %0 \n"
		: "=qm"(test)
		: "m"(atomic->value)
	);
	return test;
}

/*
 * Somma al valore della variabile atomica scelta il numero desiderato
 * Restituisce 1 se la variabile atomica è positiva, 0 altrimenti
 */
static inline int atomic_add_and_test(atomic_t *atomic, int val)
{
	unsigned char test;	
	
	asm volatile (
		"LOCK addl %2, %1 \n"
		"setg %0 \n"		
		: "=qm"(test)
		: "m"(atomic->value), "ir"(val)
	);
	return test;
}

/*
 * Sottrae al valore della variabile atomica scelta il numero desiderato
 * Restituisce 1 se la variabile atomica è positiva, 0 altrimenti
 */
static inline int atomic_sub_and_test(atomic_t *atomic, int val)
{
	unsigned char test;
	
	asm volatile (
		"LOCK subl %2, %1 \n"
		"setg %0 \n"		
		: "=qm"(test)
		: "m"(atomic->value), "ir"(val)
	);
	return test;
}

/*
 * Incrementa di un'unità il valore della variabile atomica scelta
 * Restituisce 1 se la variabile atomica è negativa, 0 altrimenti
 */
static inline int atomic_inc_and_test_neg(atomic_t *atomic)
{
	unsigned char test;	
	
	asm volatile (
		"LOCK incl %1 \n"
		"sets %0 \n"
		: "=qm"(test)
		: "m"(atomic->value)
	);
	return test;
}

/*
 * Decrementa di un'unità il valore della variabile atomica scelta
 * Restituisce 1 se la variabile atomica è negativa, 0 altrimenti
 */
static inline int atomic_dec_and_test_neg(atomic_t *atomic)
{
	unsigned char test;
	
	asm volatile (
		"LOCK decl %1 \n"
		"sets %0 \n"
		: "=qm"(test)
		: "m"(atomic->value)
	);
	return test;
}

/*
 * Somma al valore della variabile atomica scelta il numero desiderato
 * Restituisce 1 se la variabile atomica è negativa, 0 altrimenti
 */
static inline int atomic_add_and_test_neg(atomic_t *atomic, int val)
{
	unsigned char test;	
	
	asm volatile (
		"LOCK addl %2, %1 \n"
		"sets %0 \n"		
		: "=qm"(test)
		: "m"(atomic->value), "ir"(val)
	);
	return test;
}

/*
 * Sottrae al valore della variabile atomica scelta il numero desiderato
 * Restituisce 1 se la variabile atomica è negativa, 0 altrimenti
 */
static inline int atomic_sub_and_test_neg(atomic_t *atomic, int val)
{
	unsigned char test;
	
	asm volatile (
		"LOCK subl %2, %1 \n"
		"sets %0 \n"		
		: "=qm"(test)
		: "m"(atomic->value), "ir"(val)
	);
	return test;
}

/*
 * Incrementa di un'unità il valore della variabile atomica scelta
 * Restituisce 1 se la variabile atomica è pari a zero, 0 altrimenti
 */
static inline int atomic_inc_and_test_zero(atomic_t *atomic)
{
	unsigned char test;	
	
	asm volatile (
		"LOCK incl %1 \n"
		"setz %0 \n"
		: "=qm"(test)
		: "m"(atomic->value)
	);
	return test;
}

/*
 * Decrementa di un'unità il valore della variabile atomica scelta
 * Restituisce 1 se la variabile atomica è pari a zero, 0 altrimenti
 */
static inline int atomic_dec_and_test_zero(atomic_t *atomic)
{
	unsigned char test;
	
	asm volatile (
		"LOCK decl %1 \n"
		"setz %0 \n"
		: "=qm"(test)
		: "m"(atomic->value)
	);
	return test;
}

/*
 * Somma al valore della variabile atomica scelta il numero desiderato
 * Restituisce 1 se la variabile atomica è pari a zero, 0 altrimenti
 */
static inline int atomic_add_and_test_zero(atomic_t *atomic, int val)
{
	unsigned char test;	
	
	asm volatile (
		"LOCK addl %2, %1 \n"
		"setz %0 \n"		
		: "=qm"(test)
		: "m"(atomic->value), "ir"(val)
	);
	return test;
}

/*
 * Sottrae al valore della variabile atomica scelta il numero desiderato
 * Restituisce 1 se la variabile atomica è pari a zero, 0 altrimenti
 */
static inline int atomic_sub_and_test_zero(atomic_t *atomic, int val)
{
	unsigned char test;
	
	asm volatile (
		"LOCK subl %2, %1 \n"
		"setz %0 \n"		
		: "=qm"(test)
		: "m"(atomic->value), "ir"(val)
	);
	return test;
}


#endif
