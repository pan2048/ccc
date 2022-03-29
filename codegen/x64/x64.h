#ifndef _X64_H
#define _X64_H

#include "../../lib/lib.h"
#include "../genLib.h"
#include "../cg.h"


enum {
  NOREG = -1			// Use NOREG when the AST generation
  				// functions have no register to return
};

extern char *registerList[];
extern char *registerByteList[];
extern char *registerDoubleList[];

void _cgDirectiveTextSeg();
void _cgDirectiveDataSeg();

void _cgSwitchRoutine();



#endif