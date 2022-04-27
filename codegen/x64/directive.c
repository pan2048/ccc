#include "x64.h"

enum Seg
{
  no_seg,
  text_seg,
  data_seg
};
enum Seg currSeg = no_seg;

// Switch to text segment
void _cgDirectiveTextSeg()
{
  if (currSeg != text_seg)
  {
    fputs("\t.text\n", Outfile);
    currSeg = text_seg;
  }
}

// Switch to data segment
void _cgDirectiveDataSeg()
{
  if (currSeg != data_seg)
  {
    fputs("\t.data\n", Outfile);
    currSeg = data_seg;
  }
}

// Gdb directive
// source code line number on which
// the following assembly code came from
void cgDirectiveLineNumber(int line)
{
  fprintf(Outfile, "\t.loc 1 %d 0\n", line);
}
