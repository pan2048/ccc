#include "../parser.h"
#include "../../codegen/gen.h"

// Parse a primary factor and return an
// AST node representing it.
struct ASTnode *primaryFactor(int ptp)
{
  struct ASTnode *n;
  struct symTable *enumptr;
  struct symTable *varptr;
  int id;
  int type = 0;
  int size, class;
  struct symTable *ctype;

  switch (Token.token)
  {
  case T_STATIC:
  case T_EXTERN:
    fatal("Compiler doesn't support static or extern local declarations");
  case T_SIZEOF:
    // Skip the T_SIZEOF and ensure we have a left parenthesis
    scan(&Token);
    if (Token.token != T_LPAREN)
      fatal("Left parenthesis expected after sizeof");
    scan(&Token);

    // Get the type inside the parentheses
    type = parseStars(parseType(&ctype, &class));

    // Get the type's size
    size = typeSize(type, ctype);
    rparen();

    // Make a leaf node int literal with the size
    return (astMakeLeaf(A_INTLIT, P_INT, NULL, NULL, size));

  case T_INTLIT:
    // For an INTLIT token, make a leaf AST node for it.
    // Make it a P_CHAR if it's within the P_CHAR range
    if (Token.intvalue >= 0 && Token.intvalue < 256)
      n = astMakeLeaf(A_INTLIT, P_CHAR, NULL, NULL, Token.intvalue);
    else
      n = astMakeLeaf(A_INTLIT, P_INT, NULL, NULL, Token.intvalue);
    break;

  case T_STRLIT:
    // For a STRLIT token, generate the assembly for it.
    id = genGlobStr(Text, 0);

    // For successive STRLIT tokens, append their contents
    // to this one
    while (1)
    {
      scan(&Peektoken);
      if (Peektoken.token != T_STRLIT)
        break;
      genGlobStr(Text, 1);
      scan(&Token); // To skip it properly
    }

    // Now make a leaf AST node for it. id is the string's label.
    genGlobStrEnd();
    n = astMakeLeaf(A_STRLIT, pointerTo(P_CHAR), NULL, NULL, id);
    break;

  case T_IDENT:
    // If the identifier matches an enum value,
    // return an A_INTLIT node
    if ((enumptr = findenumval(Text)) != NULL)
    {
      n = astMakeLeaf(A_INTLIT, P_INT, NULL, NULL, enumptr->st_posn);
      break;
    }

    // See if this identifier exists as a symbol. For arrays, set rvalue to 1.
    if ((varptr = findsymbol(Text)) == NULL)
    {
      fatals("Unknown variable or function", Text);
    }
    switch (varptr->stype)
    {
    case S_VARIABLE:
      n = astMakeLeaf(A_IDENT, varptr->type, varptr->ctype, varptr, 0);
      break;
    case S_ARRAY:
      n = astMakeLeaf(A_ADDR, varptr->type, varptr->ctype, varptr, 0);
      n->rvalue = 1;
      break;
    case S_FUNCTION:
      // Function call, see if the next token is a left parenthesis
      scan(&Token);
      if (Token.token != T_LPAREN)
        fatals("Function name used without parentheses", Text);
      return (funccall());
    default:
      fatals("Identifier not a scalar or array variable", Text);
    }
    break;

  case T_LPAREN:
    return (paren_expression(ptp));

  default:
    fatals("Expecting a primary expression, got token", Token.tokstr);
  }

  // Scan in the next token and return the leaf node
  scan(&Token);
  return (n);
}
