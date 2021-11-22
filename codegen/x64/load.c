#include "x64.h"

// Load an integer literal value into a register.
// Return the number of the register.
// For x86-64, we don't need to worry about the type.
int cgLoadLiteralInt(int value, int type)
{
  // Get a new register
  int r = cgRegisterAlloc();

  fprintf(Outfile, "\tmovq\t$%d, %s\n", value, registerList[r]);
  return (r);
}

// Load a value from a variable into a register.
// Return the number of the register. If the
// operation is pre- or post-increment/decrement,
// also perform this action.
int cgLoadVariable(struct symTable *sym, int op)
{
  int r, postreg, offset = 1;

  // Get a new register
  r = cgRegisterAlloc();

  // If the symbol is a pointer, use the size
  // of the type that it points to as any
  // increment or decrement. If not, it's one.
  if (typeIsPointer(sym->type))
    offset = typeSize(valueAt(sym->type), sym->ctype);

  // Negate the offset for decrements
  if (op == A_PREDEC || op == A_POSTDEC)
    offset = -offset;

  // If we have a pre-operation
  if (op == A_PREINC || op == A_PREDEC)
  {
    // Load the symbol's address
    if (sym->class == C_LOCAL || sym->class == C_PARAM)
      fprintf(Outfile, "\tleaq\t%d(%%rbp), %s\n", sym->st_posn, registerList[r]);
    else
      fprintf(Outfile, "\tleaq\t%s(%%rip), %s\n", sym->name, registerList[r]);

    // and change the value at that address
    switch (sym->size)
    {
    case 1:
      fprintf(Outfile, "\taddb\t$%d,(%s)\n", offset, registerList[r]);
      break;
    case 4:
      fprintf(Outfile, "\taddl\t$%d,(%s)\n", offset, registerList[r]);
      break;
    case 8:
      fprintf(Outfile, "\taddq\t$%d,(%s)\n", offset, registerList[r]);
      break;
    }
  }

  // Now load the output register with the value
  if (sym->class == C_LOCAL || sym->class == C_PARAM)
  {
    switch (sym->size)
    {
    case 1:
      fprintf(Outfile, "\tmovzbq\t%d(%%rbp), %s\n", sym->st_posn, registerList[r]);
      break;
    case 4:
      fprintf(Outfile, "\tmovslq\t%d(%%rbp), %s\n", sym->st_posn, registerList[r]);
      break;
    case 8:
      fprintf(Outfile, "\tmovq\t%d(%%rbp), %s\n", sym->st_posn, registerList[r]);
    }
  }
  else
  {
    switch (sym->size)
    {
    case 1:
      fprintf(Outfile, "\tmovzbq\t%s(%%rip), %s\n", sym->name, registerList[r]);
      break;
    case 4:
      fprintf(Outfile, "\tmovslq\t%s(%%rip), %s\n", sym->name, registerList[r]);
      break;
    case 8:
      fprintf(Outfile, "\tmovq\t%s(%%rip), %s\n", sym->name, registerList[r]);
    }
  }

  // If we have a post-operation, get a new register
  if (op == A_POSTINC || op == A_POSTDEC)
  {
    postreg = cgRegisterAlloc();

    // Load the symbol's address
    if (sym->class == C_LOCAL || sym->class == C_PARAM)
      fprintf(Outfile, "\tleaq\t%d(%%rbp), %s\n", sym->st_posn,
              registerList[postreg]);
    else
      fprintf(Outfile, "\tleaq\t%s(%%rip), %s\n", sym->name,
              registerList[postreg]);

    // and change the value at that address
    switch (sym->size)
    {
    case 1:
      fprintf(Outfile, "\taddb\t$%d,(%s)\n", offset, registerList[postreg]);
      break;
    case 4:
      fprintf(Outfile, "\taddl\t$%d,(%s)\n", offset, registerList[postreg]);
      break;
    case 8:
      fprintf(Outfile, "\taddq\t$%d,(%s)\n", offset, registerList[postreg]);
      break;
    }

    // Finally, free the register
    cgRegisterFree(postreg);
  }

  // Return the register with the value
  return (r);
}

// Given the label number of a global string,
// load its address into a new register
int cgLoadGlobalStr(int label)
{
  // Get a new register
  int r = cgRegisterAlloc();
  fprintf(Outfile, "\tleaq\tL%d(%%rip), %s\n", label, registerList[r]);
  return (r);
}

// Load a boolean value (only 0 or 1)
// into the given register
void cgLoadBoolean(int r, int val)
{
  fprintf(Outfile, "\tmovq\t$%d, %s\n", val, registerList[r]);
}

// Generate code to load the address of an
// identifier into a variable. Return a new register
int cgLoadAddress(struct symTable *sym)
{
  int r = cgRegisterAlloc();

  if (sym->class == C_GLOBAL ||
      sym->class == C_EXTERN || sym->class == C_STATIC)
    fprintf(Outfile, "\tleaq\t%s(%%rip), %s\n", sym->name, registerList[r]);
  else
    fprintf(Outfile, "\tleaq\t%d(%%rbp), %s\n", sym->st_posn, registerList[r]);
  return (r);
}
