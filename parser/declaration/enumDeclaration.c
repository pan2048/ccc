#include "../parser.h"

// Parse an enum declaration
void enumDeclaration()
{
  struct symTable *etype = NULL;
  char *name = NULL;
  int intval = 0;

  // Skip the enum keyword.
  scan(&Token);

  // If there's a following enum type name, get a
  // pointer to any existing enum type node.
  if (Token.token == T_IDENT)
  {
    etype = findenumtype(Text);
    name = strDup(Text); // As it gets tromped soon
    scan(&Token);
  }

  // If the next token isn't a LBRACE, check
  // that we have an enum type name, then return
  if (Token.token != T_LBRACE)
  {
    if (etype == NULL)
      fatals("undeclared enum type:", name);
    return;
  }

  // We do have an LBRACE. Skip it
  scan(&Token);

  // If we have an enum type name, ensure that it
  // hasn't been declared before.
  if (etype != NULL)
    fatals("enum type redeclared:", etype->name);
  else
    // Build an enum type node for this identifier
    etype = addenum(name, C_ENUMTYPE, 0);

  // Loop to get all the enum values
  while (1)
  {
    // Ensure we have an identifier
    // Copy it in case there's an int literal coming up
    ident();
    name = strDup(Text);

    // Ensure this enum value hasn't been declared before
    etype = findenumval(name);
    if (etype != NULL)
      fatals("enum value redeclared:", Text);

    // If the next token is an '=', skip it and
    // get the following int literal
    if (Token.token == T_ASSIGN)
    {
      scan(&Token);
      if (Token.token != T_INTLIT)
        fatal("Expected int literal after '='");
      intval = Token.intvalue;
      scan(&Token);
    }

    // Build an enum value node for this identifier.
    // Increment the value for the next enum identifier.
    etype = addenum(name, C_ENUMVAL, intval++);

    // Bail out on a right curly bracket, else get a comma
    if (Token.token == T_RBRACE)
      break;
    comma();
  }
  scan(&Token); // Skip over the right curly bracket
}
