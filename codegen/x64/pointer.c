#include "x64.h"

// Dereference a pointer to get the value
// it points at into the same register
int cgPointerDeref(int r, int type) {
  // Get the type that we are pointing to
  int newtype = value_at(type);
  // Now get the size of this type
  int size = cgTypeSize(newtype);

  switch (size) {
  case 1:
    fprintf(Outfile, "\tmovzbq\t(%s), %s\n", registerList[r], registerList[r]);
    break;
  case 4:
    fprintf(Outfile, "\tmovslq\t(%s), %s\n", registerList[r], registerList[r]);
    break;
  case 8:
    fprintf(Outfile, "\tmovq\t(%s), %s\n", registerList[r], registerList[r]);
    break;
  default:
    fatald("Can't cgPointerDeref on type:", type);
  }
  return (r);
}

// Store through a dereferenced pointer
int cgPointerStorDeref(int r1, int r2, int type) {
  // Get the size of the type
  int size = cgTypeSize(type);

  switch (size) {
  case 1:
    fprintf(Outfile, "\tmovb\t%s, (%s)\n", registerByteList[r1], registerList[r2]);
    break;
  case 4:
    fprintf(Outfile, "\tmovl\t%s, (%s)\n", registerDoubleList[r1], registerList[r2]);
    break;
  case 8:
    fprintf(Outfile, "\tmovq\t%s, (%s)\n", registerList[r1], registerList[r2]);
    break;
  default:
    fatald("Can't cgPointerStorDeref on type:", type);
  }
  return (r1);
}

