#include "x64.h"

// Given a scalar type, return the
// size of the type in bytes.
int cgTypeSize(int type)
{
  if (typeIsPointer(type))
    return (8);
  switch (type)
  {
  case P_CHAR:
    return (1);
  case P_INT:
    return (4);
  case P_LONG:
    return (8);
  default:
    fatald("Bad type in cgTypeSize:", type);
  }
  return (0); // Keep -Wall happy
}

// Given a scalar type, an existing memory offset
// (which hasn't been allocated to anything yet)
// and a direction (1 is up, -1 is down), calculate
// and return a suitably aligned memory offset
// for this scalar type. This could be the original
// offset, or it could be above/below the original
int cgTypeAlign(int type, int offset, int direction)
{
  int alignment;

  // We don't need to do this on x86-64, but let's
  // align chars on any offset and align ints/pointers
  // on a 4-byte alignment
  switch (type)
  {
  case P_CHAR:
    break;
  default:
    // Align whatever we have now on a 4-byte alignment.
    // I put the generic code here so it can be reused elsewhere.
    alignment = 4;
    offset = (offset + direction * (alignment - 1)) & ~(alignment - 1);
  }
  return (offset);
}

// Widen the value in the register from the old
// to the new type, and return a register with
// this new value
int cgTypeWiden(int r, int oldtype, int newtype)
{
  // Nothing to do
  return (r);
}