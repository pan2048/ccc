#include "cg.h"
#include "gen.h"
#include "../lib/lib.h"

// Generate the code for an
// A_LOGAND or A_LOGOR operation
int genLogAndOr(struct ASTnode *n) {
  // Generate two labels
  int Lfalse = getLabel();
  int Lend = getLabel();
  int reg;

  // Generate the code for the left expression
  // followed by the jump to the false label
  reg = genAST(n->left, NOLABEL, NOLABEL, NOLABEL, 0);
  cgJmpBoolean(reg, n->op, Lfalse);
  cgRegisterFreeAll(NOREG);

  // Generate the code for the right expression
  // followed by the jump to the false label
  reg = genAST(n->right, NOLABEL, NOLABEL, NOLABEL, 0);
  cgJmpBoolean(reg, n->op, Lfalse);
  cgRegisterFreeAll(reg);

  // We didn't jump so set the right boolean value
  if (n->op == A_LOGAND) {
    cgLoadBoolean(reg, 1);
    cgJumpTo(Lend);
    cgOutputLabel(Lfalse);
    cgLoadBoolean(reg, 0);
  } else {
    cgLoadBoolean(reg, 0);
    cgJumpTo(Lend);
    cgOutputLabel(Lfalse);
    cgLoadBoolean(reg, 1);
  }
  cgOutputLabel(Lend);
  return (reg);
}
