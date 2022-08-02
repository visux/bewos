#ifndef __IO_PORT_H
#define __IO_PORT_H

char inportb  (int numport);
void outportb (int portdata, int portnum);
unsigned char readcmos();
void writecmos( unsigned char, unsigned char );

#endif
