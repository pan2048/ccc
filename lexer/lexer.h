#ifndef _LEXER_H_
#define _LEXER_H_

#include "../lib/lib.h"
#include "../model/token.h"

enum {
  TEXTLEN = 512			// Length of identifiers in input
};
extern char Text[TEXTLEN + 1];
extern int Linestart;	
extern int Putback;	

int next();
int peek();
void putback(int c);
int preProcess(int c);
int skip();

int scanInteger(struct token *t, int c);
int scanChar(void);
int scanCharacter(struct token *t, int c);
int scanString(struct token *t, int c);
int scanLiteral(struct token* t, int c);
int scanIdentifer(struct token *t, int c) ;
int scanOperator(struct token *t, int c);
int scan(struct token *t);

#endif