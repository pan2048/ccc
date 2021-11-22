#include "../parser.h"

// Parse a postfix expression and return
// an AST node representing it. The
// identifier is already in Text.
struct ASTnode *postfix(int ptp)
{
  struct ASTnode *n;

  // Get the primary expression
  n = primaryFactor(ptp);

  // Loop until there are no more postfix operators
  while (1)
  {
    switch (Token.token)
    {
    case T_LBRACKET:
      // An array reference
      n = array_access(n);
      break;

    case T_DOT:
      // Access into a struct or union
      n = member_access(n, 0);
      break;

    case T_ARROW:
      // Pointer access into a struct or union
      n = member_access(n, 1);
      break;

    case T_INC:
      // Post-increment: skip over the token
      if (n->rvalue == 1)
        fatal("Cannot ++ on rvalue");
      scan(&Token);

      // Can't do it twice
      if (n->op == A_POSTINC || n->op == A_POSTDEC)
        fatal("Cannot ++ and/or -- more than once");

      // and change the AST operation
      n->op = A_POSTINC;
      break;

    case T_DEC:
      // Post-decrement: skip over the token
      if (n->rvalue == 1)
        fatal("Cannot -- on rvalue");
      scan(&Token);

      // Can't do it twice
      if (n->op == A_POSTINC || n->op == A_POSTDEC)
        fatal("Cannot ++ and/or -- more than once");

      // and change the AST operation
      n->op = A_POSTDEC;
      break;

    default:
      return (n);
    }
  }

  return (NULL); // Keep -Wall happy
}
