#include "x64.h"

// Flag to say which section outputting into
enum { no_seg, text_seg, data_seg } currSeg = no_seg;

// Switch to the text segment
void _cgDirectiveTextSeg() {
  if (currSeg != text_seg) {
    fputs("\t.text\n", Outfile);
    currSeg = text_seg;
  }
}

// Switch to the data segment
void _cgDirectiveDataSeg() {
  if (currSeg != data_seg) {
    fputs("\t.data\n", Outfile);
    currSeg = data_seg;
  }
}

// Output a gdb directive to say on which
// source code line number the following
// assembly code came from
void cgDirectiveLineNumber(int line) {
  fprintf(Outfile, "\t.loc 1 %d 0\n", line);
}
