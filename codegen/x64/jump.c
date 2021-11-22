#include "x64.h"

int cgJmpBoolean(int r, int op, int label)
{
  fprintf(Outfile, "\ttest\t%s, %s\n", registerList[r], registerList[r]);
  switch (op)
  {
  case A_IF:
  case A_WHILE:
  case A_LOGAND:
    fprintf(Outfile, "\tje\tL%d\n", label);
    break;
  case A_LOGOR:
    fprintf(Outfile, "\tjne\tL%d\n", label);
    break;
  case A_TOBOOL:
    fprintf(Outfile, "\tsetnz\t%s\n", registerByteList[r]);
    fprintf(Outfile, "\tmovzbq\t%s, %s\n", registerByteList[r], registerList[r]);
  default:
    fatal("Impossible in cgJmpBoolean");
  }
  return (r);
}

// List of comparison instructions in AST order: 
// A_EQ, A_NE, A_LT, A_GT, A_LE, A_GE
static char *comparisonInstructions[] =
    {"sete", "setne", "setl", "setg", "setle", "setge"};

// Compare two registers and set if true.
int cgCompareAndSet(int ASToperation, int r1, int r2, int type)
{
  int size = cgTypeSize(type);

  // Check the range of the AST operation
  if (ASToperation < A_EQ || ASToperation > A_GE)
    fatal("Bad ASTop in cgCompareAndSet()");

  switch (size)
  {
  case 1:
    fprintf(Outfile, "\tcmpb\t%s, %s\n", registerByteList[r2], registerByteList[r1]);
    break;
  case 4:
    fprintf(Outfile, "\tcmpl\t%s, %s\n", registerDoubleList[r2], registerDoubleList[r1]);
    break;
  default:
    fprintf(Outfile, "\tcmpq\t%s, %s\n", registerList[r2], registerList[r1]);
  }

  fprintf(Outfile, "\t%s\t%s\n", comparisonInstructions[ASToperation - A_EQ], registerByteList[r2]);
  fprintf(Outfile, "\tmovzbq\t%s, %s\n", registerByteList[r2], registerList[r2]);
  cgRegisterFree(r1);
  return (r2);
}

// Generate a label
void cgOutputLabel(int label)
{
  fprintf(Outfile, "L%d:\n", label);
}

// Generate a jump to a label
void cgJumpTo(int label)
{
  fprintf(Outfile, "\tjmp\tL%d\n", label);
}

// List of inverted jump instructions in AST order: 
// A_EQ, A_NE, A_LT, A_GT, A_LE, A_GE
static char *invcmplist[] = {"jne", "je", "jge", "jle", "jg", "jl"};

// Compare two registers and jump if false.
int cgCompareAndJumpTo(int ASToperation, int r1, int r2, int label, int type)
{
  int size = cgTypeSize(type);

  // Check the range of the AST operation
  if (ASToperation < A_EQ || ASToperation > A_GE)
    fatal("Bad ASTop in cgcompare_and_set()");

  switch (size)
  {
  case 1:
    fprintf(Outfile, "\tcmpb\t%s, %s\n", registerByteList[r2], registerByteList[r1]);
    break;
  case 4:
    fprintf(Outfile, "\tcmpl\t%s, %s\n", registerDoubleList[r2], registerDoubleList[r1]);
    break;
  default:
    fprintf(Outfile, "\tcmpq\t%s, %s\n", registerList[r2], registerList[r1]);
  }

  fprintf(Outfile, "\t%s\tL%d\n", invcmplist[ASToperation - A_EQ], label);
  cgRegisterFree(r1);
  cgRegisterFree(r2);
  return (NOREG);
}

// Generate code to return a value from a function
void cgReturn(int reg, struct symTable *sym)
{

  // Only return a value if we have a value to return
  if (reg != NOREG)
  {
    // Deal with pointers here as we can't put them in
    // the switch statement
    if (typeIsPointer(sym->type))
      fprintf(Outfile, "\tmovq\t%s, %%rax\n", registerList[reg]);
    else
    {
      // Generate code depending on the function's type
      switch (sym->type)
      {
      case P_CHAR:
        fprintf(Outfile, "\tmovzbl\t%s, %%eax\n", registerByteList[reg]);
        break;
      case P_INT:
        fprintf(Outfile, "\tmovl\t%s, %%eax\n", registerDoubleList[reg]);
        break;
      case P_LONG:
        fprintf(Outfile, "\tmovq\t%s, %%rax\n", registerList[reg]);
        break;
      default:
        fatald("Bad function type in cgreturn:", sym->type);
      }
    }
  }

  cgJumpTo(sym->st_endlabel);
}
