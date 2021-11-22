#include "cg.h"
#include "gen.h"
#include "../lib/lib.h"

// Generate code for a ternary expression
int genTernary(struct ASTnode *node) {
  int Lfalse, Lend;
  int reg, expreg;

  // Generate two labels: one for the
  // false expression, and one for the
  // end of the overall expression
  Lfalse = getLabel();
  Lend = getLabel();

  // Generate the condition code followed
  // by a jump to the false label.
  genAST(node->left, Lfalse, NOLABEL, NOLABEL, node->op);
  //genfreeregs(NOREG);

  // Get a register to hold the result of the two expressions
  reg = cgRegisterAlloc();

  // Generate the true expression and the false label.
  // Move the expression result into the known register.
  expreg = genAST(node->mid, NOLABEL, NOLABEL, NOLABEL, node->op);
  cgRegisterMove(expreg, reg);
  cgRegisterFree(expreg);
  cgJumpTo(Lend);
  cgOutputLabel(Lfalse);

  // Generate the false expression and the end label.
  // Move the expression result into the known register.
  expreg = genAST(node->right, NOLABEL, NOLABEL, NOLABEL, node->op);
  cgRegisterMove(expreg, reg);
  cgRegisterFree(expreg);
  cgOutputLabel(Lend);
  return (reg);
}