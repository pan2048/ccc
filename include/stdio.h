#ifndef _STDIO_H_
# define _STDIO_H_

#include <stddef.h>

#ifndef NULL
# define NULL (void *)0
#endif

#ifndef EOF
# define EOF (-1)
#endif
// This FILE definition will do for now
typedef char * FILE;

FILE *fopen(char *pathname, char *mode);
int fclose(FILE *stream);

int fprintf(FILE *stream, char *format);
int printf(char *format);
int snprintf(char *str, size_t size, char *format);
int fputs(char *s, FILE *stream);
int fputc(int c, FILE *stream);
int fgetc(FILE *stream);
FILE *popen(char *command, char *type);


extern FILE *stderr;
extern FILE *stdout;

#endif	// _STDIO_H_
