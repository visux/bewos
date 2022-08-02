/* Black & White Operating System
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details (in the COPYING file).
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/******************************************INFO**********************************************
* Coders: Black, Baldosoft, Fain
* Descrizione: Definizioni di funzioni per la gestione delle stringe
***********************************************************************************************/


#include<string.h>
#include<io.h>
#include<math.h>

/* cerca il primo carattere @car e ritorna l'indirizzo di memoria */
void *memchr( const void *mem, int byte, size_t size )
{
/* secondo l'ansi C il carattere deve essere convertito in unsigned char */
    unsigned char car = ( unsigned char ) byte;
    const unsigned char *tmem = mem;
/* cerca un byte contenente il carattere @car */
    while( (*tmem != car) && size-- ) tmem++;
    
/* restituisce NULL se non sono stati trovati caratteri, l'indirizzo di memoria se e' stato trovato */
    return *tmem == car ? (void *)tmem : NULL;
}

/* compara @size bytes tra i dui puntatori, ritorna 0 se sono uguali altrimenti un numero diverso */
int memcmp(const void *mem1 ,const void *mem2 ,size_t count)
{
	const unsigned char *tmem1, *tmem2;

	for(tmem1 = mem1, tmem2 = mem2; count; ++tmem1, ++tmem2, count--)
		if((*tmem1 - *tmem2))
			break;
	return count;
}
/* copia  @size bytes da una zona di memoria all'altra */
void *memcpy( void *mem1, const void *mem2, size_t size )
{
    unsigned char *tmem1 = mem1;
    const unsigned char *tmem2 = mem2;
/* copia bytes fin quando size non e' 0 */
    while( size-- )
    {
        *tmem1 = *tmem2;
        
        tmem1++;
        tmem2++;
    }
/* ritorna l'indirizzo di memoria di destinazione */
    return tmem1;
}

/* copia @bytes caratteri dalla seconda zona di memoria alla prima nello stesso vettore */
void *memmove( void *mem1, const void *mem2, size_t bytes )
{
    unsigned char *tmem1 = mem1;
    const unsigned char *tmem2 = mem2;
    
    
    if( mem2 > mem1 )
    {
/* se l'indirizzo di origine  e' maggiore di quello di destinazione la copia e' regolare */
        while( bytes-- )
        {   
            *tmem1 = *tmem2;
            tmem1++;
            tmem2++;
        }
    }
    else
/* se l'indirizzo di origine e' minore di quello di destinazione la copia e' inversa */
    {
        tmem1 += bytes;
        tmem2 += bytes;
        
        while( bytes-- )
        {
            tmem1--;
            tmem2--;
            *tmem1 = *tmem2;
        }
    }
/* ritorna l'indirizzo di destinazione */
    return mem1;
}


/* setta a @byte @size numero di bytes di una zona di memoria */
void *memset( void *mem, int byte, size_t size )  
{
    unsigned char *tmem = mem;
    unsigned char car = ( unsigned char ) byte;
/* imposta i bytes a @byte fin quando size non e' 0 */
    while( size-- )
    {
        *tmem = car;
        tmem++;
    }
/* ritorna la zona di memoria di destinazione */
    return mem;
} 

/* aggiunge ad una stringa un'altra stringa */
char *strcat( char *str1, const char *str2 )
{
    char *t_str = str1;
    str1 += strlen( str1 );
/* copia la stringa fin quando *str2 non e' 0 */
    while( (*str1 = *str2) ) str1++, str2++;
    
/* segnala la fine della stringa con 0 */
    *str1 = 0;
/* ritorna la stringa di destinazione */
    return t_str;
}

/* cerca la prima occorrenza di @byte nella stringa data */
char *strchr( const char *str, int byte )
{
    unsigned char car = ( unsigned char ) byte;
/* cerca il carattere fin quando la stringa non e' finita */
    while( *str && *str != car) str++;
/* ritona str se il byte e' stato trovato, altrimenti NULL */
    return *str ? (char *)str : NULL;
}

