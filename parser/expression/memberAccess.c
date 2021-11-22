#include "../parser.h"

// Parse the member reference of a struct or union
// and return an AST tree for it. If withpointer is true,
// the access is through a pointer to the member.
struct ASTnode *member_access(struct ASTnode *left, int withpointer)
{
  struct ASTnode *right;
  struct symTable *typeptr;
  struct symTable *m;

  // Check that the left AST tree is a pointer to struct or union
  if (withpointer && left->type != pointerTo(P_STRUCT) && left->type != pointerTo(P_UNION))
    fatal("Expression is not a pointer to a struct/union");

  // Or, check that the left AST tree is a struct or union.
  // If so, change it from an A_IDENT to an A_ADDR so that
  // we get the base address, not the value at this address.
  if (!withpointer)
  {
    if (left->type == P_STRUCT || left->type == P_UNION)
      left->op = A_ADDR;
    else
      fatal("Expression is not a struct/union");
  }

  // Get the details of the composite type
  typeptr = left->ctype;

  // Skip the '.' or '->' token and get the member's name
  scan(&Token);
  ident();

  // Find the matching member's name in the type
  // Die if we can't find it
  for (m = typeptr->member; m != NULL; m = m->next)
    if (!strCmp(m->name, Text))
      break;
  if (m == NULL)
    fatals("No member found in struct/union: ", Text);

  // Make the left tree an rvalue
  left->rvalue = 1;

  // Build an A_INTLIT node with the offset
  right = astMakeLeaf(A_INTLIT, P_INT, NULL, NULL, m->st_posn);

  // Add the member's offset to the base of the struct/union
  // and dereference it. Still an lvalue at this point
  left = astMakeNode(A_ADD, pointerTo(m->type), m->ctype, left, NULL, right, NULL, 0);
  left = astMakeUnary(A_DEREF, m->type, m->ctype, left, NULL, 0);
  return (left);
}
