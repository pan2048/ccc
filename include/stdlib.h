#ifndef _STDLIB_H_
# define _STDLIB_H_

void exit(int status);

void *malloc(int size);
void *realloc(void *ptr, int size);
int system(char *command);

#endif	// _STDLIB_H_
