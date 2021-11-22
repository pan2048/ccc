#include "lexer.h"
#include "../lib/lib.h"
#include "string.h"


// Scan an identifier from the input file and
// store it in buf[]. Return the identifier's length
int scanident(int c, char *buf, int lim)
{
  int i = 0;

  // Allow digits, alpha and underscores
  while (charIsAlpha(c) || charIsDigit(c) || '_' == c)
  {
    // Error if we hit the identifier length limit,
    // else append to buf[] and get next character
    if (lim - 1 == i)
    {
      fatal("Identifier too long");
    }
    else if (i < lim - 1)
    {
      buf[i++] = (char)c;
    }
    c = next();
  }

  // We hit a non-valid character, put it back.
  // NUL-terminate the buf[] and return the length
  putback(c);
  buf[i] = '\0';
  return (i);
}

// Given a word from the input, return the matching
// keyword token number or 0 if it's not a keyword.
// Switch on the first letter so that we don't have
// to waste time strcmp()ing against all the keywords.
static int keyword(char *s)
{
  switch (*s)
  {
  case 'b':
    if (!strcmp(s, "break"))
      return (T_BREAK);
    break;
  case 'c':
    if (!strcmp(s, "case"))
      return (T_CASE);
    if (!strcmp(s, "char"))
      return (T_CHAR);
    if (!strcmp(s, "continue"))
      return (T_CONTINUE);
    break;
  case 'd':
    if (!strcmp(s, "default"))
      return (T_DEFAULT);
    break;
  case 'e':
    if (!strcmp(s, "else"))
      return (T_ELSE);
    if (!strcmp(s, "enum"))
      return (T_ENUM);
    if (!strcmp(s, "extern"))
      return (T_EXTERN);
    break;
  case 'f':
    if (!strcmp(s, "for"))
      return (T_FOR);
    break;
  case 'i':
    if (!strcmp(s, "if"))
      return (T_IF);
    if (!strcmp(s, "int"))
      return (T_INT);
    break;
  case 'l':
    if (!strcmp(s, "long"))
      return (T_LONG);
    break;
  case 'r':
    if (!strcmp(s, "return"))
      return (T_RETURN);
    break;
  case 's':
    if (!strcmp(s, "sizeof"))
      return (T_SIZEOF);
    if (!strcmp(s, "static"))
      return (T_STATIC);
    if (!strcmp(s, "struct"))
      return (T_STRUCT);
    if (!strcmp(s, "switch"))
      return (T_SWITCH);
    break;
  case 't':
    if (!strcmp(s, "typedef"))
      return (T_TYPEDEF);
    break;
  case 'u':
    if (!strcmp(s, "union"))
      return (T_UNION);
    break;
  case 'v':
    if (!strcmp(s, "void"))
      return (T_VOID);
    break;
  case 'w':
    if (!strcmp(s, "while"))
      return (T_WHILE);
    break;
  }
  return (0);
}

int scanIdentifer(struct token *t, int c) {
    int tokentype;

    if (charIsAlpha(c) || '_' == c)
    {
      // Read in a keyword or identifier
      scanident(c, Text, TEXTLEN);

      // If it's a recognised keyword, return that token
      if ((tokentype = keyword(Text)) != 0)
      {
        t->token = tokentype;
        return (1);
      }

      // Not a recognised keyword, so it must be an identifier
      t->token = T_IDENT;
      return (1);
    }
    return (0);
}