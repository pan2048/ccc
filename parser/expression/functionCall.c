#include "../parser.h"

// Parse a function call and return its AST
struct ASTnode *funccall()
{
  struct ASTnode *tree;
  struct symtable *funcptr;

  // Check that the identifier has been defined as a function,
  // then make a leaf node for it.
  if ((funcptr = findsymbol(Text)) == NULL || funcptr->stype != S_FUNCTION)
  {
    fatals("Undeclared function", Text);
  }

  // Get the '('
  lparen();

  // Parse the argument expression list
  tree = expressionList(T_RPAREN);

  // XXX Check type of each argument against the function's prototype

  // Build the function call AST node. Store the
  // function's return type as this node's type.
  // Also record the function's symbol-id
  tree = astMakeUnary(A_FUNCCALL, funcptr->type, funcptr->ctype, tree, funcptr, 0);

  // Get the ')'
  rparen();
  return (tree);
}