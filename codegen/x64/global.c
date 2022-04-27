#include "x64.h"

// Generate a global symbol but not functions
void cgGlobalSym(struct symTable *node)
{
  if (node == NULL)
    return;
  if (node->stype == S_FUNCTION)
    return;

  // Get the size of the variable (or its elements if an array)
  // and the type of the variable
  int type, size;
  if (node->stype == S_ARRAY)
  {
    type = typeValueAt(node->type);
    size = typeSize(type, node->ctype);
  }
  else
  {
    type = node->type;
    size = node->size;
  }

  // Generate the global identity and the label
  _cgDirectiveDataSeg();
  if (node->class == C_GLOBAL)
    fprintf(Outfile, "\t.globl\t%s\n", node->name);
  fprintf(Outfile, "%s:\n", node->name);

  // Output space for one or more elements
  int i;
  for (i = 0; i < node->nelems; i++)
  {
    // Get any initial value
    int initvalue = 0;
    if (node->initlist != NULL)
      initvalue = node->initlist[i];

    // Generate the space for this type
    switch (size)
    {
    case 1:
      fprintf(Outfile, "\t.byte\t%d\n", initvalue);
      break;
    case 4:
      fprintf(Outfile, "\t.long\t%d\n", initvalue);
      break;
    case 8:
      // Generate the pointer to a string literal. Treat a zero value
      // as actually zero, not the label L0
      if (node->initlist != NULL && type == typePointerTo(P_CHAR) && initvalue != 0)
        fprintf(Outfile, "\t.quad\tL%d\n", initvalue);
      else
        fprintf(Outfile, "\t.quad\t%d\n", initvalue);
      break;
    default:
      for (i = 0; i < size; i++)
        fprintf(Outfile, "\t.byte\t0\n");
    }
  }
}

// Generate a global string and its start label.
// Don't output the label if append is true.
void cgGlobalStr(int lable, char *strvalue, int append)
{
  if (!append)
    cgOutputLabel(lable);
  char *cptr;
  for (cptr = strvalue; *cptr; cptr++)
  {
    fprintf(Outfile, "\t.byte\t%d\n", *cptr);
  }
}

// NUL terminate a global string
void cgGlobalStrEnd(void)
{
  fprintf(Outfile, "\t.byte\t0\n");
}
