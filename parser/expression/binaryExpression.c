#include "../../codegen/gen.h"
#include "../parser.h"

// Return an AST tree whose root is a binary operator.
// Parameter ptp is the previous token's precedence.
struct ASTnode *binexpr(int ptp)
{
  struct ASTnode *left, *right;
  struct ASTnode *ltemp, *rtemp;
  int ASTop;
  int tokentype;

  // Get the tree on the left.
  // Fetch the next token at the same time.
  left = prefix(ptp);

  // If we hit one of several terminating tokens, return just the left node
  tokentype = Token.token;
  if (tokentype == T_SEMI || tokentype == T_RPAREN ||
      tokentype == T_RBRACKET || tokentype == T_COMMA ||
      tokentype == T_COLON || tokentype == T_RBRACE)
  {
    left->rvalue = 1;
    return (left);
  }

  // While the precedence of this token is more than that of the
  // previous token precedence, or it's right associative and
  // equal to the previous token's precedence
  while ((op_precedence(tokentype) > ptp) ||
         (rightassoc(tokentype) && op_precedence(tokentype) == ptp))
  {
    // Fetch in the next integer literal
    scan(&Token);

    // Recursively call binexpr() with the
    // precedence of our token to build a sub-tree
    right = binexpr(op_precedence(tokentype));

    // Determine the operation to be performed on the sub-trees
    ASTop = binastop(tokentype);

    switch (ASTop)
    {
    case A_TERNARY:
      // Ensure we have a ':' token, scan in the expression after it
      match(T_COLON, ":");
      ltemp = binexpr(0);

      // Build and return the AST for this statement. Use the middle
      // expression's type as the return type. XXX We should also
      // consider the third expression's type.
      return (astMakeNode(A_TERNARY, right->type, right->ctype, left, right, ltemp, NULL, 0));

    case A_ASSIGN:
      // Assignment
      // Make the right tree into an rvalue
      right->rvalue = 1;

      // Ensure the right's type matches the left
      right = modify_type(right, left->type, left->ctype, 0);
      if (right == NULL)
        fatal("Incompatible expression in assignment");

      // Make an assignment AST tree. However, switch
      // left and right around, so that the right expression's
      // code will be generated before the left expression
      ltemp = left;
      left = right;
      right = ltemp;
      break;

    default:
      // We are not doing a ternary or assignment, so both trees should
      // be rvalues. Convert both trees into rvalue if they are lvalue trees
      left->rvalue = 1;
      right->rvalue = 1;

      // Ensure the two types are compatible by trying
      // to modify each tree to match the other's type.
      ltemp = modify_type(left, right->type, right->ctype, ASTop);
      rtemp = modify_type(right, left->type, left->ctype, ASTop);
      if (ltemp == NULL && rtemp == NULL)
      {
        fatal("Incompatible types in binary expression");
      }
      if (ltemp != NULL)
        left = ltemp;
      if (rtemp != NULL)
        right = rtemp;
    }

    // Join that sub-tree with ours. Convert the token
    // into an AST operation at the same time.
    left = astMakeNode(binastop(tokentype), left->type, left->ctype, left, NULL, right, NULL, 0);

    // Some operators produce an int result regardless of their operands
    switch (binastop(tokentype))
    {
    case A_LOGOR:
    case A_LOGAND:
    case A_EQ:
    case A_NE:
    case A_LT:
    case A_GT:
    case A_LE:
    case A_GE:
      left->type = P_INT;
    }

    // Update the details of the current token.
    // If we hit a terminating token, return just the left node
    tokentype = Token.token;
    if (tokentype == T_SEMI || tokentype == T_RPAREN ||
        tokentype == T_RBRACKET || tokentype == T_COMMA ||
        tokentype == T_COLON || tokentype == T_RBRACE)
    {
      left->rvalue = 1;
      return (left);
    }
  }

  // Return the tree we have when the precedence
  // is the same or lower
  left->rvalue = 1;
  return (left);
}