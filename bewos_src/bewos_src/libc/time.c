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
* Coders: Baldosoft
* Descrizione: Libreria standar time
***********************************************************************************************/

#include<time.h>   



const char *Mesi[] = 
{
  "January", "February", "March",
  "April", "May", "June",
  "July", "August", "September",
  "October", "November", "December"
};

const char *MesiAbbreviati[] = 
{
  "Jan", "Feb", "Mar",
  "Apr", "May", "Jun",
  "Jul", "Aug", "Sep",
  "Oct", "Nov", "Dec"
};

const char *Giorni[] = 
{ "Sunday", "Monday","Tuesday",
  "Wednesday", "Thursday",
  "Friday", "Saturday"
};

const char *GiorniAbbreviati[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

const int NUMERO_GIORNI[] = {
31, // Gennaio
28,  // Febbraio - Viene calcolato dal programma
31, // Marzo
30, // Aprile
31, // Maggio
30, // Giugno
31, // Luglio
31, // Agosto
30, // Settembre
31, // Ottobre
30, // Novembre
31  // Dicembre
};


double difftime( time_t TimeMax, time_t TimeMin )
{
/*
time_t rappresenta il numero di secondi dal 1970, di conseguenza per fare la differenza sara' sufficente 
fare una semplice sottrazione
*/
    return TimeMax - TimeMin;
}


size_t strftime( char *Buffer, size_t BufferLen, const char *Formato, const struct tm *StTempo )
{
/* scrive in Buffer la stringa formato, e i caratteri preceduti da '%' vengono sostituiti dal loro corrispondente
   nell'ansi C. */
    size_t CaratteriScritti = 1; //lo inizializzo a 1 cosi' tengo sempre conto del byte di terminazione
/* nel caso vada tutto bene sara' restituito il numero di caratteri scritti, altrimenti sara' restituto 0 */
    *Buffer = 0;
    
    while( *Formato )
    {
        if( *Formato == '%' )
        {
            register char Carattere;
            char *Str;
            Carattere = *(++Formato);
            switch( Carattere )
            {
                case 'a':
                    if( strlen( GiorniAbbreviati[ StTempo-> tm_wday ] ) + CaratteriScritti > BufferLen )
                        return 0;
                    strcat( Buffer, GiorniAbbreviati[ StTempo-> tm_wday ] );
                    CaratteriScritti += strlen( GiorniAbbreviati[ StTempo-> tm_wday ] );
                    break;
                    
               case 'A':
                    if( strlen( Giorni[ StTempo-> tm_wday ] ) + CaratteriScritti > BufferLen )
                        return 0;
                    strcat( Buffer, Giorni[ StTempo-> tm_wday ] );
                    CaratteriScritti += strlen( Giorni[ StTempo-> tm_wday ] );
                    break;
                    
              case 'b':
                    if( strlen( MesiAbbreviati[ StTempo-> tm_mon ] ) + CaratteriScritti > BufferLen )
                        return 0;
                    strcat( Buffer, MesiAbbreviati[ StTempo-> tm_mon ] );
                    CaratteriScritti += strlen( MesiAbbreviati[ StTempo-> tm_mon ] );
                    break;
              case 'B':
                    if( strlen( Mesi[ StTempo-> tm_mon ] ) + CaratteriScritti > BufferLen )
                        return 0;
                    strcat( Buffer, Mesi[ StTempo-> tm_mon ] );
                    CaratteriScritti += strlen( Mesi[ StTempo-> tm_mon ] );
                    break;
              
              case 'c':
                    //formato: Tue Jul 12 14:35:46 2005
                    if( CaratteriScritti + 24 > BufferLen )
                        return 0;
                        
                    CaratteriScritti += 24;
                    
                    strcat( Buffer, GiorniAbbreviati[ StTempo->tm_wday ] );
                    strcat( Buffer, " " );
                    strcat( Buffer, MesiAbbreviati[ StTempo->tm_mon ] );
                    strcat( Buffer, " " );
                    Str = conv_int_string( StTempo->tm_mday, 10 );
                    strcat( Buffer, Str );
                    strcat( Buffer, " " );
                    Str = conv_int_string( StTempo->tm_hour, 10 );
                    strcat( Buffer, Str );
                    strcat( Buffer, ":" );
                    Str = conv_int_string( StTempo->tm_min, 10 );
                    strcat( Buffer,  Str );
                    strcat( Buffer, ":" );
                    Str = conv_int_string( StTempo->tm_sec, 10 );
                    strcat( Buffer, Str );
                    strcat( Buffer, " " );
                    Str = conv_int_string( StTempo->tm_year + SECOLO, 10 );
                    strcat( Buffer, Str );
                    
                    break;
              
              case 'd':
                    Str = conv_int_string( StTempo->tm_mday, 10 );
                    if( strlen( Str ) + CaratteriScritti > BufferLen )
                        return 0;
                    
                    strcat( Buffer, Str );
                    CaratteriScritti += strlen( Str );
                    break;
                    
              case 'H':
                    Str = conv_int_string( StTempo->tm_hour, 10 );
                    if( strlen( Str ) + CaratteriScritti > BufferLen )
                        return 0;
                    
                    strcat( Buffer, Str );
                    CaratteriScritti += strlen( Str );
                    break;
                    
              case 'I':
                    Str = conv_int_string( StTempo->tm_hour > 12 ? StTempo->tm_hour - 12 : StTempo->tm_hour, 10 );
                    if( strlen( Str ) + CaratteriScritti > BufferLen )
                        return 0;
                    
                    strcat( Buffer, Str );
                    CaratteriScritti += strlen( Str );
                    break;
                    
              case 'j':
                    Str = conv_int_string( StTempo->tm_yday, 10 );
                    if( strlen( Str ) + CaratteriScritti > BufferLen )
                        return 0;
                    
                    strcat( Buffer, Str );
                    CaratteriScritti += strlen( Str );
                    break;
                    
              case 'm':
                    Str = conv_int_string( StTempo->tm_mon + 1, 10 );
                    if( strlen( Str ) + CaratteriScritti > BufferLen )
                        return 0;
                    
                    strcat( Buffer, Str );
                    CaratteriScritti += strlen( Str );
                    break;
                    
              case 'M':
                    Str = conv_int_string( StTempo->tm_min, 10 );
                    if( strlen( Str ) + CaratteriScritti > BufferLen )
                        return 0;
                    
                    strcat( Buffer, Str );
                    CaratteriScritti += strlen( Str );
                    break;
                    
              case 'p':
                    if( CaratteriScritti + 2 > BufferLen )
                        return 0;
                    strcat( Buffer, StTempo->tm_hour > 12 ? "PM" : "AM" );
                    CaratteriScritti += 2;
                    break;
              
              case 'S':
                    Str = conv_int_string( StTempo->tm_sec, 10 );
                    if( strlen( Str ) + CaratteriScritti > BufferLen )
                        return 0;
                    
                    strcat( Buffer, Str );
                    CaratteriScritti += strlen( Str );
                    break;
              
              case 'U':
              case 'W':
                    //U dovrebbe stampare il numero di settimane dalla prima domenica e W dal primo lunedi'
                    //l'algoritmo e' stato semplificato
                    Str = conv_int_string( StTempo->tm_yday / 7, 10 );
                    if( strlen( Str ) + CaratteriScritti > BufferLen )
                        return 0;
                    
                    strcat( Buffer, Str );
                    CaratteriScritti += strlen( Str );
                    break;
                    
              case 'w':
                    Str = conv_int_string( StTempo->tm_wday, 10 );
                    if( strlen( Str ) + CaratteriScritti > BufferLen )
                        return 0;
                    
                    strcat( Buffer, Str );
                    CaratteriScritti += strlen( Str );
                    break;
                    
              case 'x':
                    //formato "07/12/05"
                    if( CaratteriScritti + 8 > BufferLen )
                        return 0;
                    
                    Str = conv_int_string( StTempo->tm_mday, 10 );
                    if( StTempo->tm_mday <= 9 )
                        strcat( Buffer, "0" ); 
                    strcat( Buffer, Str );
                    strcat( Buffer, "/" );
                    
                    Str = conv_int_string( StTempo->tm_mon + 1, 10 );
                    if( StTempo->tm_mon + 1 <= 9 )
                        strcat( Buffer, "0" ); 
                    strcat( Buffer, Str );
                    strcat( Buffer, "/" );
                    
                    Str = conv_int_string( StTempo->tm_year % 100, 10 );
                    if( StTempo->tm_year % 100 <= 9 )
                        strcat( Buffer, "0" ); 
                    strcat( Buffer, Str );                    
                    
                    break;
                    
                case 'X':
                     //formato: "15:35:48"
                    if( CaratteriScritti + 8 > BufferLen )
                        return 0;
                    
                    Str = conv_int_string( StTempo->tm_hour, 10 );
                    if( StTempo->tm_hour <= 9 )
                        strcat( Buffer, "0" ); 
                    strcat( Buffer, Str );
                    strcat( Buffer, ":" );
                    
                    Str = conv_int_string( StTempo->tm_min, 10 );
                    if( StTempo->tm_min <= 9 )
                        strcat( Buffer, "0" ); 
                    strcat( Buffer, Str );
                    strcat( Buffer, ":" );
                    
                    Str = conv_int_string( StTempo->tm_sec, 10 );
                    if( StTempo->tm_sec <= 9 )
                        strcat( Buffer, "0" ); 
                    strcat( Buffer, Str );
                    
                    break;
                    
                case 'y':
                    Str = conv_int_string( StTempo->tm_year, 10 );
                    if( strlen( Str ) + CaratteriScritti > BufferLen )
                        return 0;
                    
                    strcat( Buffer, Str );
                    CaratteriScritti += strlen( Str );
                    break;
              
                case 'Y':
                    Str = conv_int_string( StTempo->tm_year + 1900, 10 );
                    if( strlen( Str ) + CaratteriScritti > BufferLen )
                        return 0;
                    
                    strcat( Buffer, Str );
                    CaratteriScritti += strlen( Str );
                    break;
                
                case 'Z':
                    //fuso orario: non implementato
                    break;
                
                case '%':
                    if( ++CaratteriScritti > BufferLen )
                        return 0;
                        
                    strcat( Buffer,"%" );
                    CaratteriScritti++;
                    break;
             }
             Formato++;
        }
        else
        {
            if( ++CaratteriScritti > BufferLen )
                return 0;
            strncat( Buffer, Formato, 1 );
            Formato++;
        }
    }
    
    return CaratteriScritti - 1;
}


char *asctime( const struct tm *StTempo )
{
/* ritorna una stringa con il seguente formato: "Tue Jul 12 14:35:46 2005\n"  da una strcut tm */
    static char Str[26];
    Str[0] = 0;
    if( strftime( Str, 26, "%c\n", StTempo ) == 0 )
        return NULL;
    return Str;
}

char *ctime( const time_t *Tempo )
{
/* ritorna una stringa con il seguente formato: "Tue Jul 12 14:35:46 2005\n" da time_t */
    struct tm *StTempo;
    StTempo = localtime( Tempo );
    
    if( StTempo == NULL )
        return NULL;
    
    return asctime( StTempo );
}

struct tm *gmtime( const time_t *Tempo )
{
/* ritorna il tempo in ora locale, ma dato che viene usato solo il tempo in italiano, ritorno localtime() */
    return localtime( Tempo );
} 

struct tm *localtime( const time_t *Tempo )
{
    static struct tm StTempo;
    int Anno;
    int Mese;
    int Giorno;
    int GiornoDellaSettimana;
    int GiorniTotali = (*Tempo / 86400) - 1; /* mi ricavo i giorni passati dal 1970 */

    for( Anno = 2, Mese = 0, Giorno = 1, GiornoDellaSettimana = 3 ;  GiorniTotali ; Giorno++, GiorniTotali--, StTempo.tm_yday++ )
    {
        if( Mese == 12 )
        {
            StTempo.tm_yday = 0;
            Mese = 0;
            Anno++;
        }
        
        GiornoDellaSettimana++;
        if( GiornoDellaSettimana == 7 )
            GiornoDellaSettimana = 0;
        
        if( Giorno >= 28 )
        {
            if( Mese == 1 )
            {
                if( ( Anno % 4 != 0 ) || ( Anno % 4 == 0 && Giorno == 29 ) )
                {
                    Mese++;
                    Giorno = 0;
                    continue;
                }
            }
            
            if( Mese == 3 || Mese == 5 || Mese == 8 || Mese == 10 )
            {
                if( Giorno == 30 )
                {
                    Mese++;
                    Giorno = 0;
                    continue;
                }
                else
                    continue;
            }
            
            if( Giorno == 31 )
            {
                Mese++;
                Giorno = 0;
                continue;
            }
        }
    }
/* copio le informazioni ricavate nella struct tm */

    if( Anno % 4 )
    {
        Giorno++;
        
        if( Giorno > NUMERO_GIORNI[ Mese ] )
        {
            Giorno = 0;
            Mese++;
        }
        
        if( Mese == 12 )
        {
            StTempo.tm_yday = 0;
            Mese = 0;
            Anno++;
        }
        
        GiornoDellaSettimana++;
        if( GiornoDellaSettimana == 7 )
            GiornoDellaSettimana = 0;
    }
    
    StTempo.tm_mday = Giorno;
    StTempo.tm_mon = Mese;
    StTempo.tm_year = Anno + 68;
    StTempo.tm_wday = GiornoDellaSettimana;
    StTempo.tm_sec = *Tempo % 60;
    StTempo.tm_min = ( *Tempo / 60 ) % 60;
    StTempo.tm_hour = ( *Tempo / 3600 ) % 24;
    
    /*GiorniTotali = *Tempo / 86400;
    
    GiorniTotali -= (Anno / 4) * 366;
    GiorniTotali -= (Anno - (Anno / 4)) * 365;

    StTempo.tm_yday = GiorniTotali - 1;*/
    
    StTempo.tm_isdst = 0;
    
    return &StTempo;
}

time_t time( time_t *PntTempo )
{
    struct tm StTempo;
    time_t Tempo = 0;
    struct tm *Test;
/* prendiamo il tempo corrente */
    if( GetCurrentTime( &StTempo ) != 0)
        return -1;
    
/* ora lo convertiamo in time_t */
    Tempo = mktime( &StTempo );
/* se il puntatore non e' nullo copiamo Tempo nell'indirizzo fornito */

    Test = localtime( &Tempo );
    
    if( PntTempo )
        *PntTempo = Tempo;
    
    return Tempo;
}

time_t mktime( struct tm *StTempo )
{
    time_t Tempo;
/* questa parte serve per razzionalizzare le date "assurde" come ad esempio il 50 gennaio */

    StTempo->tm_min += StTempo->tm_sec / 60;
    StTempo->tm_sec %= 60;
    StTempo->tm_hour += StTempo->tm_min / 60;
    StTempo->tm_min %= 60;
    StTempo->tm_mday += StTempo->tm_hour / 24;
    StTempo->tm_hour %= 24;
    StTempo->tm_mon += StTempo->tm_mday / ( StTempo->tm_mon == 3 || StTempo->tm_mon == 5 || StTempo->tm_mon == 8 || StTempo->tm_mon == 10 ? 30 : StTempo->tm_mon != 1 ? 31 : StTempo->tm_year % 4 == 0 ? 29 : 28 );
    StTempo->tm_year += StTempo->tm_mon / 12; //i mesi sono da 0 a 11, quindi dodici sarebbe un tredicesimo mese
    StTempo->tm_mon %= 12; //togliamo i mesi che abbiamo sommato agli anni

/* trasformiamo minuti, ore, giorni, anni in secondi per ottere un tipo time_t */   
    Tempo = StTempo->tm_sec;
    Tempo += StTempo->tm_min * 60;
    Tempo += StTempo->tm_hour * 3600;
    Tempo += StTempo->tm_yday * 86400;
    Tempo += ( StTempo->tm_year / 4 ) * 31622400;
    Tempo += ( StTempo->tm_year - ( StTempo->tm_year / 4 ) ) * 31536000;
    
    Tempo -=  2209075000LL; //togliamo il numero di secondi passati dal 1900 al 1970

    return Tempo;
}

int GetCurrentTime( struct tm *PntStTempo )
{
    unsigned int Temp;
    int Mese;
    char byte_bcd;


/* utilizziamo il cmos per ottenere l'ora e la data corrente.
   il cmos ritorna alcuni dati in formato bcd, ovvero memorizzando una cifra alla volta in blocchi da 4 bit
   quindi useremo un piccolo algoritmo per ricavarci i bytes reali
*/
    byte_bcd = readcmos( 0x0 );
    PntStTempo->tm_sec = ( ( byte_bcd >> 4 ) *10 ) + ( byte_bcd & 0xF );
    
    byte_bcd = readcmos( 0x2 );
    PntStTempo->tm_min = ( ( byte_bcd >> 4 ) *10 ) + ( byte_bcd & 0xF );
    
    byte_bcd = readcmos( 0x4 );
    PntStTempo->tm_hour = ( ( byte_bcd >> 4 ) *10 ) + ( byte_bcd & 0xF );
    
    byte_bcd =  readcmos( 0x6 );
    PntStTempo->tm_wday = ( ( byte_bcd >> 4 ) *10 ) + ( byte_bcd & 0xF );
    PntStTempo->tm_wday --; //di default il giorno 0 e' domenica, lo correggiamo.
        
    byte_bcd =  readcmos( 0x7 );
    PntStTempo->tm_mday = ( ( byte_bcd >> 4 ) *10 ) + ( byte_bcd & 0xF );
    
    byte_bcd = readcmos( 0x8 );
    PntStTempo->tm_mon = ( ( byte_bcd >> 4 ) *10 ) + ( byte_bcd & 0xF );
    PntStTempo->tm_mon--; //i mesi del cmos vanno da 1 a 12, lo correggiamo.
    
    byte_bcd = readcmos( 0x9 );
    PntStTempo->tm_year = ( ( byte_bcd >> 4 ) *10 ) + ( byte_bcd & 0xF );
    PntStTempo->tm_year += 100; //il cmos ritorna solo le ultime due cifre dell'anno.

/* una volta conclusa la raccolta dei dati si ottengono i dati mancanti: yday */

    for( PntStTempo->tm_yday = PntStTempo->tm_mday,  Mese = PntStTempo->tm_mon;
    Mese; Mese-- )
    {
        if( Mese == 2 )
        {
            if( PntStTempo->tm_year % 4  == 0 )
                PntStTempo->tm_yday += 29;
            else
                PntStTempo->tm_yday += 28;
            continue;
        }
        
        if( Mese == 4 || Mese == 6 || Mese == 9 || Mese == 11 )
        {
            PntStTempo->tm_yday += 30;
            continue;
        }
            
        PntStTempo->tm_yday += 31;
    }
    
    PntStTempo->tm_isdst = 0;
      
    return 0;
}

int settimeofday(const struct timeval *tv, const struct timezone *tz) {
struct tm *newtime = 0;
char new_wday;
char new_mon;
char new_year;
// Prendo i secondi passati da 1970 della nuova data dalla struct timeval *tv e li converto in struct tm *newtime in modo da "spezzettare" i secondi in secondi,minuti,ore,giorni,mesi,anni e sapere il giorno della settimana (Lun,Mar,Mer...)
newtime = gmtime( &(tv->tv_sec) );
// I microsecondi vengono ignorati
// la struct timezone *tz viene ignorata
// Nella memoria cmos (che devo scrivere) i nuomeri sono espressi in BCD, cioe' ogni cifra e' in blocchi da 4 bits es:
// 12 = 1 2 = 0001 0010, quindi uso un piccolo algoritmo per converitire in bcd
writecmos (0x0,(((newtime->tm_sec / 10) << 4) | (newtime->tm_sec % 10))); // Scrivo i secondi presi da newtime
writecmos (0x2,(((newtime->tm_min / 10) << 4) | (newtime->tm_min % 10))); // Scrivo i minuti da newtime
writecmos (0x4,(((newtime->tm_hour / 10) << 4) | (newtime->tm_hour % 10))); // Scrivo le ore da newtime
// ATTENZIONE!!!: In bewos il giorno 0 e' lunedì, nella cmos (la memoria che dovro' scrivere per modificare la data e l'ora di sistema) il giorno 0 e' la domenica quindi faccio questra trasformazione
new_wday = newtime->tm_wday + 1; // Aumento di 1
if (new_wday == 7) new_wday = 0; // Se e' 7 vuol dire che e' una domenica, quindi imposto a 0
writecmos (0x6,(((new_wday / 10)<<4) | (new_wday % 10)));// Scrivo il giorno della settimana appena calcolato
writecmos (0x7,(((newtime->tm_mday / 10)<<4) | (newtime->tm_mday % 10))); // Scrivo il giorno del mese da newtime (1-31)
// In bewos i mesi vano da 0 a 11, nella cmos da 1 a 12 quindi...
new_mon = newtime->tm_mon + 1; // ...aumento di 1
writecmos (0x8,(((new_mon/10) << 4) | (new_mon % 10))); // Scrivo il mese appena calcolato
// La cmos memorizza solo le due cifre finali dell'anno quindi le prendo...
new_year = newtime->tm_year % 100; // Basta prendere il resto della divisione per 100
writecmos (0x9,(((new_year / 10) << 4) | (new_year % 10))); // ...e le scrivo
return 0;
}
