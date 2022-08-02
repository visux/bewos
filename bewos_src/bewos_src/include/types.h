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
* Coders: Demone^ && Matrixina
* Descrizione: Dichiarazione estensione di ext2file
***********************************************************************************************/

#ifndef __TYPES_H
#define __TYPES_H


// definizione di tipi che serviranno per lo struct della ext2_file
typedef unsigned long u_int32_t;
typedef signed long int32_t;

// intero a 32 bit
typedef u_int32_t size_t;

#define BITS_PER_LUNGHEZZA 32

// attributi per la lunghezza e l'allineamento. Si rifà ai 32 bit definiti 
#define asmlinkage __attribute__((regparm(0)))
#define __cacheline_aligned __attribute__((aligned(32)))

#endif 
/** __TYPES_H__ 
*/
