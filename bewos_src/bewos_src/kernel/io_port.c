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
* Coders: Black
* Descrizione: Definizioni di funzioni che leggono\scrivono dalle\sulle porte di I\O
***********************************************************************************************/



char inportb  (int numport)
{
  char valore;
  __asm__ volatile ("inb %%dx,%%al":"=a" (valore):"d" (numport));
  return valore;
 }
/********************************************************************************
********************************************************************************/
 void outportb (int portdata, int portnum)
 {
   __asm__ volatile ("outb %%al,%%dx"::"a" (portdata),"d" (portnum));
  return;
  }

unsigned char readcmos( unsigned char offset )
{    
    unsigned char byte_bcd;
    
    __asm__ (
        "mov %1,%%al\n\t"
        "out %%al,$0x70\n\t"
        "mov $64,%%cx\n\t"
        "Ciclo: loop Ciclo\n\t"
        "in $0x71,%%al\n\t"
        "mov %%al,%0\n\t"
        : "=r" (byte_bcd) : "r" (offset) : "%al", "%cx"
    );

    return byte_bcd;
}

void writecmos( unsigned char offset, unsigned char byte )
{
    __asm__ (
        "mov %0,%%al\n\t"
        "out %%al,$0x70\n\t"
        "mov %1,%%al\n\t"
        "out %%al,$0x71\n\t"
        :: "r" (offset), "r" (byte) : "%al" 
    );
}
