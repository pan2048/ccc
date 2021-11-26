#include "../parser.h"
#include "../../codegen/gen.h"

// Given the type, name and class of an array variable, parse
// the size of the array, if any. Then parse any initialisation
// value and allocate storage for it.
// Return the variable's symbol table entry.
struct symTable *arrayDeclaration(char *varname, int type, struct symTable *ctype, int class)
{
  int numElems = -1;      // Assume the number of elements won't be given
  int maxElems;         // The maximum number of elements in the init list
  struct symTable *sym; // New symbol table entry
  int *initlist;        // The list of initial elements
  int i = 0, j;

  // Skip past the '['
  scan(&Token);

  // See if we have an array size
  if (Token.token != T_RBRACKET)
  {
    numElems = literal(P_INT);
    if (numElems <= 0)
      fatald("Array size is illegal", numElems);
  }

  // Ensure we have a following ']'
  match(T_RBRACKET, "]");

  // Add this as a known array. We treat the
  // array as a pointer to its elements' type
  switch (class)
  {
  case C_STATIC:
  case C_EXTERN:
  case C_GLOBAL:
    // See if this variable is new or already exists
    sym = findGlobal(varname);
    if (isNewSymbol(sym, class, typePointerTo(type), ctype))
      sym = addGlobal(varname, typePointerTo(type), ctype, S_ARRAY, class, 0, 0);
    break;
  case C_LOCAL:
    sym = addLocal(varname, typePointerTo(type), ctype, S_ARRAY, 0);
    break;
  default:
    fatal("Declaration of array parameters is not implemented");
  }

  // Array initialisation
  if (Token.token == T_ASSIGN)
  {
    if (class != C_GLOBAL && class != C_STATIC)
      fatals("Variable can not be initialised", varname);
    scan(&Token);

    // Get the following left curly bracket
    match(T_LBRACE, "{");

#define TABLE_INCREMENT 10
    // If the array already has nelems, allocate that many elements
    // in the list. Otherwise, start with TABLE_INCREMENT.
    if (numElems != -1)
      maxElems = numElems;
    else
      maxElems = TABLE_INCREMENT;
    initlist = (int *)memAlloc(maxElems * sizeof(int));

    // Loop getting a new literal value from the list
    while (1)
    {

      // Check we can add the next value, then parse and add it
      if (numElems != -1 && i == maxElems)
        fatal("Too many values in initialisation list");

      initlist[i++] = literal(type);

      // Increase the list size if the original size was
      // not set and we have hit the end of the current list
      if (numElems == -1 && i == maxElems)
      {
        maxElems += TABLE_INCREMENT;
        initlist = (int *)memRealloc((char *)initlist, maxElems * sizeof(int));
      }

      // Leave when we hit the right curly bracket
      if (Token.token == T_RBRACE)
      {
        scan(&Token);
        break;
      }

      // Next token must be a comma, then
      comma();
    }

    // Zero any unused elements in the initlist.
    // Attach the list to the symbol table entry
    for (j = i; j < sym->nelems; j++)
      initlist[j] = 0;

    if (i > numElems)
      numElems = i;
    sym->initlist = initlist;
  }

  // Set the size of the array and the number of elements
  // Only externs can have no elements.
  if (class != C_EXTERN && numElems <= 0)
    fatals("Array must have non-zero elements", sym->name);

  sym->nelems = numElems;
  sym->size = sym->nelems * typeSize(type, ctype);
  // Generate any global space
  if (class == C_GLOBAL || class == C_STATIC)
    genGlobSym(sym);
  return (sym);
}
