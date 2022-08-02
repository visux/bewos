#ifndef __FLOPPY_H
#define __FLOPPY_H

#define FLOPPY_360kb    1
#define FLOPPY_1_2mb    2	
#define FLOPPY_720kb    3	
#define FLOPPY_1_44mb   4	
#define FLOPPY_2_88mb   5	
#define NO_FLOPPY       0

extern const char *S_FLOPPY[];

unsigned char GetTypeFloppy( int );
/* richiede il numero del drive floppy ( 1 o 2 ) e ritorna lo stato del drive da 0 a 5  */

#endif /* __FLOPPY_H */
