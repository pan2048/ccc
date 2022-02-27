#include "../parser.h"

// Parse the declaration of a variable or function.
// The type and any following '*'s have been scanned, and we
// have the identifier in the Token variable.
// The class argument is the symbol's class.
// Return a pointer to the symbol's entry in the symbol table
struct symTable *symbolDeclaration(int type, struct symTable *ctype,
                                   int class, struct ASTnode **tree)
{
  struct symTable *sym = NULL;
  char *varname = strDup(Text);

  // Ensure that we have an identifier.
  // We copied it above so we can scan more tokens in, e.g.
  // an assignment expression for a local variable.
  ident();

  // Deal with function declarations
  if (Token.token == T_LPAREN)
  {
    return (functionDeclaration(varname, type, ctype, class));
  }

  // See if this array or scalar variable has already been declared
  switch (class)
  {
  case C_EXTERN:
  case C_STATIC:
  case C_GLOBAL:
  case C_LOCAL:
  case C_PARAM:
    if (findlocl(varname) != NULL)
      fatals("Duplicate local variable declaration", varname);
  case C_MEMBER:
    if (findmember(varname) != NULL)
      fatals("Duplicate struct/union member declaration", varname);
  }

  // Add the array or scalar variable to the symbol table
  if (Token.token == T_LBRACKET)
  {
    sym = arrayDeclaration(varname, type, ctype, class);
    *tree = NULL; // Local arrays are not initialised
  }
  else
    sym = scalarDeclaration(varname, type, ctype, class, tree);
  return (sym);
}