/* compara 2 stringhe, ritorna 0 se sono uguali, 1 se la prima e' + lunga, -1 se e' + corta */
int strcmp( const char *str1, const char *str2 )
{
/* controlla la stringa fino a che non termina e le stringhe sono uguali */
    while( *str2 && *str1 == *str2 ) str1++, str2++;
    
/* se l'ultimo carattere e' uguale, le stringhe sono uguali, ritorna 0 */
    if( *str1 == *str2 )
        return 0;
/* se la prima stringa e' piu' corta della seoncda ritorna -1 */
    if( (!*str1) && *str2 )
        return -1;
/* se la prima stringa e' piu' lunga della seconda ritorna 1 */
    return 1;
}

/* compara 2 stringhe, ritorna 0 se sono uguali, 1 se la prima e' + lunga, -1 se e' + corta 
differisce da strcmp()  perche' e' uncase sensitive */
int strcasecmp( const char *str1, const char *str2 )
{
    char car1;
    char car2;
    
/* controlla la stringa fino a che non termina e le stringhe sono uguali */
    while( 1 )
    {
        car1 = *str1;
        car2 = *str2;
        
        str1++;
        str2++;
        
        if( car1 >= 'a' && car1 <= 'z' )
            car1 -= 32;
            
        if( car2 >= 'a' && car2 <= 'z' )
            car2 -= 32;
        
        if(! (car2 && car1 == car2) ) break;
    }
    
/* se l'ultimo carattere e' uguale, le stringhe sono uguali, ritorna 0 */
    if( car1 == car2 )
        return 0;
/* se la prima stringa e' piu' corta della seoncda ritorna -1 */
    if( (!car1) && car2 )
        return -1;
/* se la prima stringa e' piu' lunga della seconda ritorna 1 */
    return 1;
}


/*
Da implementare

int strcoll( const char *str1, const char *str2)
{
} 

*/   

/* copia una stringa */
char *strcpy( char *str1, const char *str2 )
{
    char *tstr = str1;
/* copia la stringa finche' i bytes sono diversi da 0 */
    while( (*str1 = *str2) ) str1++, str2++;
/* imposta la fine della stringa */
    *str1 = 0;
/* ritorna la stringa di destinazione */
    return tstr;
}

/* ritorna la lunghezza della stringa senza i caratteri della seconda stringa fornita */
size_t strcspn( const char *str, const char *car )
{
    const char *tcar;
    size_t n_car = 0;
    
    while( *str )
    {
/* assegna a tcar l'indirizzo originale */
        tcar = car;
    
        while( *tcar )
        {
/* controlla se almeno un carattere e' uguale, se e' uguale ritorna il numero
   di caratteri controllati */
            if( *str == *tcar ) return n_car;
            tcar++;
        }    
        str++;
        n_car++;
    }
/* ritorna il numero di caratteri */
    return n_car;
}   

/*
Da implementare
char *strerror( int err )
{
}    
*/

/* determina la lunghezza di una stringa */
size_t strlen( const char *str )
{
    const char *tstr = str;
    
    while( *str ) str++;
    
    return str - tstr;
} 

/* aggiunge alla stringa @num caratteri dalla seconda stringa */ 
char *strncat( char *str1, const char *str2, size_t num )
{
    char *tstr1 = str1;
/* si inizializza il puntatore all'ultimo byte della stringa */
    str1 += strlen( str1 );
/* copia i caratteri fin tanto che la stringa d'origine non e' terminata e num e' maggiore di 0 */
while( (*str1++ = *str2++) && --num );
/* segna la fine della stringa */
    *str1 = 0;
/* ritorna la stringa di destinazione */
    return tstr1;
}

/* compara @size caratteri di 2 stringhe, ritorna 0 se sono uguali, 1 se la prima e' + lunga,
   -1 se e' + corta */
int strncmp( const char *str1, const char *str2, size_t size )
{
    while( *str2 && *str1 == *str2 && size-- ) str2++,str1++;
/* se @size e' 0 ritorna 0 */
    if( ! size )
        return 0;
/* se @str1 e' piu' corta di @str2 ritorna -1 */
    if( (!*str1) && *str2 )
        return -1;
/* ritorna 1 in quanto @str2 e' + lunga di @str2 */
    return 1;
}

