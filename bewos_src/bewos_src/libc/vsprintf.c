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
* Descrizione: funzione vsprintf e sprintf
***********************************************************************************************/



#include<stdarg.h>
#include<io.h>


int vsprintf(char *buf, const char *string, va_list args) 
{


int i = 0, n, len, j, tmp_precisione;
char *s;

	for(i = 0; string[i] != 0; i++) 
	{

 //Questi if controllano se nella specifica di conversione e' stata specificata anche la precisione.. la sintassi e' %.nf dove
 //n indica la precisione dei numeri in virgola mobile
		if(string[i]=='%')
		{
			if(string[i+1]=='.')
			{
				tmp_precisione=string[i+2]-'0';
 				if(tmp_precisione == 0  || tmp_precisione < 10)
				{
					precisione=tmp_precisione;
					i+=2;
				}
			}
				
				
			switch(string[++i])
			{
				
				
				case 'c':
				*buf++ = (unsigned char)va_arg(args,int);
				break;
				
				case 'd':
				case 'i':
				case 'u':
				n = va_arg(args,int);
				s = (char*)conv_int_string(n, 10); //converto l' intero in una stringa
				len = strlen(s);	//calcolo la lunghezza della stringa(il numero convertito in stringa)
				for(j = 0; j < len; j++)		//metto di volta in volta ogni carattere della stringa nel buffer
				*buf++ = *s++;	
				break;
				
				case 'f':
				n = va_arg(args,int);
				s = (char*)conv_float_string(n, precisione); //converto l' intero in una stringa
				len = strlen(s);	//calcolo la lunghezza della stringa(il numero convertito in stringa)
				for(j = 0; j < len; j++)		//metto di volta in volta ogni carattere della stringa nel buffer
				*buf++ = *s++;	
				break;
				
				case 's':
				s = va_arg(args,char *);
				len = strlen(s);	//calcolo la lunghezza della stringa(il numero convertito in stringa)
				for(j = 0; j < len; j++) 		//metto di volta in volta ogni carattere della stringa nel buffer
				*buf++ = *s++;	
				break;
				
				case '%':
				*buf++ = '%';
				break;
				
			} //chiude lo switch
		
		} //chiude l' if
		
		else 
			*buf++ = string[i];
			
			
	} //chiude il for

	*buf = '\0'; //chiudo la stringa

	return strlen(buf);

} //chiude la funzione




int sprintf(char *buf, const char *string, ...)
{
	va_list args;
	int len;

	va_start(args, string);
	len = vsprintf(buf,string,args);
	va_end(args);

	return len;
}
