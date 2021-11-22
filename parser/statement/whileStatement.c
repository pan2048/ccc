#include "../../lib/lib.h"
#include "../../lexer/lexer.h"
#include "../parser.h"

// while_statement: 'while' '(' true_false_expression ')' statement  ;
//
// Parse a WHILE statement and return its AST
struct ASTnode *whileStatement()
{
    struct ASTnode *condAST, *bodyAST;

    // Ensure we have 'while' '('
    match(T_WHILE, "while");
    lparen();

    // Parse the following expression
    // and the ')' following. Force a
    // non-comparison to be boolean
    // the tree's operation is a comparison.
    condAST = binexpr(0);
    if (condAST->op < A_EQ || condAST->op > A_GE)
        condAST = astMakeUnary(A_TOBOOL, condAST->type, condAST->ctype, condAST, NULL, 0);
    rparen();

    // Get the AST for the statement.
    // Update the loop depth in the process
    Looplevel++;
    bodyAST = singleStatement();
    Looplevel--;

    // Build and return the AST for this statement
    return (astMakeNode(A_WHILE, P_NONE, NULL, condAST, NULL, bodyAST, NULL, 0));
}