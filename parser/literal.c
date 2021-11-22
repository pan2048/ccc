#include "parser.h"

// Given a type, parse an expression of literals and ensure
// that the type of this expression matches the given type.
// Parse any type cast that precedes the expression.
// If an integer literal, return this value.
// If a string literal, return the label number of the string.
int literal(int type)
{
  struct ASTnode *tree;

  // Parse the expression and optimise the resulting AST tree
  tree = optimise(binexpr(0));

  // If there's a cast, get the child and
  // mark it as having the type from the cast
  if (tree->op == A_CAST)
  {
    tree->left->type = tree->type;
    tree = tree->left;
  }

  // The tree must now have an integer or string literal
  if (tree->op != A_INTLIT && tree->op != A_STRLIT)
    fatal("Cannot initialise globals with a general expression");

  // If the type is char * and
  if (type == pointerTo(P_CHAR))
  {
    // We have a string literal, return the label number
    if (tree->op == A_STRLIT)
      return (tree->a_intvalue);
    // We have a zero int literal, so that's a NULL
    if (tree->op == A_INTLIT && tree->a_intvalue == 0)
      return (0);
  }

  // We only get here with an integer literal. The input type
  // is an integer type and is wide enough to hold the literal value
  if (intType(type) && typeSize(type, NULL) >= typeSize(tree->type, NULL))
    return (tree->a_intvalue);

  fatal("Type mismatch: literal vs. variable");
  return (0); // Keep -Wall happy
}
