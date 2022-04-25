#ifndef _CG_H_
#define _CG_H_

#include "../parser/parser.h"
#include "x64/x64.h"

void cgDirectiveLineNumber(int line);

void cgFilePreamble(char *filename);
void cgFilePostamble();

void cgFunctionPreamble(struct symTable *sym);
void cgFunctionPostamble(struct symTable *sym);
int cgFunctionCall(struct symTable *sym, int numargs);
void cgFunctionCopyArg(int r, int argposn);

void cgGlobalSym(struct symTable *node);
void cgGlobalStr(int lable, char *strvalue, int append);
void cgGlobalStrEnd(void);

int cgJmpBoolean(int r, int op, int label);
int cgCompareAndSet(int ASToperation, int r1, int r2, int type);
int cgCompareAndJumpTo(int ASToperation, int r1, int r2, int label, int type);
void cgOutputLabel(int label);
void cgJumpTo(int label);
void cgReturn(int reg, struct symTable *sym);

void cgRegisterMove(int r1, int r2);
int cgLoadLiteralInt(int value, int type);
int cgLoadVariable(struct symTable *sym, int op);
int cgLoadGlobalStr(int label);
void cgLoadBoolean(int r, int val);
int cgLoadAddress(struct symTable *sym);

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
int cgPointerStoreDeref(int r1, int r2, int type);

int cgRegisterAlloc(void);
void cgRegisterFreeAll(int regtokeep);
void cgRegisterFree(int reg);
void cgRegisterSpill();
void cgRegisterUnSpill();

int cgStoreGlobal(int r, struct symTable *sym);
int cgStoreLocal(int r, struct symTable *sym);

void cgSwitch(int reg, int casecount, int toplabel,
	      int *caselabel, int *caseval, int defaultlabel);

int cgTypeSize(int type);
int cgTypeAlign(int type, int offset, int direction);
int cgTypeWiden(int r, int oldtype, int newtype);

#endif