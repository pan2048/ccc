#ifndef _CG_H_
#define _CG_H_

#include "../parser/parser.h"

void cgDirectiveLineNumber(int line);

void cgFilePreamble(char *filename);
void cgFilePostamble();

void cgFunctionPreamble(struct symtable *sym);
void cgFunctionPostamble(struct symtable *sym);
int cgFunctionCall(struct symtable *sym, int numargs);
void cgFunctionCopyArg(int r, int argposn);

void cgGlobalSym(struct symtable *node);
void cgGlobalStr(int l, char *strvalue, int append);
void cgGlobalStrEnd(void);

int cgJmpBoolean(int r, int op, int label);
int cgCompareAndSet(int ASToperation, int r1, int r2, int type);
int cgCompareAndJumpTo(int ASToperation, int r1, int r2, int label, int type);
void cgOutputLabel(int label);
void cgJumpTo(int label);
void cgReturn(int reg, struct symtable *sym);

void cgRegisterMove(int r1, int r2);
int cgLoadLiteralInt(int value, int type);
int cgLoadVariable(struct symtable *sym, int op);
int cgLoadGlobalStr(int label);
void cgLoadBoolean(int r, int val);
int cgLoadAddress(struct symtable *sym);

int cgOpAdd(int r1, int r2);
int cgOpSub(int r1, int r2);
int cgOpMul(int r1, int r2);
int cgOpDivMod(int r1, int r2, int op);
int cgOpNegate(int r);
int cgOpInvert(int r);
int cgOpLogNot(int r);
int cgOpAnd(int r1, int r2);
int cgOpOr(int r1, int r2);
int cgOpXor(int r1, int r2);
int cgOpShl(int r1, int r2);
int cgOpShr(int r1, int r2);
int cgOpShlConst(int r, int val);

int cgPointerDeref(int r, int type);
int cgPointerStorDeref(int r1, int r2, int type);

int cgRegisterAlloc(void);
void cgRegisterFreeAll(int keepreg);
void cgRegisterFree(int reg);
void cgRegisterSpill();
void cgRegisterUnSpill();

int cgStoreGlobal(int r, struct symtable *sym);
int cgStoreLocal(int r, struct symtable *sym);

void cgSwitch(int reg, int casecount, int toplabel,
	      int *caselabel, int *caseval, int defaultlabel);

int cgTypeSize(int type);
int cgTypeAlign(int type, int offset, int direction);
int cgTypeWiden(int r, int oldtype, int newtype);

#endif