#include "../parser.h"

struct symTable *Functionid; // Symbol ptr of the current function

int Looplevel;   // Depth of nested loops
int Switchlevel; // Depth of nested switches

//struct symtable *composite_declaration(int type);
int typedef_declaration(struct symTable **ctype);

// Given a typedef name, return the type it represents
int type_of_typedef(char *name, struct symTable **ctype)
{
  struct symTable *t;

  // Look up the typedef in the list
  t = findtypedef(name);
  if (t == NULL)
    fatals("unknown type", name);
  scan(&Token);
  *ctype = t->ctype;
  return (t->type);
}

// Parse the current token and return a primitive type enum value,
// a pointer to any composite type and possibly modify
// the class of the type.
int parseType(struct symTable **ctype, int *class)
{
  int type, externStatic = 1;

  // See if the class has been changed to extern or static
  while (externStatic)
  {
    switch (Token.token)
    {
    case T_EXTERN:
      if (*class == C_STATIC)
        fatal("Illegal to have extern and static at the same time");
      *class = C_EXTERN;
      scan(&Token);
      break;
    case T_STATIC:
      if (*class == C_LOCAL)
        fatal("Compiler doesn't support static local declarations");
      if (*class == C_EXTERN)
        fatal("Illegal to have extern and static at the same time");
      *class = C_STATIC;
      scan(&Token);
      break;
    default:
      externStatic = 0;
    }
  }

  // Now work on the actual type keyword
  switch (Token.token)
  {
  case T_VOID:
    type = P_VOID;
    scan(&Token);
    break;
  case T_CHAR:
    type = P_CHAR;
    scan(&Token);
    break;
  case T_INT:
    type = P_INT;
    scan(&Token);
    break;
  case T_LONG:
    type = P_LONG;
    scan(&Token);
    break;

    // For the following, if we have a ';' after the
    // parsing then there is no type, so return -1.
    // Example: struct x {int y; int z};
  case T_STRUCT:
    type = P_STRUCT;
    *ctype = compositeDeclaration(P_STRUCT);
    if (Token.token == T_SEMI)
      type = -1;
    break;
  case T_UNION:
    type = P_UNION;
    *ctype = compositeDeclaration(P_UNION);
    if (Token.token == T_SEMI)
      type = -1;
    break;
  case T_ENUM:
    type = P_INT; // Enums are really ints
    enumDeclaration();
    if (Token.token == T_SEMI)
      type = -1;
    break;
  case T_TYPEDEF:
    type = typedef_declaration(ctype);
    if (Token.token == T_SEMI)
      type = -1;
    break;
  case T_IDENT:
    type = type_of_typedef(Text, ctype);
    break;
  default:
    fatals("Illegal type, token", Token.tokstr);
  }
  return (type);
}

// Given a type parsed by parse_type(), scan in any following
// '*' tokens and return the new type
int parseStars(int type)
{
  while (1)
  {
    if (Token.token != T_STAR)
      break;
    type = typePointerTo(type);
    scan(&Token);
  }
  return (type);
}

// Parse a type which appears inside a cast
int parseCast(struct symTable **ctype)
{
  int type, class = 0;

  // Get the type inside the parentheses
  type = parseStars(parseType(ctype, &class));

  // Do some error checking. I'm sure more can be done
  if (type == P_STRUCT || type == P_UNION || type == P_VOID)
    fatal("Cannot cast to a struct, union or void type");
  return (type);
}


// Parse a typedef declaration and return the type
// and ctype that it represents
int typedef_declaration(struct symTable **ctype)
{
  int type, class = 0;

  // Skip the typedef keyword.
  scan(&Token);

  // Get the actual type following the keyword
  type = parseType(ctype, &class);
  if (class != 0)
    fatal("Can't have static/extern in a typedef declaration");

  // See if the typedef identifier already exists
  if (findtypedef(Text) != NULL)
    fatals("redefinition of typedef", Text);

  // Get any following '*' tokens
  type = parseStars(type);

  // It doesn't exist so add it to the typedef list
  addtypedef(Text, type, *ctype);
  scan(&Token);
  return (type);
}
