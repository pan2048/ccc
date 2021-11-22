#include "../main.h"
#include "cg.h"
#include "genLib.h"
#include "../lib/lib.h"
#include "../parser/parser.h"

void genPreamble(char *filename) {
  cgFilePreamble(filename);
}

void genPostamble() {
  cgFilePostamble();
}

void genGlobSym(struct symTable *node) {
  cgGlobalSym(node);
}

// Generate a global string.
// If append is true, append to
// previous genglobstr() call.
int genGlobStr(char *strvalue, int append) {
  int l = getLabel();
  cgGlobalStr(l, strvalue, append);
  return (l);
}

void genGlobStrEnd() {
  cgGlobalStrEnd();
}

int genPrimSize(int type) {
  return (cgTypeSize(type));
}

int genAlign(int type, int offset, int direction) {
  return (cgTypeAlign(type, offset, direction));
}

void genLineNumber(int linenum) {
  // Output the line into the assembly if we've
  // changed the line number in the AST node
  if (linenum != 0 && LineNumber != linenum) {
    LineNumber = linenum;
    cgDirectiveLineNumber(LineNumber);
  }
}