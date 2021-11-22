#include "../parser.h"

// if_statement: if_head
//      |        if_head 'else' statement
//      ;
//
// if_head: 'if' '(' true_false_expression ')' statement  ;
//
// Parse an IF statement including any
// optional ELSE clause and return its AST
struct ASTnode *ifStatement()
{
  struct ASTnode *condAST, *trueAST, *falseAST = NULL;

  // Ensure we have 'if' '('
  match(T_IF, "if");
  lparen();

  // Parse the following expression
  // and the ')' following. Force a
  // non-comparison to be boolean
  // the tree's operation is a comparison.
  condAST = binexpr(0);
  if (condAST->op < A_EQ || condAST->op > A_GE)
    condAST = astMakeUnary(A_TOBOOL, condAST->type, condAST->ctype, condAST, NULL, 0);
  rparen();

  // Get the AST for the statement
  trueAST = singleStatement();

  // If we have an 'else', skip it
  // and get the AST for the statement
  if (Token.token == T_ELSE)
  {
    scan(&Token);
    falseAST = singleStatement();
  }

  // Build and return the AST for this statement
  return (astMakeNode(A_IF, P_NONE_TYPE, NULL, condAST, trueAST, falseAST, NULL, 0));
}
