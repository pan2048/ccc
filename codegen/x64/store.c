#include "x64.h"

// Store a register's value into a variable
int cgStoreGlobal(int r, struct symTable *sym)
{
  if (cgTypeSize(sym->type) == 8)
  {
    fprintf(Outfile, "\tmovq\t%s, %s(%%rip)\n", registerList[r], sym->name);
  }
  else
    switch (sym->type)
    {
    case P_CHAR:
      fprintf(Outfile, "\tmovb\t%s, %s(%%rip)\n", registerByteList[r], sym->name);
      break;
    case P_INT:
      fprintf(Outfile, "\tmovl\t%s, %s(%%rip)\n", registerDoubleList[r], sym->name);
      break;
    default:
      fatald("Bad type in cgStoreGlobal:", sym->type);
    }
  return (r);
}

// Store a register's value into a local variable
int cgStoreLocal(int r, struct symTable *sym)
{
  if (cgTypeSize(sym->type) == 8)
  {
    fprintf(Outfile, "\tmovq\t%s, %d(%%rbp)\n", registerList[r], sym->st_posn);
  }
  else
    switch (sym->type)
    {
    case P_CHAR:
      fprintf(Outfile, "\tmovb\t%s, %d(%%rbp)\n", registerByteList[r], sym->st_posn);
      break;
    case P_INT:
      fprintf(Outfile, "\tmovl\t%s, %d(%%rbp)\n", registerDoubleList[r], sym->st_posn);
      break;
    default:
      fatald("Bad type in cgStoreLocal:", sym->type);
    }
  return (r);
}