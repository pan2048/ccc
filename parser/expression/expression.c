#include "../../codegen/gen.h"
#include "../parser.h"


// Convert a binary operator token into a binary AST operation.
// We rely on a 1:1 mapping from token to AST operation
int binastop(int tokentype)
{
  if (tokentype > T_EOF && tokentype <= T_MOD)
    return (tokentype);
  fatals("Syntax error, token", TokenString[tokentype]);
  return (0); // Keep -Wall happy
}

// Return true if a token is right-associative,
// false otherwise.
int rightassoc(int tokentype)
{
  if (tokentype >= T_ASSIGN && tokentype <= T_ASSLASH)
    return (1);
  return (0);
}


