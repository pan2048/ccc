#include "../parser.h"

// Parse a list of symbols where there is an initial type.
// Return the type of the symbols. et1 and et2 are end tokens.
int declarationList(struct symtable **ctype, int class, int et1, int et2,
                     struct ASTnode **gluetree)
{
    int inittype, type;
    struct symtable *sym;
    struct ASTnode *tree;
    *gluetree = NULL;

    // Get the initial type. If -1, it was
    // a composite type definition, return this
    if ((inittype = parse_type(ctype, &class)) == -1)
        return (inittype);

    // Now parse the list of symbols
    while (1)
    {
        // See if this symbol is a pointer
        type = parse_stars(inittype);

        // Parse this symbol
        sym = symbol_declaration(type, *ctype, class, &tree);

        // We parsed a function, there is no list so leave
        if (sym->stype == S_FUNCTION)
        {
            if (class != C_GLOBAL && class != C_STATIC)
                fatal("Function definition not at global level");
            return (type);
        }

        // Glue any AST tree from a local declaration
        // to build a sequence of assignments to perform
        if (*gluetree == NULL)
            *gluetree = tree;
        else
            *gluetree =
                astMakeNode(A_GLUE, P_NONE_TYPE, NULL, *gluetree, NULL, tree, NULL, 0);

        // We are at the end of the list, leave
        if (Token.token == et1 || Token.token == et2)
            return (type);

        // Otherwise, we need a comma as separator
        comma();
    }

    return (0); // Keep -Wall happy
}
