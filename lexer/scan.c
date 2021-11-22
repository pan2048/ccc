#include "../lib/lib.h"
#include "lexer.h"

char Text[TEXTLEN + 1]; // Last identifier scanned
int Linestart = 1;      // True if at start of a line
int Putback = 0;        // Character put back by scanner

// Get the next character from the input file.
int next()
{
  int c;

  if (Putback)
  {              // Use the character put
    c = Putback; // back if there is one
    Putback = 0;
    return (c);
  }

  c = inFileGetChar(); // Read from input file

  // Skip whitespace
  c = preProcess(c);

  Linestart = 0; // No longer at the start of the line
  if ('\n' == c)
  {
    LineNumber++;  // Increment line count
    Linestart = 1; // Now back at the start of the line
  }
  return (c);
}

// Put back an unwanted character
void putback(int c)
{
  if (Putback)
  {
    fatal("Already has putback");
  }
  else
  {
    Putback = c;
  }
}

// Skip past input that we don't need to deal with,
// i.e. whitespace, newlines. Return the first
// character we do need to deal with.
int skip()
{
  int c;

  c = next();
  while (' ' == c || '\t' == c || '\n' == c || '\r' == c || '\f' == c)
  {
    c = next();
  }
  return (c);
}

int preProcess(int c)
{
  int l;

  while (Linestart && c == '#')
  {                // We've hit a pre-processor statement
    Linestart = 0; // No longer at the start of the line
    scan(&Token);  // Get the line number into l
    if (Token.token != T_INTLIT)
      fatals("Expecting pre-processor line number, got:", Text);
    l = Token.intvalue;

    scan(&Token); // Get the filename in Text
    if (Token.token != T_STRLIT)
      fatals("Expecting pre-processor file name, got:", Text);

    if (Text[0] != '<')
    {                               // If this is a real filename
      if (strCmp(Text, Infilename)) // and not the one we have now
        Infilename = strDup(Text);  // save it. Then update the line num
      LineNumber = l;
    }

    while ((c = inFileGetChar()) != '\n')
      ;            // Skip to the end of the line
    c = next();    // and get the next character
    Linestart = 1; // Now back at the start of the line
  }
  return (c);
}

// Scan and return the next token found in the input.
// Return 1 if token valid, 0 if no tokens left.
int scan(struct token *t)
{
  int c;

  // If we have a lookahead token, return this token
  if (Peektoken.token != 0)
  {
    t->token = Peektoken.token;
    t->tokstr = Peektoken.tokstr;
    t->intvalue = Peektoken.intvalue;
    Peektoken.token = 0;
    return (1);
  }

  c = skip();

  if (scanLiteral(t, c))
  {
    // We found a token
    t->tokstr = TokenString[t->token];
    return (1);
  }
  if (scanIdentifer(t, c))
  {
    // We found a token
    t->tokstr = TokenString[t->token];
    return (1);
  }
  if (scanOperator(t, c))
  {
    // We found a token
    t->tokstr = TokenString[t->token];
    return (1);
  }

  // We found a token
  t->tokstr = TokenString[t->token];
  return (1);
}
