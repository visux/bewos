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
* Descrizione: Definizioni di funzioni che "Strettamente" riguardano il sistema
***********************************************************************************************/



#include<system.h>

int read_cur_privilege() 
{
int cs_value;
	read_cs_register(cs_value); /*leggo il valore di cs attraverso la macro read_cs_register e metto questo valore nella variabile
	 "cs_value" */
	
	return (cs_value & 0x3); //ritorno il CPL - Current Privilege Level (identificato dai primi due bit del valore di CS)
}
