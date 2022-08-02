#ifndef __STRING_H
#define __STRING_H

#ifndef NULL
#define NULL ((void *)0)
#endif /* NULL */

#ifndef DEF_SIZE_T
typedef unsigned int size_t;
#define DEF_SIZE_T
#endif /* DEF_SIZE_T */

void* memchr (const void*, int, size_t);
int memcmp (const void*, const void*, size_t);
void *memcpy (void*, const void*, size_t);
void *memmove (void*, const void*, size_t);
void *memset (void*, int, size_t);
char *strcat (char*, const char*);
char *strchr (const char*, int);
int strcmp (const char*, const char*);
int strcasecmp( const char *, const char *);
int strcoll (const char*, const char*);	
char *strcpy (char*, const char*);
size_t strcspn (const char*, const char*);
char *strerror (int);
size_t strlen (const char*);
char *strncat (char*, const char*, size_t);
int strncmp (const char*, const char*, size_t);
char *strncpy (char*, const char*, size_t);
char *strpbrk (const char*, const char*);
char *strrchr (const char*, int);
size_t strspn (const char*, const char*);
char *strstr (const char*, const char*);
char *strtok (char*, const char*);
size_t strxfrm (char*, const char*, size_t);

int stringcmp(const char *str1, const char *str2, int n);
char* conv_int_string(long int num, int base);
char* conv_float_string(double n, int precisione);
int conv_string_int(char *buffer);

#endif /* __STRING_H */