/* copia @num caratteri da una stringa all'altra */
char *strncpy( char *str1, const char *str2, size_t num )
{
    char *tstr = str1;
    
while( (*str1 = *str2) && --num ) str1++,str2++;
    
/* ritorna la stringa di destinazione */
    return tstr;
}

/* individua la prima occorrenza di un qualsiasi carattere contenuto nella seconda stringa e ritorna
   un puntatore a qust'ultimo. */
char *strpbrk( const char *str1, const char *str2 )
{
    const char *tstr2;
    
    while( *str1++ )
    {
        tstr2 = str2;
        
        while( *tstr2++ )
        {
/* se il carattere e' uguale ritorna il puntatore */
            if( *str1 == *tstr2 ) return (char *)str1;
        }
    }
/* se non trova il carattere ritorna NULL */
    return NULL;
}

/* cerca e ritorna l'ultimo carattere @byte nella stringa */
char *strrchr( const char *str, int byte )
{
    unsigned char car = ( unsigned char ) byte;
    const char *tstr = str;
/* inizializza il puntatore all'ultimo carattere */
    tstr += strlen( str );
/* controlla se nella stringa e' presente il carattere @car e che siamo ancora 
   all'interno della stringa */
    while( *tstr != car && tstr >= str ) tstr--;
/* ritorna un puntatore a @car se e' stato trovato il carattere, altrimenti ritorna NULL */

    return *tstr == car ? (char *)tstr : NULL;
}     

/* ritorna la lunghezza della stringa con i caratteri della seconda stringa fornita */
size_t strspn( const char *str, const char *car )
{
    const char *tstr = str;
    const char *tcar;
    size_t n_car = 0;
    
    while( *tstr )
    {
        tcar = car;
/* se un carattere e' diverso il ciclo viene bloccato e riotorna il numero di 
  caratteri controllati */
  
        while( *tcar && (*tstr != *tcar) ) tcar++;   
        
        if(! *tcar ) return n_car;
        
        tstr++;
        n_car++;
    }
/* ritorna il numero di caratteri controllati */
    return n_car;
} 

/* cerca la prima istanza della seconda stringa nella prima stringa e ritorna un puntatore */
char *strstr( const char *str1, const char *str2 )
{
/* se la seconda stringa e' vuota ritorna la prima stringa */
    if( strlen( str2 ) == 0 ) return (char *)str1;
    const char *tstr2 = str2;
    
    while( *str1++ )
    {
/* se il primo carattere della stringa e' uguale al primo carattere della seconda stringa
   compara l'intera stringa */
        if( *tstr2 == *str1 )
        {
            while( *tstr2 && *tstr2 == *str1 ) tstr2++,str1++;
            
/* se il puntatore alla seconda stringa ha raggiunto lo 0 ritorna un puntatore all'inizio della
   stringa trovata */
            if(! *tstr2 )
                return (char *)str1 - strlen(str2);
        }  
          
    }
/* se non sono state trovate istanze ritorna NULL */
    return NULL;
}

/* suddivide la stringa in piu' pezzi usando come separatori i caratteri dati */
char *strtok( char *str1, const char *str2 )
{
    static char *sstr;
    char *tstr1;
    const char *tstr2;
    
    if( str1 == NULL )
    {
/* se la prima stringa e' nulla verra' inizializzata con il valore della stringa statica ( @sstr ) */
        str1 = sstr;
        tstr1 = sstr;
    }
    else tstr1 = str1;   
/* se la prima stringa e' nulla anche dopo l'assegnazione dalla variabile statica ritorna NULL */
    if( str1 == NULL ) return NULL;
    
    while( *str1++ )
    {
        tstr2 = str2;
        
        while( *tstr2++ )
        {
/* controlla se il byte contiene una dei caratteri della seconda stringa */
            if( *str1 == *str2 )
            {
/* se la prima stringa contiene uno dei caratteri separatori: termina la stringa, assegna alla
  variabile statica il valore della stringa attuale + 1, ritorna la stringa controllata */
                *str1 = 0;
                sstr = str1 + 1;
                return tstr1;
            }
            
        }
        
    }
/* se non e' stato trovato un separatore si annulla il puntatore
   statico e si ritorna l'ultimo token */
    sstr = NULL;
    return tstr1;
}                

