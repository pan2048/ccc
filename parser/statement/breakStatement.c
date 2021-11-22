#include "../../lib/lib.h"
#include "../../lexer/lexer.h"
#include "../parser.h"

// break_statement: 'break' ;
//
// Parse a break statement and return its AST
struct ASTnode *breakStatement(void) {

  if (Looplevel == 0 && Switchlevel == 0)
    fatal("no loop or switch to break out from");
  scan(&Token);
  semi();
  return (astMakeLeaf(A_BREAK, P_NONE, NULL, NULL, 0));
}
