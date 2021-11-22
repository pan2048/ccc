#include "../external/external.h"
#include "../main.h"

#define LDCMD "cc -no-pie -g -o"
char cmd[CMDLEN];

void doLink(char *outFileName, char **objFileNames, int verbose)
{
  char *cptr;

  int cnt, size = CMDLEN;
;
  int err;

  // Start with the linker command and the output file
  cptr = cmd;
  cnt = snprintf(cptr, size, "%s %s", LDCMD, outFileName);
  cptr += cnt;
  size -= cnt;
  if (size <= 0)
  {
    fprintf(stderr, "Linker has no enough buffer\n");
    sysExit(1);
  }

  // Now append each object file
  while (*objFileNames != NULL)
  {
    cnt = snprintf(cptr, size, " %s", *objFileNames);
    objFileNames++;
    cptr += cnt;
    size -= cnt;
    if (size <= 0)
    {
      fprintf(stderr, "Linker has no enough buffer\n");
      sysExit(1);
    }
  }

  if (verbose)
  {
    printf("%s\n", cmd);
  }
  err = sysRun(cmd);
  if (err != 0)
  {
    fprintf(stderr, "Linking failed\n");
    sysExit(1);
  }
}