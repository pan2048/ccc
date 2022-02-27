#include "cg.h"
#include "gen.h"
#include "../lib/lib.h"

// Generate the code for a SWITCH statement
int genSwitch(struct ASTnode *n) {
  // Output the code to calculate the switch condition
  int Ljumptop = getLabel();
  int reg = genAST(n->left, NOLABEL, NOLABEL, NOLABEL, 0);
  cgJumpTo(Ljumptop);
  cgRegisterFreeAll(reg);

  // Create arrays for the case values and associated labels.
  // Ensure that we have at least one position in each array.
  int *caseValue = (int *) memAlloc((n->a_intvalue + 1) * sizeof(int));
  int * caseLabel = (int *) memAlloc((n->a_intvalue + 1) * sizeof(int));

  // Generate labels for the top of the jump table, and the
  // end of the switch statement. Set a default label for
  // the end of the switch, in case we don't have a default.
  int Lend = getLabel();
  int defaultlabel = Lend;

  // Walk the right-child linked list to
  // generate the code for each case
  int i = 0;
  struct ASTnode *c;
  int caseCount = 0;
  for (i = 0, c = n->right; c != NULL; i++, c = c->right) {
    // Get a label for this case. Store it
    // and the case value in the arrays.
    // Record if it is the default case.
    caseValue[i] = c->a_intvalue;
    caseLabel[i] = getLabel();
    cgOutputLabel(caseLabel[i]);
    if (c->op == A_DEFAULT)
      defaultlabel = caseLabel[i];
    else
      caseCount++;

    // Generate the case code. Pass in the end label for the breaks.
    // If case has no body, we will fall into the following body.
    if (c->left)
      genAST(c->left, NOLABEL, NOLABEL, Lend, 0);
    cgRegisterFreeAll(NOREG);
  }

  // Ensure the last case jumps past the switch table
  cgJumpTo(Lend);

  // Now output the switch table and the end label.
  cgSwitch(reg, caseCount, Ljumptop, caseLabel, caseValue, defaultlabel);
  cgOutputLabel(Lend);

  return (NOREG);
}
