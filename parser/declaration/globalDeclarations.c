#include "../parser.h"

// Parse one or more global declarations,
// either variables, functions or structs
void globalDeclarations()
{
    struct symtable *ctype = NULL;
    struct ASTnode *unused;

    // Loop parsing one declaration list until the end of file
    while (Token.token != T_EOF)
    {
        declarationList(&ctype, C_GLOBAL, T_SEMI, T_EOF, &unused);

        // Skip any separating semicolons
        if (Token.token == T_SEMI)
            scan(&Token);
    }
}