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
* Descrizione: Definizioni di funzioni per l' acquisizione di dati di input(dalla KeyBoard)
***********************************************************************************************/
 
 
 
 #include<io.h>
 #include<keyboard.h>
 #include<stdarg.h>
 #include<scank.h>
 #include<string.h>
 #include<syscall.h>
 
char buffer[BUF_SIZE]; //extern utilizzata in keyboard.c. In questo array vengono messi i caratteri digitati

 
void scank( const char *format, ...)
{


va_list arg;

va_start(arg, format);

unsigned int i = 0;
char* char_tmp;
int* int_tmp;
char* string_tmp;
char c;

char buf[BUF_SIZE];




while((c = getche()) != '\n') //getche abilita\disablita la keyboard e prende e ritorna il carattere digitato
{
	if( c == '\b' )  //se e' stato premuto backspace
	{
		i--;	   //decremento l' indice all' interno del buffer
		continue;  //continuo con l' esecuzione del ciclo
	}
		
	buf[i] = c;	//memorizzo il carattere digitato in un buffer
	i++; 		//incremento l' indice del buffer
}

buf[i] = 0;  //chiudo il buffer


i = 0;

 while(format[i] != 0)  {


    if(format[i]=='%')
    {
    	


       switch(format[++i])
	{


	case 'c':
	char_tmp = va_arg(arg,char*);
	*char_tmp = *buf;
	break;

	case 'd':
	case 'i':
	case 'u':
	int_tmp  = va_arg(arg,int*);
	*int_tmp = conv_string_int(buf);
	break;

	/* TODO */
	//case 'f':
	//break;

	
	case 's':
	string_tmp = va_arg(arg,char*);
	strcpy(string_tmp, buf);
	break;


	}

	
      va_end(arg);

}
i++;
}


}



