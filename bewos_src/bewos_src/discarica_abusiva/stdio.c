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
* Descrizione: standar stdio
***********************************************************************************************/

#include <keyboard.h>
#include <stdio.h>

#define BUFF_GETCHAR 1024


/* La getchar legge tutti i caratteri digitati fino a quando non si preme invio e li ritorna man mano tutte le volte che viene chiamata la getchar stessa. In pratica quando viene chiamata la prima volta legge tutti i caratteri e li memorizza in un buffer e ritorna il primo carattere all' interno del buffer, quando viene richiamata(per la seconda volta) ritorna il secondo carattere ecc...

while((c = getchar()) != '\n')
	printk("%c", c);
	
	
questo codice in pratica effettua i seguenti passaggi:

- chiama la getchar che legge e memorizza in un buffer tutti i caratteri digitati fino a quando non si preme invio
- viene ritornato il primo carattere presente nel buffer
- Alla prossima chiamata della getchar viene restituito il secondo carattere presente nel buffer e cosi' via

Notare che la getchar viene chiamata fino a quando essa non restituisce '\n'; infatti facciamo: while((c = getchar()) != '\n')
*/

char getchar()
{
    static char Buffer[ BUFF_GETCHAR ];
    static char *Puntatore;
    char Carattere;
    
    if(! Puntatore )        //se Puntatore e' nullo gli assegno il valore di Buffer
        Puntatore = Buffer;
    
    if( Puntatore  == Buffer + BUFF_GETCHAR )   
        *Puntatore = 0;
/* se puntatore ha raggiunto il limite del buffer assegno all'ultimo carattere 0, cosi' facendo il "ciclo di ritorno" 
dei caratteri si ferma */
   
    if(! *Puntatore )
    {
        Puntatore = Buffer;
        
        while( ( *Puntatore  = sys_getche() ) != '\n' )
        {
            if( Puntatore  == Buffer + BUFF_GETCHAR )
                break;
/* fin quando non viene premuto invio o il buffer viene riempito non esce dal ciclo */
            if( *Puntatore == '\b' ) /* se viene premuto il tasto backspace torniamo indietro */
            {   
                *Puntatore = 0;
                Puntatore -=2 ;
            }
                
            Puntatore++;
            
        }
        
        Puntatore = Buffer;
        Carattere = *Puntatore;
        *Puntatore = 0;
        Puntatore++;
        
        return Carattere;
    }
    else        /* se ci sono salvati caratteri nel buffer li ritorniamo */
    {
        Carattere = *Puntatore;
        *Puntatore = 0;
        Puntatore++;
        
        return Carattere;
    }
    
}     
