#ifndef _TOOL_H_
#define _TOOL_H_

char *doAssemble(char *fileName, int verbose);
void doLink(char *outFileName, char **objFileNames, int verbose);

#endif