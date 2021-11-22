#include "../lib/lib.h"
#include "gen.h"
#include "cg.h"


// Given an AST, an optional label, and the AST op
// of the parent, generate assembly code recursively.
// Return the register id with the tree's final value.
int genAST(struct ASTnode *n, int iflabel, int looptoplabel,
           int loopendlabel, int parentASTop)
{
    int leftReg = NOREG, rightReg = NOREG;
 
    // Empty tree, do nothing
    if (n == NULL) {
        return (NOREG);
    }

   // Update the line number in the output
    genLineNumber(n->linenum);

    // We have some specific AST node handling at the top
    // so that we don't evaluate the child sub-trees immediately
    switch (n->op)
    {
    case A_IF:
        return (genIf(n, looptoplabel, loopendlabel));
    case A_WHILE:
        return (genWhile(n));
    case A_SWITCH:
        return (genSwitch(n));
    case A_FUNCCALL:
        return (genFuncCall(n));
    case A_TERNARY:
        return (genTernary(n));
    case A_LOGOR:
        return (genLogAndOr(n));
    case A_LOGAND:
        return (genLogAndOr(n));
    case A_GLUE:
        // Do each child statement, and free the
        // registers after each child
        if (n->left != NULL)
            genAST(n->left, iflabel, looptoplabel, loopendlabel, n->op);
        cgRegisterFreeAll(NOREG);
        if (n->right != NULL)
            genAST(n->right, iflabel, looptoplabel, loopendlabel, n->op);
        cgRegisterFreeAll(NOREG);
        return (NOREG);
    case A_FUNCTION:
        // Generate the function's preamble before the code
        // in the child sub-tree
        cgFunctionPreamble(n->sym);
        genAST(n->left, NOLABEL, NOLABEL, NOLABEL, n->op);
        cgFunctionPostamble(n->sym);
        return (NOREG);
    }

    // General AST node handling below

    // Get the left and right sub-tree values
    leftReg = genAST(n->left, NOLABEL, NOLABEL, NOLABEL, n->op);
    rightReg = genAST(n->right, NOLABEL, NOLABEL, NOLABEL, n->op);

    switch (n->op)
    {
    case A_ADD:
        return (cgOpAdd(leftReg, rightReg));
    case A_SUBTRACT:
        return (cgOpSub(leftReg, rightReg));
    case A_MULTIPLY:
        return (cgOpMul(leftReg, rightReg));
    case A_DIVIDE:
        return (cgOpDivMod(leftReg, rightReg, A_DIVIDE));
    case A_MOD:
        return (cgOpDivMod(leftReg, rightReg, A_MOD));
    case A_AND:
        return (cgOpAnd(leftReg, rightReg));
    case A_OR:
        return (cgOpOr(leftReg, rightReg));
    case A_XOR:
        return (cgOpXor(leftReg, rightReg));
    case A_LSHIFT:
        return (cgOpShl(leftReg, rightReg));
    case A_RSHIFT:
        return (cgOpShr(leftReg, rightReg));
    case A_EQ:
    case A_NE:
    case A_LT:
    case A_GT:
    case A_LE:
    case A_GE:
        // If the parent AST node is an A_IF, A_WHILE or A_TERNARY,
        // generate a compare followed by a jump. Otherwise, compare
        // registers and set one to 1 or 0 based on the comparison.
        if (parentASTop == A_IF || parentASTop == A_WHILE ||
            parentASTop == A_TERNARY)
            return (cgCompareAndJumpTo(n->op, leftReg, rightReg, iflabel, n->left->type));
        else
            return (cgCompareAndSet(n->op, leftReg, rightReg, n->left->type));
    case A_INTLIT:
        return (cgLoadLiteralInt(n->a_intvalue, n->type));
    case A_STRLIT:
        return (cgLoadGlobalStr(n->a_intvalue));
    case A_IDENT:
        // Load our value if we are an rvalue
        // or we are being dereferenced
        if (n->rvalue || parentASTop == A_DEREF)
        {
            return (cgLoadVariable(n->sym, n->op));
        }
        else
            return (NOREG);
    case A_ASPLUS:
    case A_ASMINUS:
    case A_ASSTAR:
    case A_ASSLASH:
    case A_ASMOD:
    case A_ASSIGN:
        // For the '+=' and friends operators, generate suitable code
        // and get the register with the result. Then take the left child,
        // make it the right child so that we can fall into the assignment code.
        switch (n->op)
        {
        case A_ASPLUS:
            leftReg = cgOpAdd(leftReg, rightReg);
            n->right = n->left;
            break;
        case A_ASMINUS:
            leftReg = cgOpSub(leftReg, rightReg);
            n->right = n->left;
            break;
        case A_ASSTAR:
            leftReg = cgOpMul(leftReg, rightReg);
            n->right = n->left;
            break;
        case A_ASSLASH:
            leftReg = cgOpDivMod(leftReg, rightReg, A_DIVIDE);
            n->right = n->left;
            break;
        case A_ASMOD:
            leftReg = cgOpDivMod(leftReg, rightReg, A_MOD);
            n->right = n->left;
            break;
        }

        // Now into the assignment code
        // Are we assigning to an identifier or through a pointer?
        switch (n->right->op)
        {
        case A_IDENT:
            if (n->right->sym->class == C_GLOBAL ||
                n->right->sym->class == C_EXTERN ||
                n->right->sym->class == C_STATIC)
                return (cgStoreGlobal(leftReg, n->right->sym));
            else
                return (cgStoreLocal(leftReg, n->right->sym));
        case A_DEREF:
            return (cgPointerStorDeref(leftReg, rightReg, n->right->type));
        default:
            fatald("Can't A_ASSIGN in genAST(), op", n->op);
        }
    case A_WIDEN:
        // Widen the child's type to the parent's type
        return (cgTypeWiden(leftReg, n->left->type, n->type));
    case A_RETURN:
        cgReturn(leftReg, Functionid);
        return (NOREG);
    case A_ADDR:
        // If we have a symbol, get its address. Otherwise,
        // the left register already has the address because
        // it's a member access
        if (n->sym != NULL)
            return (cgLoadAddress(n->sym));
        else
            return (leftReg);
    case A_DEREF:
        // If we are an rvalue, dereference to get the value we point at,
        // otherwise leave it for A_ASSIGN to store through the pointer
        if (n->rvalue)
            return (cgPointerDeref(leftReg, n->left->type));
        else
            return (leftReg);
    case A_SCALE:
        // Small optimisation: use shift if the
        // scale value is a known power of two
        switch (n->a_size)
        {
        case 2:
            return (cgOpShlConst(leftReg, 1));
        case 4:
            return (cgOpShlConst(leftReg, 2));
        case 8:
            return (cgOpShlConst(leftReg, 3));
        default:
            // Load a register with the size and
            // multiply the leftreg by this size
            rightReg = cgLoadLiteralInt(n->a_size, P_INT);
            return (cgOpMul(leftReg, rightReg));
        }
    case A_POSTINC:
    case A_POSTDEC:
        // Load and decrement the variable's value into a register
        // and post increment/decrement it
        return (cgLoadVariable(n->sym, n->op));
    case A_PREINC:
    case A_PREDEC:
        // Load and decrement the variable's value into a register
        // and pre increment/decrement it
        return (cgLoadVariable(n->left->sym, n->op));
    case A_NEGATE:
        return (cgOpNegate(leftReg));
    case A_INVERT:
        return (cgOpInvert(leftReg));
    case A_LOGNOT:
        return (cgOpLogNot(leftReg));
    case A_TOBOOL:
        // If the parent AST node is an A_IF or A_WHILE, generate
        // a compare followed by a jump. Otherwise, set the register
        // to 0 or 1 based on it's zeroeness or non-zeroeness
        return (cgJmpBoolean(leftReg, parentASTop, iflabel));
    case A_BREAK:
        cgJumpTo(loopendlabel);
        return (NOREG);
    case A_CONTINUE:
        cgJumpTo(looptoplabel);
        return (NOREG);
    case A_CAST:
        return (leftReg); // Not much to do
    default:
        fatald("Unknown AST operator", n->op);
    }
    return (NOREG); // Keep -Wall happy
}
