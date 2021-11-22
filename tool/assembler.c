#include "../lib/lib.h"
#include "../main.h"

#define ASCMD "as -g -o "

extern char cmd[CMDLEN];
  
// down to object code. Return the object filename
char *doAssemble(char *fileName, int verbose)
{

  int err;

  char *outfilename = fileNameAlterSuffix(fileName, 'o');
  if (outfilename == NULL)
  {
    fprintf(stderr, "Error: %s has no suffix, try .s on the end\n", fileName);
    sysExit();
  }

  // Build the assembly command and run it
  snprintf(cmd, CMDLEN, "%s %s %s", ASCMD, outfilename, fileName);
  if (verbose)
    printf("%s\n", cmd);
  err = sysRun(cmd);
  if (err != 0)
  {
    fprintf(stderr, "Assembly of %s failed\n", fileName);
    sysExit();
  }
  return (outfilename);
}
