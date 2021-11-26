#include "../parser.h"

// Return an AST tree whose root is a binary operator.
// Parameter previousTokenPrecedence is the previous token's precedence.
struct ASTnode *binaryExpression(int previousTokenPrecedence)
{
  struct ASTnode *left;
  int tokenType;
  struct ASTnode *right;
  int ASTOperation;
  struct ASTnode *lTemp, *rTemp;

  // Get the tree on the left.
  // Fetch the next token at the same time.
  left = prefixExpression(previousTokenPrecedence);

  // If we hit one of several terminating tokens, return just the left node
  tokenType = Token.token;
  if (tokenType == T_SEMI || tokenType == T_RPAREN ||
      tokenType == T_RBRACKET || tokenType == T_COMMA ||
      tokenType == T_COLON || tokenType == T_RBRACE)
  {
    left->rvalue = 1;
    return (left);
  }

  // While the precedence of this token is more than that of the
  // previous token precedence, or it's right associative and
  // equal to the previous token's precedence
  while ((tokenOperatorPrecedence(tokenType) > previousTokenPrecedence) ||
         (tokenRightAssociate(tokenType) && tokenOperatorPrecedence(tokenType) == previousTokenPrecedence))
  {
    // Fetch in the next integer literal
    scan(&Token);

    // Recursively call binaryExpression() with the
    // precedence of our token to build a sub-tree
    right = binaryExpression(tokenOperatorPrecedence(tokenType));

    // Determine the operation to be performed on the sub-trees
    ASTOperation = tokenBinaryAstOperation(tokenType);

    switch (ASTOperation)
    {
    case A_TERNARY:
      // Ensure we have a ':' token, scan in the expression after it
      match(T_COLON, ":");
      lTemp = binaryExpression(0);

      // Build and return the AST for this statement. Use the middle
      // expression's type as the return type. XXX We should also
      // consider the third expression's type.
      return (astMakeNode(A_TERNARY, right->type, right->ctype, left, right, lTemp, NULL, 0));

    case A_ASSIGN:
      // Assignment
      // Make the right tree into an rvalue
      right->rvalue = 1;

      // Ensure the right's type matches the left
      right = typeModify(right, left->type, left->ctype, 0);
      if (right == NULL)
        fatal("Incompatible expression in assignment");

      // Make an assignment AST tree. However, switch
      // left and right around, so that the right expression's
      // code will be generated before the left expression
      lTemp = left;
      left = right;
      right = lTemp;
      break;

    default:
      // We are not doing a ternary or assignment, so both trees should
      // be rvalues. Convert both trees into rvalue if they are lvalue trees
      left->rvalue = 1;
      right->rvalue = 1;

      // Ensure the two types are compatible by trying
      // to modify each tree to match the other's type.
      lTemp = typeModify(left, right->type, right->ctype, ASTOperation);
      rTemp = typeModify(right, left->type, left->ctype, ASTOperation);
      if (lTemp == NULL && rTemp == NULL)
      {
        fatal("Incompatible types in binary expression");
      }
      if (lTemp != NULL)
        left = lTemp;
      if (rTemp != NULL)
        right = rTemp;
    }

    // Join that sub-tree with ours. Convert the token
    // into an AST operation at the same time.
    left = astMakeNode(tokenBinaryAstOperation(tokenType), left->type, left->ctype, left, NULL, right, NULL, 0);

    // Some operators produce an int result regardless of their operands
    switch (tokenBinaryAstOperation(tokenType))
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
    tokenType = Token.token;
    if (tokenType == T_SEMI || tokenType == T_RPAREN ||
        tokenType == T_RBRACKET || tokenType == T_COMMA ||
        tokenType == T_COLON || tokenType == T_RBRACE)
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