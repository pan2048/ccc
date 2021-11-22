#include "../parser.h"
#include "../../codegen/gen.h"

// Given the type, name and class of an array variable, parse
// the size of the array, if any. Then parse any initialisation
// value and allocate storage for it.
// Return the variable's symbol table entry.
struct symtable *array_declaration(char *varname, int type, struct symtable *ctype, int class)
{

  struct symtable *sym; // New symbol table entry
  int nelems = -1;      // Assume the number of elements won't be given
  int maxelems;         // The maximum number of elements in the init list
  int *initlist;        // The list of initial elements
  int i = 0, j;

  // Skip past the '['
  scan(&Token);

  // See if we have an array size
  if (Token.token != T_RBRACKET)
  {
    nelems = literal(P_INT);
    if (nelems <= 0)
      fatald("Array size is illegal", nelems);
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
    sym = findglob(varname);
    if (is_new_symbol(sym, class, pointer_to(type), ctype))
      sym = addglob(varname, pointer_to(type), ctype, S_ARRAY, class, 0, 0);
    break;
  case C_LOCAL:
    sym = addlocl(varname, pointer_to(type), ctype, S_ARRAY, 0);
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
    if (nelems != -1)
      maxelems = nelems;
    else
      maxelems = TABLE_INCREMENT;
    initlist = (int *)memAlloc(maxelems * sizeof(int));

    // Loop getting a new literal value from the list
    while (1)
    {

      // Check we can add the next value, then parse and add it
      if (nelems != -1 && i == maxelems)
        fatal("Too many values in initialisation list");

      initlist[i++] = literal(type);

      // Increase the list size if the original size was
      // not set and we have hit the end of the current list
      if (nelems == -1 && i == maxelems)
      {
        maxelems += TABLE_INCREMENT;
        initlist = (int *)memRealloc((char *)initlist, maxelems * sizeof(int));
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

    if (i > nelems)
      nelems = i;
    sym->initlist = initlist;
  }

  // Set the size of the array and the number of elements
  // Only externs can have no elements.
  if (class != C_EXTERN && nelems <= 0)
    fatals("Array must have non-zero elements", sym->name);

  sym->nelems = nelems;
  sym->size = sym->nelems * typesize(type, ctype);
  // Generate any global space
  if (class == C_GLOBAL || class == C_STATIC)
    genGlobSym(sym);
  return (sym);
}
