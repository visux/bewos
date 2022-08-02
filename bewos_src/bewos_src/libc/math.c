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
* Descrizione: Definizioni di funzioni matematiche
***********************************************************************************************/



#include<math.h>

/********************************************************************************
********************************************************************************/
double pow(double x, double y)
{
 double ret=1;
 
 if(y == 0)
  return (double)1;
 
  while(y!=0)
  {
    ret*=x;
    y--;
   }

return ret;
}
