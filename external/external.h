#ifndef _EXTERNAL_H_
#define _EXTERNAL_H_

#include <stdio.h>

int inFileGetChar();
void inFilePipeOpen(char *cmd, char *fileName);
void outFileOpen(char *outFileName);
void fileUnlink(char *fileName);

extern FILE *Outfile; // Output file

void sysExit();
int sysRun(char *command);

void panic(char *s1, int lineNum, char *fileName);
void panics(char *s1, char *s2, int lineNum, char *fileName);
void panicd(char *s, int d, int lineNum, char *fileName);
void panicc(char *s, int c, int lineNum, char *fileName);

void *memAlloc(int size);
void *memRealloc(char *ptr, int size);

int print(int *varg,...);

#endif