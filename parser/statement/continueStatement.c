#include "../parser.h"

// continue_statement: 'continue' ;
// Parse a continue statement and return its AST
struct ASTnode *continueStatement()
{
  if (Looplevel == 0)
    fatal("no loop to continue to");
  scan(&Token);
  semi();
  return (astMakeLeaf(A_CONTINUE, P_NONE_TYPE, NULL, NULL, 0));
}
