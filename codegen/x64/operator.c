#include "x64.h"

// Add two registers
int cgOpAdd(int r1, int r2) 
{
  fprintf(Outfile, "\taddq\t%s, %s\n", registerList[r2], registerList[r1]);
  cgRegisterFree(r2);
  return (r1);
}

// Subtract the second register from the first
int cgOpSub(int r1, int r2)
{
  fprintf(Outfile, "\tsubq\t%s, %s\n", registerList[r2], registerList[r1]);
  cgRegisterFree(r2);
  return (r1);
}

// Multiply two registers
int cgOpMul(int r1, int r2)
{
  fprintf(Outfile, "\timulq\t%s, %s\n", registerList[r2], registerList[r1]);
  cgRegisterFree(r2);
  return (r1);
}

// Divide or modulo the first register by the second
int cgOpDivMod(int r1, int r2, int op)
{
  fprintf(Outfile, "\tmovq\t%s,%%rax\n", registerList[r1]);
  fprintf(Outfile, "\tcqo\n");
  fprintf(Outfile, "\tidivq\t%s\n", registerList[r2]);
  if (op == A_DIVIDE)
    fprintf(Outfile, "\tmovq\t%%rax,%s\n", registerList[r1]);
  else
    fprintf(Outfile, "\tmovq\t%%rdx,%s\n", registerList[r1]);
  cgRegisterFree(r2);
  return (r1);
}

// Bitwise AND two registers
int cgOpAnd(int r1, int r2)
{
  fprintf(Outfile, "\tandq\t%s, %s\n", registerList[r2], registerList[r1]);
  cgRegisterFree(r2);
  return (r1);
}

// Bitwise OR two registers
int cgOpOr(int r1, int r2)
{
  fprintf(Outfile, "\torq\t%s, %s\n", registerList[r2], registerList[r1]);
  cgRegisterFree(r2);
  return (r1);
}

// Bitwise XOR two registers
int cgOpXor(int r1, int r2)
{
  fprintf(Outfile, "\txorq\t%s, %s\n", registerList[r2], registerList[r1]);
  cgRegisterFree(r2);
  return (r1);
}

// Shift left r1 by r2 bits
int cgOpShl(int r1, int r2)
{
  fprintf(Outfile, "\tmovb\t%s, %%cl\n", registerByteList[r2]);
  fprintf(Outfile, "\tshlq\t%%cl, %s\n", registerList[r1]);
  cgRegisterFree(r2);
  return (r1);
}

// Shift right r1 by r2 bits
int cgOpShr(int r1, int r2)
{
  fprintf(Outfile, "\tmovb\t%s, %%cl\n", registerDoubleList[r2]);
  fprintf(Outfile, "\tshrq\t%%cl, %s\n", registerList[r1]);
  cgRegisterFree(r2);
  return (r1);
}

// Shift a register left by a constant
int cgOpShlConst(int r, int val)
{
  fprintf(Outfile, "\tsalq\t$%d, %s\n", val, registerList[r]);
  return (r);
}

// Negate a register's value
int cgOpNegate(int r)
{
  fprintf(Outfile, "\tnegq\t%s\n", registerList[r]);
  return (r);
}

// Invert a register's value
int cgOpInvert(int r)
{
  fprintf(Outfile, "\tnotq\t%s\n", registerList[r]);
  return (r);
}

// Logically negate a register's value
int cgOpLogNot(int r)
{
  fprintf(Outfile, "\ttest\t%s, %s\n", registerList[r], registerList[r]);
  fprintf(Outfile, "\tsete\t%s\n", registerByteList[r]);
  fprintf(Outfile, "\tmovzbq\t%s, %s\n", registerByteList[r], registerList[r]);
  return (r);
}
