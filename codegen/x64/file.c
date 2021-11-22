#include "x64.h"

// Print out the assembly preamble
// for one output file
void cgFilePreamble(char *filename) {
  cgRegisterFreeAll(NOREG);
  _cgDirectiveTextSeg();
  fprintf(Outfile, "\t.file 1 ");
  fputc('"', Outfile);
  fprintf(Outfile, "%s", filename);
  fputc('"', Outfile);
  fputc('\n', Outfile);
  _cgSwitchRoutine();
}

// Nothing to do for the end of a file
void cgFilePostamble() {
}
