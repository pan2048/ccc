#include "../parser.h"


// for_statement: 'for' '(' expression_list ';'
//                          true_false_expression ';'
//                          expression_list ')' statement  ;
// Parse a FOR statement and return its AST
struct ASTnode *forStatement() {
  struct ASTnode *conditionAST, *bodyAST;
  struct ASTnode *preOpAST, *postopAST;
  struct ASTnode *tree;

  // Ensure we have 'for' '('
  match(T_FOR, "for");
  lparen();

  // Get the pre_op expression and the ';'
  preOpAST = expressionList(T_SEMI);
  semi();

  // Get the condition and the ';'.
  // Force a non-comparison to be boolean
  // the tree's operation is a comparison.
  conditionAST = binaryExpression(0);
  if (conditionAST->op < A_EQ || conditionAST->op > A_GE)
    conditionAST = astMakeUnary(A_TOBOOL, conditionAST->type, conditionAST->ctype, conditionAST, NULL, 0);
  semi();

  // Get the post_op expression and the ')'
  postopAST = expressionList(T_RPAREN);
  rparen();

  // Get the statement which is the body
  // Update the loop depth in the process
  Looplevel++;
  bodyAST = statement();
  Looplevel--;

  // Glue the statement and the postop tree
  tree = astMakeNode(A_GLUE, P_NONE_TYPE, NULL, bodyAST, NULL, postopAST, NULL, 0);

  // Make a WHILE loop with the condition and this new body
  tree = astMakeNode(A_WHILE, P_NONE_TYPE, NULL, conditionAST, NULL, tree, NULL, 0);

  // And glue the preop tree to the A_WHILE tree
  return (astMakeNode(A_GLUE, P_NONE_TYPE, NULL, preOpAST, NULL, tree, NULL, 0));
}
