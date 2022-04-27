#include "x64.h"

// The assembly preamble for one output file
void cgFilePreamble(char *filename) {
  cgRegisterFreeAll(NOREG);
  _cgDirectiveTextSeg();
  fprintf(Outfile, "\t.file 1 %c%s%c\n", '"',   filename, '"');
  _cgSwitchRoutine();
}

// Nothing to do for the end of a file
void cgFilePostamble() {
}
