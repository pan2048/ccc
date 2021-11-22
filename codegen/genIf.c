#include "cg.h"
#include "gen.h"
#include "../lib/lib.h"
#include "../parser/parser.h"

/*
  if (condition is true) 
    first block
  else
    other block
  ===> 
       opposite comparison
       jump to L1 if true
       first block
       jump to L2
L1:
       other block
L2:
*/

int genIf(struct ASTnode *n, int looptoplabel, int loopendlabel)
{
  int Lfalse, Lend;

  // Generate two labels:
  // one for the false compound statement,
  // one for the end of the overall IF statement.
  // When there is no ELSE clause, Lfalse _is_ the ending label!
  Lfalse = getLabel();
  if (n->right)
    Lend = getLabel();

  // Generate the condition code followed
  // by a jump to the false label.
  genAST(n->left, Lfalse, NOLABEL, NOLABEL, n->op);

  // Generate the true compound statement
  genAST(n->mid, NOLABEL, looptoplabel, loopendlabel, n->op);
  cgRegisterFreeAll(NOREG);

  // If there is an optional ELSE clause,
  // generate the jump to skip to the end
  if (n->right)
    cgJumpTo(Lend);

  // Now the false label
  cgOutputLabel(Lfalse);

  // Optional ELSE clause:
  // generate the false compound statement and the end label
  if (n->right)
  {
    genAST(n->right, NOLABEL, NOLABEL, loopendlabel, n->op);
    cgRegisterFreeAll(NOREG);
    cgOutputLabel(Lend);
  }

  return (NOREG);
}