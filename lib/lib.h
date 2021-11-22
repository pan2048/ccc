#ifndef _LIB_H_
#define _LIB_H_

#include "../external/external.h"

extern char *Outfilename; // Name of file we opened as Outfile
extern char *Infilename;  // Name of file we are parsing
extern int LineNumber;    // Current line number

int charIsAlpha(int c);
int charIsDigit(int c);
int charIsXDigit(int c);
int charToLower(int c);
int charPos(char *s, int c);

void fatal(char *s1);
void fatals(char *s1, char *s2);
void fatald(char *s, int d);
void fatalc(char *s, int c);

char *fileNameAlterSuffix(char *str, char suffix);

int strLen(char *str);
char *strDup(char *s);
int strCmp(char *p1, char *p2);
char *strRchr(char *s, int c);
char *strChr(char *s, int c);

void *memCopy(char *dest, char *src, int len);

#endif
