#include "../parser.h"

struct symtable *Functionid; // Symbol ptr of the current function

int Looplevel;   // Depth of nested loops
int Switchlevel; // Depth of nested switches

//struct symtable *composite_declaration(int type);
int typedef_declaration(struct symtable **ctype);

// Given a typedef name, return the type it represents
int type_of_typedef(char *name, struct symtable **ctype)
{
  struct symtable *t;

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
int parse_type(struct symtable **ctype, int *class)
{
  int type, exstatic = 1;

  // See if the class has been changed to extern or static
  while (exstatic)
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
      exstatic = 0;
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
    *ctype = composite_declaration(P_STRUCT);
    if (Token.token == T_SEMI)
      type = -1;
    break;
  case T_UNION:
    type = P_UNION;
    *ctype = composite_declaration(P_UNION);
    if (Token.token == T_SEMI)
      type = -1;
    break;
  case T_ENUM:
    type = P_INT; // Enums are really ints
    enum_declaration();
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
int parse_stars(int type)
{
  while (1)
  {
    if (Token.token != T_STAR)
      break;
    type = pointer_to(type);
    scan(&Token);
  }
  return (type);
}

// Parse a type which appears inside a cast
int parse_cast(struct symtable **ctype)
{
  int type, class = 0;

  // Get the type inside the parentheses
  type = parse_stars(parse_type(ctype, &class));

  // Do some error checking. I'm sure more can be done
  if (type == P_STRUCT || type == P_UNION || type == P_VOID)
    fatal("Cannot cast to a struct, union or void type");
  return (type);
}

// Given a pointer to a symbol that may already exist
// return true if this symbol doesn't exist. We use
// this function to convert externs into globals
int is_new_symbol(struct symtable *sym, int class,
                  int type, struct symtable *ctype)
{

  // There is no existing symbol, thus is new
  if (sym == NULL)
    return (1);

  // global versus extern: if they match that it's not new
  // and we can convert the class to global
  if ((sym->class == C_GLOBAL && class == C_EXTERN) || (sym->class == C_EXTERN && class == C_GLOBAL))
  {

    // If the types don't match, there's a problem
    if (type != sym->type)
      fatals("Type mismatch between global/extern", sym->name);

    // Struct/unions, also compare the ctype
    if (type >= P_STRUCT && ctype != sym->ctype)
      fatals("Type mismatch between global/extern", sym->name);

    // If we get to here, the types match, so mark the symbol
    // as global
    sym->class = C_GLOBAL;
    // Return that symbol is not new
    return (0);
  }

  // It must be a duplicate symbol if we get here
  fatals("Duplicate global variable declaration", sym->name);
  return (-1); // Keep -Wall happy
}

// Parse a typedef declaration and return the type
// and ctype that it represents
int typedef_declaration(struct symtable **ctype)
{
  int type, class = 0;

  // Skip the typedef keyword.
  scan(&Token);

  // Get the actual type following the keyword
  type = parse_type(ctype, &class);
  if (class != 0)
    fatal("Can't have static/extern in a typedef declaration");

  // See if the typedef identifier already exists
  if (findtypedef(Text) != NULL)
    fatals("redefinition of typedef", Text);

  // Get any following '*' tokens
  type = parse_stars(type);

  // It doesn't exist so add it to the typedef list
  addtypedef(Text, type, *ctype);
  scan(&Token);
  return (type);
}
