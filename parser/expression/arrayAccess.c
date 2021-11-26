#include "../parser.h"

// Parse the index into an array and return an AST tree for it
struct ASTnode *arrayAccess(struct ASTnode *left)
{
  struct ASTnode *right;

  // Check that the sub-tree is a pointer
  if (!typeIsPointer(left->type))
    fatal("Not an array or pointer");

  // Get the '['
  scan(&Token);

  // Parse the following expression
  right = binaryExpression(0);

  // Get the ']'
  match(T_RBRACKET, "]");

  // Ensure that this is of int type
  if (!typeIsInt(right->type))
    fatal("Array index is not of integer type");

  // Make the left tree an rvalue
  left->rvalue = 1;

  // Scale the index by the size of the element's type
  right = typeModify(right, left->type, left->ctype, A_ADD);

  // Return an AST tree where the array's base has the offset added to it,
  // and dereference the element. Still an lvalue at this point.
  left = astMakeNode(A_ADD, left->type, left->ctype, left, NULL, right, NULL, 0);
  left = astMakeUnary(A_DEREF, typeValueAt(left->type), left->ctype, left, NULL, 0);
  return (left);
}