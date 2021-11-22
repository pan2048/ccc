#include "lexer.h"
#include "../lib/lib.h"

int scanInteger(struct token *t, int c)
{
    int k, val = 0, radix = 10;

    if (!charIsDigit(c))
        return (0);

    // Assume the radix is 10, but if it starts with 0
    if (c == '0')
    {
        // and the next character is 'x', it's radix 16
        if ((c = next()) == 'x')
        {
            radix = 16;
            c = next();
        }
        else
            // Otherwise, it's radix 8
            radix = 8;
    }

    // Convert each character into an int value
    while ((k = charPos("0123456789abcdef", charToLower(c))) >= 0)
    {
        if (k >= radix)
            fatalc("invalid digit in integer literal", c);
        val = val * radix + k;
        c = next();
    }

    // We hit a non-integer character, put it back.
    putback(c);

    t->intvalue = val;
    t->token = T_INTLIT;
    return (1);
}

// Read in a hexadecimal constant from the input
int scanHexChar(void)
{
  int c, h, n = 0, f = 0;

  // Loop getting characters
  while (charIsXDigit(c = next()))
  {
    // Convert from char to int value
    h = charPos("0123456789abcdef", charToLower(c));

    // Add to running hex value
    n = n * 16 + h;
    f = 1;
  }

  // We hit a non-hex character, put it back
  putback(c);

  // Flag tells us we never saw any hex characters
  if (!f)
    fatal("missing digits after '\\x'");
  if (n > 255)
    fatal("value out of range after '\\x'");

  return (n);
}


// Return the next character from a character
// or string literal
int scanChar(void)
{
  int i, c, c2;

  // Get the next input character and interpret
  // metacharacters that start with a backslash
  c = next();
  if (c == '\\')
  {
    switch (c = next())
    {
    case 'a':
      return ('\a');
    case 'b':
      return ('\b');
    case 'f':
      return ('\f');
    case 'n':
      return ('\n');
    case 'r':
      return ('\r');
    case 't':
      return ('\t');
    case 'v':
      return ('\v');
    case '\\':
      return ('\\');
    case '"':
      return ('"');
    case '\'':
      return ('\'');

      // Deal with octal constants by reading in
      // characters until we hit a non-octal digit.
      // Build up the octal value in c2 and count
      // # digits in i. Permit only 3 octal digits.
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
      for (i = c2 = 0; charIsDigit(c) && c < '8'; c = next())
      {
        if (++i > 3)
          break;
        c2 = c2 * 8 + (c - '0');
      }

      putback(c); // Put back the first non-octal char
      return (c2);
    case 'x':
      return (scanHexChar());
    default:
      fatalc("unknown escape sequence", c);
    }
  }
  return (c); // Just an ordinary old character!
}

int scanCharacter(struct token *t, int c)
{
    if (c != '\'')
        return (0);
    t->intvalue = scanChar();
    t->token = T_INTLIT;
    if (next() != '\'')
      fatal("Expected '\\'' at end of char literal");        
    return (1);  
}

int scanString(struct token *t, int c)
{
    int i;

    if (c != '"')
        return (0);

    // Loop while we have enough buffer space
    for (i = 0; i < TEXTLEN - 1; i++)
    {
        // Get the next char and append to buf
        // Return when we hit the ending double quote
        if ((c = scanChar()) == '"')
        {
            Text[i] = 0;
            t->token = T_STRLIT;
            return (1);
        }
        Text[i] = (char)c;
    }

    // Ran out of buffer space
    fatal("String literal too long");
    return (0);
}

int scanLiteral(struct token *t, int c)
{
    if (scanInteger(t, c))
    {
        return (1);
    }
    if (scanCharacter(t, c))
    {
        return (1);
    }
    if (scanString(t, c))
    {
        return (1);
    }
    return (0);
}