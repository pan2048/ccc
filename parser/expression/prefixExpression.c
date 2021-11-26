#include "../parser.h"

// prefix_expression: postfix_expression
//     | '*'  prefix_expression
//     | '&'  prefix_expression
//     | '-'  prefix_expression
//     | '++' prefix_expression
//     | '--' prefix_expression
//     ;

// Parse a prefix expression and return
// a sub-tree representing it.
struct ASTnode *prefixExpression(int previousTokenPrecedence)
{
  struct ASTnode *tree;
  switch (Token.token)
  {
  case T_AMPER:
    // Get the next token and parse it
    // recursively as a prefix expression
    scan(&Token);
    tree = prefixExpression(previousTokenPrecedence);

    // Ensure that it's an identifier
    if (tree->op != A_IDENT)
      fatal("& operator must be followed by an identifier");

    // Prevent '&' being performed on an array
    if (tree->sym->stype == S_ARRAY)
      fatal("& operator cannot be performed on an array");

    // Now change the operator to A_ADDR and the type to
    // a pointer to the original type
    tree->op = A_ADDR;
    tree->type = typePointerTo(tree->type);
    break;
  case T_STAR:
    // Get the next token and parse it
    // recursively as a prefix expression.
    // Make it an rvalue
    scan(&Token);
    tree = prefixExpression(previousTokenPrecedence);
    tree->rvalue = 1;

    // Ensure the tree's type is a pointer
    if (!typeIsPointer(tree->type))
      fatal("* operator must be followed by an expression of pointer type");

    // Prepend an A_DEREF operation to the tree
    tree =
        astMakeUnary(A_DEREF, typeValueAt(tree->type), tree->ctype, tree, NULL, 0);
    break;
  case T_MINUS:
    // Get the next token and parse it
    // recursively as a prefix expression
    scan(&Token);
    tree = prefixExpression(previousTokenPrecedence);

    // Prepend a A_NEGATE operation to the tree and
    // make the child an rvalue. Because chars are unsigned,
    // also widen this if needed to int so that it's signed
    tree->rvalue = 1;
    if (tree->type == P_CHAR)
      tree->type = P_INT;
    tree = astMakeUnary(A_NEGATE, tree->type, tree->ctype, tree, NULL, 0);
    break;
  case T_INVERT:
    // Get the next token and parse it
    // recursively as a prefix expression
    scan(&Token);
    tree = prefixExpression(previousTokenPrecedence);

    // Prepend a A_INVERT operation to the tree and
    // make the child an rvalue.
    tree->rvalue = 1;
    tree = astMakeUnary(A_INVERT, tree->type, tree->ctype, tree, NULL, 0);
    break;
  case T_LOGNOT:
    // Get the next token and parse it
    // recursively as a prefix expression
    scan(&Token);
    tree = prefixExpression(previousTokenPrecedence);

    // Prepend a A_LOGNOT operation to the tree and
    // make the child an rvalue.
    tree->rvalue = 1;
    tree = astMakeUnary(A_LOGNOT, tree->type, tree->ctype, tree, NULL, 0);
    break;
  case T_INC:
    // Get the next token and parse it
    // recursively as a prefix expression
    scan(&Token);
    tree = prefixExpression(previousTokenPrecedence);

    // For now, ensure it's an identifier
    if (tree->op != A_IDENT)
      fatal("++ operator must be followed by an identifier");

    // Prepend an A_PREINC operation to the tree
    tree = astMakeUnary(A_PREINC, tree->type, tree->ctype, tree, NULL, 0);
    break;
  case T_DEC:
    // Get the next token and parse it
    // recursively as a prefix expression
    scan(&Token);
    tree = prefixExpression(previousTokenPrecedence);

    // For now, ensure it's an identifier
    if (tree->op != A_IDENT)
      fatal("-- operator must be followed by an identifier");

    // Prepend an A_PREDEC operation to the tree
    tree = astMakeUnary(A_PREDEC, tree->type, tree->ctype, tree, NULL, 0);
    break;
  default:
    tree = postfixExpression(previousTokenPrecedence);
  }
  return (tree);
}


