#include "../../lib/lib.h"
#include "../../lexer/lexer.h"
#include "../parser.h"

// return_statement: 'return' '(' expression ')'  ;
//
// Parse a return statement and return its AST
struct ASTnode *returnStatement() {
  struct ASTnode *tree= NULL;

  // Ensure we have 'return'
  match(T_RETURN, "return");

  // See if we have a return value
  if (Token.token == T_LPAREN) {
    // Can't return a value if function returns P_VOID
    if (Functionid->type == P_VOID)
      fatal("Can't return from a void function");

    // Skip the left parenthesis
    lparen();

    // Parse the following expression
    tree = binaryExpression(0);

    // Ensure this is compatible with the function's type
    tree = typeModify(tree, Functionid->type, Functionid->ctype, 0);
    if (tree == NULL)
      fatal("Incompatible type to return");

    // Get the ')'
    rparen();
  }

  // Add on the A_RETURN node
  tree = astMakeUnary(A_RETURN, P_NONE_TYPE, NULL, tree, NULL, 0);

  // Get the ';'
  semi();
  return (tree);
}





