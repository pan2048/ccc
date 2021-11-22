#include "tool/tool.h"
#include "lib/lib.h"
#include "lexer/lexer.h"
#include "main.h"
#include "parser/parser.h"

// Command-line flags
int O_dumpAST;  // If true, dump the AST trees
int O_dumpsym;  // If true, dump the symbol table
int O_keepasm;  // If true, keep any assembly files
int O_assemble; // If true, assemble the assembly files
int O_dolink;   // If true, link the object files
int O_verbose;  // If true, print info on compilation stages

// Given an input filename, compile that file
// down to assembly code. Return the new file's name
char *doCompile(char *filename)
{
  char cmd[CMDLEN];

  // Change the input file's suffix to .s
  Outfilename = fileNameAlterSuffix(filename, 's');
  if (Outfilename == NULL)
  {
    fprintf(stderr, "Error: %s has no suffix, try .c on the end\n", filename);
    sysExit();
  }

  // Generate the pre-processor command
  snprintf(cmd, CMDLEN, "%s %s %s", CPPCMD, INCDIR, filename);

  // Open up the pre-processor pipe
  inFilePipeOpen(cmd, filename);

  Infilename = filename;

  // Create the output file
  outFileOpen(Outfilename);

  LineNumber = 1; // Reset the scanner
  Linestart = 1;
  Putback = '\n';
  clear_symtable(); // Clear the symbol table
  if (O_verbose)
    printf("compiling %s\n", filename);
  scan(&Token);              // Get the first token from the input
  Peektoken.token = 0;       // and set there is no lookahead token
  genPreamble(filename);     // Output the preamble
  parseGlobalDeclarations(); // Parse the global declarations
  genPostamble();            // Output the postamble
  fclose(Outfile);           // Close the output file

  // Dump the symbol table if requested
  if (O_dumpsym)
  {
    printf("Symbols for %s\n", filename);
    dumpsymtables();
    fprintf(stdout, "\n\n");
  }

  freestaticsyms(); // Free any static symbols in the file
  return (Outfilename);
}

// Print out a usage if started incorrectly
static void usage(char *prog)
{
  fprintf(stderr, "Usage: %s [-vcSTM] [-o outfile] file [file ...]\n", prog);
  fprintf(stderr,
          "       -v give verbose output of the compilation stages\n");
  fprintf(stderr, "       -c generate object files but don't link them\n");
  fprintf(stderr, "       -S generate assembly files but don't link them\n");
  fprintf(stderr, "       -T dump the AST trees for each input file\n");
  fprintf(stderr, "       -M dump the symbol table for each input file\n");
  fprintf(stderr, "       -o outfile, produce the outfile executable file\n");
  sysExit();
}

// Main program: check arguments and print a usage
// if we don't have an argument. Open up the input
// file and call scanfile() to scan the tokens in it.
enum
{
  MAXOBJ = 300
};
int main(int argc, char **argv)
{
  char *outFileName = AOUT;
  char *asmFileName, *objFileName;
  char *objFileNames[MAXOBJ];
  int i, j, objCount = 0;

  // Initialise our variables
  O_dumpAST = 0;
  O_dumpsym = 0;
  O_keepasm = 0;
  O_assemble = 0;
  O_verbose = 0;
  O_dolink = 1;

  // Scan for command-line options
  for (i = 1; i < argc; i++)
  {
    // No leading '-', stop scanning for options
    if (*argv[i] != '-')
      break;

    // For each option in this argument
    for (j = 1; (*argv[i] == '-') && argv[i][j]; j++)
    {
      switch (argv[i][j])
      {
      case 'o':
        outFileName = argv[++i]; // Save & skip to next argument
        break;
      case 'T':
        O_dumpAST = 1;
        break;
      case 'M':
        O_dumpsym = 1;
        break;
      case 'c':
        O_assemble = 1;
        O_keepasm = 0;
        O_dolink = 0;
        break;
      case 'S':
        O_keepasm = 1;
        O_assemble = 0;
        O_dolink = 0;
        break;
      case 'v':
        O_verbose = 1;
        break;
      default:
        usage(argv[0]);
      }
    }
  }

  // Ensure we have at lease one input file argument
  if (i >= argc)
    usage(argv[0]);

  // Work on each input file in turn
  while (i < argc)
  {
    asmFileName = doCompile(argv[i]); // Compile the source file

    if (O_dolink || O_assemble)
    {
      objFileName = doAssemble(asmFileName, O_verbose); // Assemble it to object forma
      if (objCount == (MAXOBJ - 2))
      {
        fprintf(stderr, "Too many object files for the compiler to handle\n");
        sysExit();
      }
      objFileNames[objCount++] = objFileName; // Add the object file's name
      objFileNames[objCount] = NULL;          // to the list of object files
    }

    if (!O_keepasm)            // Remove the assembly file if
      fileUnlink(asmFileName); // we don't need to keep it
    i++;
  }

  // Now link all the object files together
  if (O_dolink)
  {
    doLink(outFileName, objFileNames, O_verbose);

    // If we don't need to keep the object
    // files, then remove them
    if (!O_assemble)
    {
      for (i = 0; objFileNames[i] != NULL; i++)
        fileUnlink(objFileNames[i]);
    }
  }

  return (0);
}
