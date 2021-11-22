#include "../../lib/lib.h"
#include "../../lexer/lexer.h"
#include "../parser.h"


// for_statement: 'for' '(' expression_list ';'
//                          true_false_expression ';'
//                          expression_list ')' statement  ;
//
// Parse a FOR statement and return its AST
struct ASTnode *forStatement() {
  struct ASTnode *condAST, *bodyAST;
  struct ASTnode *preopAST, *postopAST;
  struct ASTnode *tree;

  // Ensure we have 'for' '('
  match(T_FOR, "for");
  lparen();

  // Get the pre_op expression and the ';'
  preopAST = expression_list(T_SEMI);
  semi();

  // Get the condition and the ';'.
  // Force a non-comparison to be boolean
  // the tree's operation is a comparison.
  condAST = binexpr(0);
  if (condAST->op < A_EQ || condAST->op > A_GE)
    condAST =
      astMakeUnary(A_TOBOOL, condAST->type, condAST->ctype, condAST, NULL, 0);
  semi();

  // Get the post_op expression and the ')'
  postopAST = expression_list(T_RPAREN);
  rparen();

  // Get the statement which is the body
  // Update the loop depth in the process
  Looplevel++;
  bodyAST = singleStatement();
  Looplevel--;

  // Glue the statement and the postop tree
  tree = astMakeNode(A_GLUE, P_NONE, NULL, bodyAST, NULL, postopAST, NULL, 0);

  // Make a WHILE loop with the condition and this new body
  tree = astMakeNode(A_WHILE, P_NONE, NULL, condAST, NULL, tree, NULL, 0);

  // And glue the preop tree to the A_WHILE tree
  return (astMakeNode(A_GLUE, P_NONE, NULL, preopAST, NULL, tree, NULL, 0));
}
