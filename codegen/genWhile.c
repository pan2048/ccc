#include "cg.h"
#include "gen.h"
#include "../lib/lib.h"

/*
  while (condition is true) {
    statements;
  }
===>
Lstart: evaluate condition
	jump to Lend if condition false
	statements
	jump to Lstart
Lend:
*/

// Generate the code for a WHILE statement
int genWhile(struct ASTnode *n)
{
  int Lstart, Lend;

  Lstart = getLabel();
  Lend = getLabel();

  // Output the start label
  cgOutputLabel(Lstart);

  // Generate the condition code followed
  // by a jump to the end label.
  genAST(n->left, Lend, Lstart, Lend, n->op);
  cgRegisterFreeAll(NOREG);

  // Generate the compound statement for the body
  genAST(n->right, NOLABEL, Lstart, Lend, n->op);
  cgRegisterFreeAll(NOREG);

  // Finally output the jump back to the condition,
  // and the end label
  cgJumpTo(Lstart);
  cgOutputLabel(Lend);
  
  return (NOREG);
}