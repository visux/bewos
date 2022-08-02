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
* Descrizione: Rileva il tipo di floppy presente
***********************************************************************************************/

#include <floppy.h>
#include <io_port.h>

const char *S_FLOPPY[] = { 0, "360kb", "1.2mb", "720kb", "1.44mb", "2.88mb" };

unsigned char GetTypeFloppy( int NumeroDrive )
{
    unsigned char Info;
    unsigned char StatoFloppy = 0;
/*
Per leggere i dati del cmos e' necessario mandare l'offset del byte che si vuole leggere alla porta 0x70 (112d) e
leggere la risposta nella porta 0x71(113d) 
*/

    Info = readcmos( 0x10 ); /* leggo il byte 0x10 del cmos */

/*
risposte possibili:
    0 = non esistente, floppy non inserito
    1 = 360kb
    2 = 1.2mb
    3 = 720kb
    4 = 1.44mb
    5 = 2.88mb
*/
    if( NumeroDrive == 1 )
    {                            //prendiamo l'informazione del drive floppy richiesto( 1 o 2 )
        StatoFloppy = Info >> 4; //prendiamo solo gli ultimi 4 bits
    }
              
    if( NumeroDrive == 2 )
    {
        StatoFloppy = Info & 0x0F;
    }
    
/* spostiamo i bit perche' il CMOS ritorna lo stato di due drive floppy: nella parte `alta`
   quello del primo floppy e nella parte `bassa` quello del secondo. */
    return StatoFloppy;
}    


