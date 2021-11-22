#ifndef _MAIN_H_
#define _MAIN_H_

#include "parser/parser.h"
#include "codegen/gen.h"

#include "incdir.h"


#define CMDLEN 90000


// Commands and default filenames
#define AOUT "a.out"
#define CPPCMD "cpp -nostdinc -isystem "

// Command-line flags
extern int O_dumpAST;		// If true, dump the AST trees
extern int O_dumpsym;		// If true, dump the symbol table
extern int O_keepasm;		// If true, keep any assembly files
extern int O_assemble;		// If true, assemble the assembly files
extern int O_dolink;		// If true, link the object files
extern int O_verbose;		// If true, print info on compilation stages

#endif