size_t strxfrm( char *str1, const char *str2, size_t size )
{
    strncpy( str1, str2, size);
    return strlen( str1 );
}

/********************************************************************************
********************************************************************************/
//Compara due stringhe. Se sono uguali restituisce 0 altrimenti restituisce 1
int stringcmp(const char *str1, const char *str2, int n)
{
  int i=0;

  while (*str1 == *str2 && i < n)
  {
    str1++;
    str2++;
    i++;
  }

  if(*str1 != *str2)
   return 1;

  else //ho messo questo else per rendere il codice più chiaro ma si poteva anche omettere
   return 0;
}

/********************************************************************************
********************************************************************************/
//Converte una stringa in un numero intero
int conv_string_int( char *buffer )
{
    char *TmpBuffer = buffer;
    int Numero = 0;
    
    while( *TmpBuffer )
    {
        Numero *= 10;      //moltiplichiamo per 10 e aggiungiamo una cifra
        Numero += *TmpBuffer - '0';
        TmpBuffer++;
    }
    
    return Numero;
}

/********************************************************************************
********************************************************************************/
//Converte un numero intero in una stringa
char* conv_int_string(long int num,int base)
{
  static char str_buffer[12];
  char *buffer=str_buffer;
  char *tmp_buffer;
  int i=0,j=0,num_neg=0;

  tmp_buffer=buffer;


   if(num == 0)
   {
     *buffer='0';
      j++;
   }


   else if(num < 0)
   {
     j++;
     num*=-1;     //Faccio diventare il numero positivo
     num_neg=1;   //utilizzo questa varibile per dopo
   }

 //Effettua la conversione. Ps: nel buffer vengono inseriti i caratteri nell' ordine inverso. Se il numero e' 12 verra' inserito 21
    while(num > 0)
    {
      *buffer++=(num % base) + '0';
      num /= base;
      i++;
    }


//Dunque ordiniamo correttamente il buffer. Se il numero nel buffer e' 21 diventa 12.
    while(i != 0)
    {
      i--;
      buffer[j]=tmp_buffer[i];
      j++;
    }


//se si tratta di un numero negativo mettiamo nell' indice 0 del buffer il segno meno , '-'
  if(num_neg==1)
    {
   buffer[0]='-';
    }

//fine stringa
  buffer[j]=0;

//ritorniamo il buffer. Ovvero il numero convertito in una stringa :)
 return buffer;
}

/********************************************************************************
********************************************************************************/
//Converte un float/double in una stringa
char* conv_float_string(double n, int precisione)
{


char* buf_int;
char* buf_dec;
static char _buffer[20];
char* buffer=_buffer;
int i=0,j=0;
//Spezzetto le due parti; la parte intera e la parte decimale

int parte_intera=(int)n;
double parte_decimale= (double)n-parte_intera;
int tmp_pi;
int tmp_precisione=precisione;

buf_int=conv_int_string(parte_intera, 10); //metto la parte intera in un buffer

if(precisione == 0) // Se precisione e' uguale a 0 allora devo stampare solo la parte intera
return buf_int;     // Infatti ritorno solo la parte intera


// Mi prendo la parte decimale di un numero
while(tmp_precisione!=0)
 {
  parte_decimale*=10;
  tmp_pi=(int)parte_decimale;
  buf_dec[i] = tmp_pi + '0';
  parte_decimale = (double)parte_decimale-tmp_pi;

    i++;
    tmp_precisione--;
 }

buf_dec[i]=0; //Chiudo la stringa contenente la parte decimale del numero
i=0;


//concatena la parte intera e la parte decimale... dividendola da un . (che sarebbe la virgola)
while(buf_int[i] != 0)
 {
  buffer[j]=buf_int[i];
  i++;
  j++;
 }

buffer[j]='.';
j++;
i=0;


while(buf_dec[i] != 0)
 {
  buffer[j]=buf_dec[i];
  i++;
  j++;
 }


  buffer[j]=0;  //Chiudo il buffer

return buffer; // ritorno il numero frazionario sottoforma di stringa
}
