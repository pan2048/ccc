#include "../parser.h"

// Parse a compound statement
// and return its AST. If inswitch is true,
// we look for a '}', 'case' or 'default' token
// to end the parsing. Otherwise, look for
// just a '}' to end the parsing.
struct ASTnode *compoundStatement(int inswitch)
{
  struct ASTnode *left = NULL;
  struct ASTnode *tree;

  while (1)
  {
    // Leave if we've hit the end token. We do this first to allow
    // an empty compound statement
    if (Token.token == T_RBRACE)
      return (left);
    if (inswitch && (Token.token == T_CASE || Token.token == T_DEFAULT))
      return (left);

    // Parse a single statement
    tree = statement();

    // For each new tree, either save it in left
    // if left is empty, or glue the left and the
    // new tree together
    if (tree != NULL)
    {
      if (left == NULL)
        left = tree;
      else
        left = astMakeNode(A_GLUE, P_NONE_TYPE, NULL, left, NULL, tree, NULL, 0);
    }
  }
  return (NULL); // Keep -Wall happy
}
