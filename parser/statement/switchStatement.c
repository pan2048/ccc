#include "../../lib/lib.h"
#include "../../lexer/lexer.h"
#include "../parser.h"

// Parse a switch statement and return its AST
struct ASTnode *switchStatement()
{
    struct ASTnode *left, *body, *n, *c;
    struct ASTnode *casetree = NULL, *casetail;
    int inloop = 1, casecount = 0;
    int seendefault = 0;
    int ASTop, casevalue;

    // Skip the 'switch' and '('
    match(T_SWITCH, "switch");
    lparen();

    // Get the switch expression, the ')' and the '{'
    left = binexpr(0);
    rparen();
    lbrace();

    // Ensure that this is of int type
    if (!inttype(left->type))
        fatal("Switch expression is not of integer type");

    // Build an A_SWITCH subtree with the expression as
    // the child
    n = astMakeUnary(A_SWITCH, P_NONE, NULL, left, NULL, 0);

    // Now parse the cases
    Switchlevel++;
    while (inloop)
    {
        switch (Token.token)
        {
            // Leave the loop when we hit a '}'
        case T_RBRACE:
            if (casecount == 0)
                fatal("No cases in switch");
            inloop = 0;
            break;
        case T_CASE:
        case T_DEFAULT:
            // Ensure this isn't after a previous 'default'
            if (seendefault)
                fatal("case or default after existing default");

            // Set the AST operation. Scan the case value if required
            if (Token.token == T_DEFAULT)
            {
                ASTop = A_DEFAULT;
                seendefault = 1;
                scan(&Token);
            }
            else
            {
                ASTop = A_CASE;
                scan(&Token);
                left = binexpr(0);

                // Ensure the case value is an integer literal
                if (left->op != A_INTLIT)
                    fatal("Expecting integer literal for case value");
                casevalue = left->a_intvalue;

                // Walk the list of existing case values to ensure
                // that there isn't a duplicate case value
                for (c = casetree; c != NULL; c = c->right)
                    if (casevalue == c->a_intvalue)
                        fatal("Duplicate case value");
            }

            // Scan the ':' and increment the casecount
            match(T_COLON, ":");
            casecount++;

            // If the next token is a T_CASE, the existing case will fall
            // into the next case. Otherwise, parse the case body.
            if (Token.token == T_CASE)
                body = NULL;
            else
                body = compoundStatement(1);

            // Build a sub-tree with any compound statement as the left child
            // and link it in to the growing A_CASE tree
            if (casetree == NULL)
            {
                casetree = casetail =
                    astMakeUnary(ASTop, P_NONE, NULL, body, NULL, casevalue);
            }
            else
            {
                casetail->right =
                    astMakeUnary(ASTop, P_NONE, NULL, body, NULL, casevalue);
                casetail = casetail->right;
            }
            break;
        default:
            fatals("Unexpected token in switch", Token.tokstr);
        }
    }
    Switchlevel--;

    // We have a sub-tree with the cases and any default. Put the
    // case count into the A_SWITCH node and attach the case tree.
    n->a_intvalue = casecount;
    n->right = casetree;
    rbrace();

    return (n);
}