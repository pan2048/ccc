#include "x64.h"

// List of available registers and their names.
// We need a list of byte and doubleword registers, too.
// The list also includes the registers used to
// hold function parameters

#define NUM_FREE_REGS 4
int freeRegister[NUM_FREE_REGS];
char *registerList[] =
    {"%r10", "%r11", "%r12", "%r13", "%r9", "%r8", "%rcx", "%rdx", "%rsi", "%rdi"};

// We also need the 8-bit and 32-bit register names
char *registerByteList[] =
    {"%r10b", "%r11b", "%r12b", "%r13b", "%r9b", "%r8b", "%cl", "%dl", "%sil", "%dil"};

char *registerDoubleList[] =
    {"%r10d", "%r11d", "%r12d", "%r13d", "%r9d", "%r8d", "%ecx", "%edx", "%esi", "%edi"};

// Push and pop a register on/off the stack
void _cgRegisterPush(int r)
{
  fprintf(Outfile, "\tpushq\t%s\n", registerList[r]);
}

void _cgRegisterPop(int r)
{
  fprintf(Outfile, "\tpopq\t%s\n", registerList[r]);
}

// Set all registers as available.
// But if reg is positive, don't free that one.
void cgRegisterFreeAll(int keepreg)
{
  int i;
  for (i = 0; i < NUM_FREE_REGS; i++)
    if (i != keepreg)
      freeRegister[i] = 1;
}

// When we need to spill a register, we choose
// the following register and then cycle through
// the remaining registers. The spillreg increments
// continually, so we need to take a modulo NUMFREEREGS
// on it.
static int spillRegister = 0;

// Allocate a free register. Return the number of
// the register. Die if no available registers.
int cgRegisterAlloc()
{
  int reg;

  for (reg = 0; reg < NUM_FREE_REGS; reg++)
  {
    if (freeRegister[reg])
    {
      freeRegister[reg] = 0;
      return (reg);
    }
  }

  // We have no registers, so we must spill one
  reg = (spillRegister % NUM_FREE_REGS);
  spillRegister++;
  _cgRegisterPush(reg);
  return (reg);
}

// Return a register to the list of available registers.
// Check to see if it's not already there.
void cgRegisterFree(int reg)
{
  if (freeRegister[reg] != 0)
  {
    fatald("Error trying to free register", reg);
  }
  // If this was a spilled register, get it back
  if (spillRegister > 0)
  {
    spillRegister--;
    reg = (spillRegister % NUM_FREE_REGS);
    _cgRegisterPop(reg);
  }
  else
  {
    freeRegister[reg] = 1;
  }
}

// Spill all registers on the stack
void cgRegisterSpill()
{
  int i;

  for (i = 0; i < NUM_FREE_REGS; i++)
    _cgRegisterPush(i);
}

// Unspill all registers from the stack
void cgRegisterUnSpill()
{
  int i;

  for (i = NUM_FREE_REGS - 1; i >= 0; i--)
    _cgRegisterPop(i);
}

// Move value between registers
void cgRegisterMove(int r1, int r2)
{
  fprintf(Outfile, "\tmovq\t%s, %s\n", registerList[r1], registerList[r2]);
}