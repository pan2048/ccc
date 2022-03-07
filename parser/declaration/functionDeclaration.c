#include "../../main.h"
#include "../parser.h"


// Given a possibly NULL pointer to the function's previous declaration,
// parse a list of parameters and cross-check them against the
// previous declaration. Return the count of parameters
static int functionParamDeclarationList(struct symTable *oldfuncsym)
{
    struct symTable *protoptr = NULL;

    int paramcnt = 0;
    int type;
    struct symTable *ctype;
    struct ASTnode *unused;

    // Get the pointer to the first prototype parameter
    if (oldfuncsym != NULL)
        protoptr = oldfuncsym->member;

    // Loop getting any parameters
    while (Token.token != T_RPAREN)
    {
        // If the first token is 'void'
        if (Token.token == T_VOID)
        {
            // Peek at the next token. If a ')', the function
            // has no parameters, so leave the loop.
            scan(&Peektoken);
            if (Peektoken.token == T_RPAREN)
            {
                // Move the Peektoken into the Token
                paramcnt = 0;
                scan(&Token);
                break;
            }
        }

        // Get the type of the next parameter
        type = declarationList(&ctype, C_PARAM, T_COMMA, T_RPAREN, &unused);
        if (type == -1) 
            if (Token.token == T_DOTDOTDOT) {
                scan(&Token);
                continue;
            }
            else    
                fatal("Bad type in parameter list");

        // Ensure the type of this parameter matches the prototype
        if (protoptr != NULL)
        {
            if (type != protoptr->type)
                fatald("Type doesn't match prototype for parameter", paramcnt + 1);
            protoptr = protoptr->next;
        }
        paramcnt++;

        // Stop when we hit the right parenthesis
        if (Token.token == T_RPAREN)
            break;
        // We need a comma as separator
        comma();
    }

    if (oldfuncsym != NULL && paramcnt != oldfuncsym->nelems)
        fatals("Parameter count mismatch for function", oldfuncsym->name);

    // Return the count of parameters
    return (paramcnt);
}

//
// function_declaration: type identifier '(' parameter_list ')' ;
//      | type identifier '(' parameter_list ')' compound_statement   ;
//
// Parse the declaration of function.
struct symTable *functionDeclaration(char *funcname, int type,
                                      struct symTable *ctype,
                                      int class)
{
    struct ASTnode *tree, *finalstmt;
    struct symTable *oldfuncsym, *newfuncsym = NULL;
    int endlabel, paramcnt;
    int linenum = LineNumber;

    if (class != C_GLOBAL && class != C_STATIC)
        fatal("Function definition not at global level");

    // Text has the identifier's name. If this exists and is a
    // function, get the id. Otherwise, set oldfuncsym to NULL.
    if ((oldfuncsym = findSymbol(funcname)) != NULL)
        if (oldfuncsym->stype != S_FUNCTION) 
        {
            oldfuncsym = NULL;
            fatals("Impossible to have same name.", oldfuncsym->name);
        }

    // If this is a new function declaration, get a
    // label-id for the end label, and add the function
    // to the symbol table,
    if (oldfuncsym == NULL)
    {
        endlabel = getLabel();
        // Assumption: functions only return scalar types, so NULL below
        newfuncsym = addGlobal(funcname, type, NULL, S_FUNCTION, class, 0, endlabel);
    }

    // Scan in the '(', any parameters and the ')'.
    // Pass in any existing function prototype pointer
    lparen();
    paramcnt = functionParamDeclarationList(oldfuncsym);
    rparen();

    // If this is a new function declaration, update the
    // function symbol entry with the number of parameters.
    // Also copy the parameter list into the function's node.
    if (newfuncsym)
    {
        newfuncsym->nelems = paramcnt;
        newfuncsym->member = Parmhead;
        oldfuncsym = newfuncsym;
    }

    // Clear out the parameter list
    Parmhead = Parmtail = NULL;

    // If the declaration ends in a semicolon, only a prototype.
    if (Token.token == T_SEMI)
        return (oldfuncsym);

    // This is not just a prototype.
    // Set the Functionid global to the function's symbol pointer
    Functionid = oldfuncsym;

    // Get the AST tree for the compound statement and mark
    // that we have parsed no loops or switches yet
    Looplevel = 0;
    Switchlevel = 0;
    lbrace();
    tree = compoundStatement(0);
    rbrace();

    // If the function type isn't P_VOID ...
    if (type != P_VOID)
    {
        // Error if no statements in the function
        if (tree == NULL)
            fatal("No statements in function with non-void type");

        // Check that the last AST operation in the
        // compound statement was a return statement
        finalstmt = (tree->op == A_GLUE) ? tree->right : tree;
        if (finalstmt == NULL || finalstmt->op != A_RETURN)
            fatal("No return for function with non-void type");
    }

    // Build the A_FUNCTION node which has the function's symbol pointer
    // and the compound statement sub-tree
    tree = astMakeUnary(A_FUNCTION, type, ctype, tree, oldfuncsym, endlabel);
    tree->linenum = linenum;

    // Do optimisations on the AST tree
    tree = optimise(tree);

    // Dump the AST tree if requested
    if (O_dumpAST)
    {
        dumpAST(tree, NOLABEL, 0);
        fprintf(stdout, "\n\n");
    }

    // Generate the assembly code for it
    genAST(tree, NOLABEL, NOLABEL, NOLABEL, 0);

    // Now free the symbols associated with this function
    freeloclsyms();
    return (oldfuncsym);
}
