#include "../parser.h"

// expression_list: <null>
//        | expression
//        | expression ',' expression_list
//        ;

// Parse a list of zero or more comma-separated expressions and
// return an AST composed of A_GLUE nodes with the left-hand child
// being the sub-tree of previous expressions (or NULL) and the right-hand
// child being the next expression. Each A_GLUE node will have size field
// set to the number of expressions in the tree at this point. If no
// expressions are parsed, NULL is returned
struct ASTnode *expressionList(int endtoken)
{
  struct ASTnode *tree = NULL;
  struct ASTnode *child = NULL;
  int exprcount = 0;

  // Loop until the end token
  while (Token.token != endtoken)
  {

    // Parse the next expression and increment the expression count
    child = binexpr(0);
    exprcount++;

    // Build an A_GLUE AST node with the previous tree as the left child
    // and the new expression as the right child. Store the expression count.
    tree = astMakeNode(A_GLUE, P_NONE_TYPE, NULL, tree, NULL, child, NULL, exprcount);

    // Stop when we reach the end token
    if (Token.token == endtoken)
      break;

    // Must have a ',' at this point
    match(T_COMMA, ",");
  }

  // Return the tree of expressions
  return (tree);
}
