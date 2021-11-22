#ifndef _GEN_H_
#define _GEN_H_

#include "../parser/parser.h"

int getLabel(void);

void genPreamble(char *filename);
void genPostamble();
void genGlobSym(struct symtable *node);
int genGlobStr(char *strvalue, int append);
void genGlobStrEnd();
int genPrimSize(int type);
int genAlign(int type, int offset, int direction);
void genLineNumber(int linenum);

int genFuncCall(struct ASTnode *n);
int genIf(struct ASTnode *n, int looptoplabel, int loopendlabel);
int genLogAndOr(struct ASTnode *n); 
int genSwitch(struct ASTnode *n);
int genWhile(struct ASTnode *n);
int genTernary(struct ASTnode *node);

int genAST(struct ASTnode *n, int iflabel, int looptoplabel,
	   int loopendlabel, int parentASTop);

#endif