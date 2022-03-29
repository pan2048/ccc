#include "x64.h"

#define FIRST_PARAM_REG 9		// Position of first parameter register

// Position of next local variable relative to stack base pointer.
// We store the offset as positive to make aligning the stack pointer easier
static int localOffset;

// Position of stack pointer offset relative to stack base pointer.
// We need this to ensure it is aligned on a 16-byte boundary.
static int stackOffset;

// Create the position of a new local variable.
int _newLocalOffset(int size)
{
  // Decrement the offset by a minimum of 4 bytes
  // and allocate on the stack
  localOffset += (size > 4) ? size : 4;
  return (-localOffset);
}

// Print out a function preamble
void cgFunctionPreamble(struct symTable *sym)
{
  // Output in the text segment, reset local offset
  _cgDirectiveTextSeg();
  localOffset = 0;

  char *name = sym->name;
  // Output the function start, save the %rbp and %rsp
  if (sym->class == C_GLOBAL)
    fprintf(Outfile, "\t.globl\t%s\n"
                     "\t.type\t%s, @function\n",
            name, name);
  fprintf(Outfile, "%s:\n"
                   "\tpushq\t%%rbp\n"
                   "\tmovq\t%%rsp, %%rbp\n",
          name);

  // Copy any in-register parameters to the stack, up to six of them
  // The remaining parameters are already on the stack
  struct symTable *parm;
  int cnt;
  int paramOffset = 16;           // Any pushed params start at this stack offset
  int paramReg = FIRST_PARAM_REG; // Index to the first param register in above reg lists
  for (parm = sym->member, cnt = 1; parm != NULL; parm = parm->next, cnt++)
  {
    if (cnt > 6)
    {
      parm->st_posn = paramOffset;
      paramOffset += 8;
    }
    else
    {
      parm->st_posn = _newLocalOffset(parm->size);
      cgStoreLocal(paramReg--, parm);
    }
  }

  // For the remainder, if they are a parameter then they are
  // already on the stack. If only a local, make a stack position.
  struct symTable *locvar;
  for (locvar = Loclhead; locvar != NULL; locvar = locvar->next)
  {
    locvar->st_posn = _newLocalOffset(locvar->size);
  }

  // Align the stack pointer to be a multiple of 16
  // less than its previous value
  stackOffset = (localOffset + 15) & ~15;
  fprintf(Outfile, "\taddq\t$%d,%%rsp\n", -stackOffset);
}

// Print out a function postamble
void cgFunctionPostamble(struct symTable *sym)
{
  cgOutputLabel(sym->st_endlabel);
  fprintf(Outfile, "\taddq\t$%d,%%rsp\n", stackOffset);
  fputs("\tpopq	%rbp\n"
        "\tret\n",
        Outfile);
  cgRegisterFreeAll(NOREG);
}

// Call a function with the given symbol id.
// Pop off any arguments pushed on the stack.
// Return the register with the result
int cgFunctionCall(struct symTable *sym, int numargs)
{
  // Call the function
  fprintf(Outfile, "\tcall\t%s@PLT\n", sym->name);

  // Remove any arguments pushed on the stack
  if (numargs > 6)
    fprintf(Outfile, "\taddq\t$%d, %%rsp\n", 8 * (numargs - 6));

  // Unspill all the registers
  cgRegisterUnSpill();

  // Get a new register and copy the return value into it
  int outr;
  outr = cgRegisterAlloc();
  fprintf(Outfile, "\tmovq\t%%rax, %s\n", registerList[outr]);
  return (outr);
}

// Given a register with an argument value,
// copy this argument into the argposn'th
// parameter in preparation for a future function call.
// Note that argposn is 1, 2, 3, 4, ..., never zero.
void cgFunctionCopyArg(int r, int argposn)
{
  // If this is above the sixth argument, simply push the
  // register on the stack. We rely on being called with
  // successive arguments in the correct order for x86-64
  if (argposn > 6)
  {
    fprintf(Outfile, "\tpushq\t%s\n", registerList[r]);
  }
  else
  {
    // Otherwise, copy the value into one of the six registers
    // used to hold parameter values
    fprintf(Outfile, "\tmovq\t%s, %s\n", registerList[r],
            registerList[FIRST_PARAM_REG - argposn + 1]);
  }
  cgRegisterFree(r);
}