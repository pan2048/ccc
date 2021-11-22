#include "lib.h"

char *Infilename;  // Name of file we are parsing
int LineNumber;    // Current line number
char *Outfilename; // Name of file we opened as Outfile

void fatal(char *s1)
{
  fileUnlink(Outfilename);
  panic(s1, LineNumber, Infilename);
}

void fatals(char *s1, char *s2)
{
  fileUnlink(Outfilename);
  panics(s1, s2, LineNumber, Infilename);
}

void fatald(char *s, int d)
{
  fileUnlink(Outfilename);
  panicd(s, d, LineNumber, Infilename);
}

void fatalc(char *s, int c)
{
  fileUnlink(Outfilename);
  panicc(s, c, LineNumber, Infilename);
}

// Given a string with a '.' and at least a 1-character suffix
// after the '.', change the suffix to be the given character.
// Return the new string or NULL if the original string could
// not be modified
char *fileNameAlterSuffix(char *str, char suffix)
{
  char *posn;
  char *newstr;

  // Clone the string
  if ((newstr = strDup(str)) == NULL)
    return (NULL);

  // Find the '.'
  if ((posn = strRchr(newstr, '.')) == NULL)
    return (NULL);

  // Ensure there is a suffix
  posn++;
  if (*posn == '\0')
    return (NULL);

  // Change the suffix and NUL-terminate the string
  *posn = suffix;
  posn++;
  *posn = '\0';
  return (newstr);
}
