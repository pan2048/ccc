#include "../parser.h"

// Parse a list of symbols where there is an initial type.
// Return the type of the symbols. et1 and et2 are end tokens.
int declarationList(struct symTable **ctype, int class, int et1, int et2, struct ASTnode **gluetree)
{
    // Get the initial type. If -1, it was
    // a composite type definition, return this
    int inittype;
    if ((inittype = parseType(ctype, &class)) == -1)
        return (inittype);

    // Now parse the list of symbols
    *gluetree = NULL;
    while (1)
    {
        int type;
        // See if this symbol is a pointer
        type = parseStars(inittype);

        struct symTable *sym;
        struct ASTnode *tree;
        // Parse this symbol
        sym = symbolDeclaration(type, *ctype, class, &tree);

        // We parsed a function, there is no list so leave
        if (sym->stype == S_FUNCTION)
        {
            return (type);
        }

        // Glue any AST tree from a local declaration
        // to build a sequence of assignments to perform
        if (*gluetree == NULL)
            *gluetree = tree;
        else
            *gluetree = astMakeNode(A_GLUE, P_NONE_TYPE, NULL, *gluetree, NULL, tree, NULL, 0);

        // We are at the end of the list, leave
        if (Token.token == et1 || Token.token == et2)
            return (type);
        else // Otherwise, we need a comma as separator
            comma();
    }

    return (0); // Keep -Wall happy
}
