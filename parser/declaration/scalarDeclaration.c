#include "../parser.h"
#include "../../codegen/gen.h"

// Given the type, name and class of a scalar variable,
// parse any initialisation value and allocate storage for it.
// Return the variable's symbol table entry.
struct symtable *scalarDeclaration(char *varname, int type,
                                    struct symtable *ctype,
                                    int class, struct ASTnode **tree)
{
    struct symtable *sym = NULL;
    struct ASTnode *varnode, *exprnode;
    *tree = NULL;

    // Add this as a known scalar
    switch (class)
    {
    case C_STATIC:
    case C_EXTERN:
    case C_GLOBAL:
        // See if this variable is new or already exists
        sym = findglob(varname);
        if (is_new_symbol(sym, class, type, ctype))
            sym = addglob(varname, type, ctype, S_VARIABLE, class, 1, 0);
        break;
    case C_LOCAL:
        sym = addlocl(varname, type, ctype, S_VARIABLE, 1);
        break;
    case C_PARAM:
        sym = addparm(varname, type, ctype, S_VARIABLE);
        break;
    case C_MEMBER:
        sym = addmemb(varname, type, ctype, S_VARIABLE, 1);
        break;
    }

    // The variable is being initialised
    if (Token.token == T_ASSIGN)
    {
        // Only possible for a global or local
        if (class != C_GLOBAL && class != C_LOCAL && class != C_STATIC)
            fatals("Variable can not be initialised", varname);
        scan(&Token);

        // Globals must be assigned a literal value
        if (class == C_GLOBAL || class == C_STATIC)
        {
            // Create one initial value for the variable and
            // parse this value
            sym->initlist = (int *)memAlloc(sizeof(int));
            sym->initlist[0] = literal(type);
        }
        if (class == C_LOCAL)
        {
            // Make an A_IDENT AST node with the variable
            varnode = astMakeLeaf(A_IDENT, sym->type, sym->ctype, sym, 0);

            // Get the expression for the assignment, make into a rvalue
            exprnode = binexpr(0);
            exprnode->rvalue = 1;

            // Ensure the expression's type matches the variable
            exprnode = modify_type(exprnode, varnode->type, varnode->ctype, 0);
            if (exprnode == NULL)
                fatal("Incompatible expression in assignment");

            // Make an assignment AST tree
            *tree = astMakeNode(A_ASSIGN, exprnode->type, exprnode->ctype, exprnode,
                                NULL, varnode, NULL, 0);
        }
    }

    // Generate any global space
    if (class == C_GLOBAL || class == C_STATIC)
        genGlobSym(sym);

    return (sym);
}
