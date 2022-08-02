#ifndef __TIME_H
#define __TIME_H

#include <string.h>
#include <io_port.h>

#define SECOLO 1900

struct tm
{
  int tm_sec;           /* secondi 0->59 */
  int tm_min;           /* minuti 0->59 */
  int tm_hour;          /* ore 0->23 */
  int tm_mday;          /* giorni 1->31 */
  int tm_mon;           /* mesi 0->11 */
  int tm_year;          /* anno - 1900.  */
  int tm_wday;          /* giorno della settimana 0->6 */
  int tm_yday;          /* giorno dell'anno 0->365 */
  int tm_isdst;         /* flag per l'ora legale */
};

#ifndef DEF_SIZE_T
typedef unsigned int size_t;
#define DEF_SIZE_T
#endif /* DEF_SIZE_T */

typedef unsigned long time_t; /* secondi passati dal 1970 */
typedef unsigned int suseconds_t; /* microsecondi */
typedef float clock_t; /* tempo di processore dello scheduler */

struct timeval {
	time_t		tv_sec;		/* secondi dallo 01/01/1970 0.0:0  */
	suseconds_t	tv_usec;	/* microsecondi */
};

struct timezone { // Questa struct non viene usata e viene sempre sostituita negli argomenti di questo tipo da un NULL
	int	tz_minuteswest;	/* minutes west of Greenwich */
	int	tz_dsttime;	/* type of dst correction */
};


#ifndef NULL
#define NULL ((void *)0)
#endif /* NULL */

clock_t clock();
/* torna di processore utilizzato, da implementare */

double difftime( time_t, time_t );
/* calcola la differenza tra il primo e il secondo dato ( primo - secondo ), restituisce la differenza in secondi */

time_t mktime( struct tm *);
/* ritorna l'equivalente di tm* in time_t, -1 se fallisce */

time_t time( time_t * );
/* ritorna e salva nel puntatore fornito ( se non e' nullo ) l'ora e la data corrente in formato time_t */

char *asctime( const struct tm * );
/* converte tm in un formato cosi': "Mon Sep 23 9:5:47 2005\n" e ne restituisce un puntatore */

char *ctime( const time_t * );
/* converte la data e l'ora come se si facesse asctime( localtime( tm * ) ); */

struct tm *gmtime( const time_t * );
/* converte da time_t a tm , restituisce NULL se fallisce */

struct tm *localtime( const time_t * );
/* converte da time_t a tm con l'ora locale */

size_t strftime( char *, size_t, const char *, const struct tm * );
/* scrive in una stringa la data nel formato imposto e ritorna il numero di bytes scritti */

int settimeofday(const struct timeval *, const struct timezone *);
/* imposta la data di sistema */

int GetCurrentTime( struct tm * );
/* funzione non ansi che scrive nella struct tm l'ora e la data corrente, torna negativo se fallisce */

#endif /* __TIME_H */
