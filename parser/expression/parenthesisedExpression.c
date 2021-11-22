#include "../parser.h"

// Parse a parenthesised expression and
// return an AST node representing it.
struct ASTnode *paren_expression(int ptp)
{
  struct ASTnode *n;
  int type = 0;
  struct symTable *ctype = NULL;

  // Beginning of a parenthesised expression, skip the '('.
  scan(&Token);

  // If the token after is a type identifier, this is a cast expression
  switch (Token.token)
  {
  case T_IDENT:
    // We have to see if the identifier matches a typedef.
    // If not, treat it as an expression.
    if (findtypedef(Text) == NULL)
    {
      n = binexpr(0); // ptp is zero as expression inside ( )
      break;
    }
  case T_VOID:
  case T_CHAR:
  case T_INT:
  case T_LONG:
  case T_STRUCT:
  case T_UNION:
  case T_ENUM:
    // Get the type inside the parentheses
    type = parseCast(&ctype);

    // Skip the closing ')' and then parse the following expression
    rparen();

  default:
    n = binexpr(ptp); // Scan in the expression. We pass in ptp
                      // as the cast doesn't change the
                      // expression's precedence
  }

  // We now have at least an expression in n, and possibly a non-zero type
  // in type if there was a cast. Skip the closing ')' if there was no cast.
  if (type == 0)
    rparen();
  else
    // Otherwise, make a unary AST node for the cast
    n = astMakeUnary(A_CAST, type, ctype, n, NULL, 0);
  return (n);
}
