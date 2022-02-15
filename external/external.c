#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "os.h"
#include "external.h"

FILE *InFile;  // Input file
FILE *Outfile; // Output file


void sysExit()
{
  os_exit();
}

int inFileGetChar()
{
  return (fgetc(InFile));
}

void inFilePipeOpen(char *cmd, char *fileName)
{
  if ((InFile = popen(cmd, "r")) == NULL)
  {
    fprintf(stderr, "Unable to open %s: %s\n", fileName, strerror(errno));
    sysExit();
  }
}

void outFileOpen(char *outFileName)
{
  // Create the output file
  if ((Outfile = fopen(outFileName, "w")) == NULL)
  {
    fprintf(stderr, "Unable to create %s: %s\n", outFileName, strerror(errno));
    sysExit();
  }
}

void fileUnlink(char *fileName)
{
  unlink(fileName);
}

int sysRun(char *command)
{
  return (system(command));
}

void panic(char *s1, int lineNum, char *fileName)
{
  fprintf(stderr, "%s on line %d of %s\n", s1, lineNum, fileName);
  fclose(Outfile);
  sysExit();
}

void panics(char *s1, char *s2, int lineNum, char *fileName)
{
  fprintf(stderr, "%s:%s on line %d of %s\n", s1, s2, lineNum, fileName);
  fclose(Outfile);
  sysExit();
}

void panicd(char *s, int d, int lineNum, char *fileName)
{
  fprintf(stderr, "%s:%d on line %d of %s\n", s, d, lineNum, fileName);
  fclose(Outfile);
  sysExit();
}

void panicc(char *s, int c, int lineNum, char *fileName)
{
  fprintf(stderr, "%s:%c on line %d of %s\n", s, c, lineNum, fileName);
  fclose(Outfile);
  sysExit();
}

void *memAlloc(int size)
{
  return (malloc(size));
}

void *memRealloc(char *ptr, int size)
{
  return (realloc(ptr, size));
}

