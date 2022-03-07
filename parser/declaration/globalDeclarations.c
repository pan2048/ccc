#include "../parser.h"

// Parse one or more global declarations,
// either variables, functions or structs
void globalDeclarations()
{
    // parse declaration lists until the end of file
    while (Token.token != T_EOF)
    {
        // Skip any separating semicolons
        while (Token.token == T_SEMI)
            scan(&Token);

        struct symTable *unusedCtype = NULL;
        struct ASTnode *unusedGluetree = NULL;
        declarationList(&unusedCtype, C_GLOBAL, T_SEMI, T_EOF, &unusedGluetree);
    }
}