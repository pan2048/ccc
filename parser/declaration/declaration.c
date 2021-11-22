#include "../../lib/lib.h"
#include "../../codegen/gen.h"
#include "../../lexer/lexer.h"
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

// Given the type, name and class of an array variable, parse
// the size of the array, if any. Then parse any initialisation
// value and allocate storage for it.
// Return the variable's symbol table entry.
struct symtable *array_declaration(char *varname, int type,
                                   struct symtable *ctype, int class)
{

  struct symtable *sym; // New symbol table entry
  int nelems = -1;      // Assume the number of elements won't be given
  int maxelems;         // The maximum number of elements in the init list
  int *initlist;        // The list of initial elements
  int i = 0, j;

  // Skip past the '['
  scan(&Token);

  // See if we have an array size
  if (Token.token != T_RBRACKET)
  {
    nelems = literal(P_INT);
    if (nelems <= 0)
      fatald("Array size is illegal", nelems);
  }

  // Ensure we have a following ']'
  match(T_RBRACKET, "]");

  // Add this as a known array. We treat the
  // array as a pointer to its elements' type
  switch (class)
  {
  case C_STATIC:
  case C_EXTERN:
  case C_GLOBAL:
    // See if this variable is new or already exists
    sym = findglob(varname);
    if (is_new_symbol(sym, class, pointer_to(type), ctype))
      sym = addglob(varname, pointer_to(type), ctype, S_ARRAY, class, 0, 0);
    break;
  case C_LOCAL:
    sym = addlocl(varname, pointer_to(type), ctype, S_ARRAY, 0);
    break;
  default:
    fatal("Declaration of array parameters is not implemented");
  }

  // Array initialisation
  if (Token.token == T_ASSIGN)
  {
    if (class != C_GLOBAL && class != C_STATIC)
      fatals("Variable can not be initialised", varname);
    scan(&Token);

    // Get the following left curly bracket
    match(T_LBRACE, "{");

#define TABLE_INCREMENT 10

    // If the array already has nelems, allocate that many elements
    // in the list. Otherwise, start with TABLE_INCREMENT.
    if (nelems != -1)
      maxelems = nelems;
    else
      maxelems = TABLE_INCREMENT;
    initlist = (int *)memAlloc(maxelems * sizeof(int));

    // Loop getting a new literal value from the list
    while (1)
    {

      // Check we can add the next value, then parse and add it
      if (nelems != -1 && i == maxelems)
        fatal("Too many values in initialisation list");

      initlist[i++] = literal(type);

      // Increase the list size if the original size was
      // not set and we have hit the end of the current list
      if (nelems == -1 && i == maxelems)
      {
        maxelems += TABLE_INCREMENT;
        initlist = (int *)memRealloc((char *)initlist, maxelems * sizeof(int));
      }

      // Leave when we hit the right curly bracket
      if (Token.token == T_RBRACE)
      {
        scan(&Token);
        break;
      }

      // Next token must be a comma, then
      comma();
    }

    // Zero any unused elements in the initlist.
    // Attach the list to the symbol table entry
    for (j = i; j < sym->nelems; j++)
      initlist[j] = 0;

    if (i > nelems)
      nelems = i;
    sym->initlist = initlist;
  }

  // Set the size of the array and the number of elements
  // Only externs can have no elements.
  if (class != C_EXTERN && nelems <= 0)
    fatals("Array must have non-zero elements", sym->name);

  sym->nelems = nelems;
  sym->size = sym->nelems * typesize(type, ctype);
  // Generate any global space
  if (class == C_GLOBAL || class == C_STATIC)
    genGlobSym(sym);
  return (sym);
}

// Parse composite type declarations: structs or unions.
// Either find an existing struct/union declaration, or build
// a struct/union symbol table entry and return its pointer.
struct symtable *composite_declaration(int type)
{
  struct symtable *ctype = NULL;
  struct symtable *m;
  struct ASTnode *unused;
  int offset;
  int t;

  // Skip the struct/union keyword
  scan(&Token);

  // See if there is a following struct/union name
  if (Token.token == T_IDENT)
  {
    // Find any matching composite type
    if (type == P_STRUCT)
      ctype = findstruct(Text);
    else
      ctype = findunion(Text);
    scan(&Token);
  }

  // If the next token isn't an LBRACE , this is
  // the usage of an existing struct/union type.
  // Return the pointer to the type.
  if (Token.token != T_LBRACE)
  {
    if (ctype == NULL)
      fatals("unknown struct/union type", Text);
    return (ctype);
  }

  // Ensure this struct/union type hasn't been
  // previously defined
  if (ctype)
    fatals("previously defined struct/union", Text);

  // Build the composite type and skip the left brace
  if (type == P_STRUCT)
    ctype = addstruct(Text);
  else
    ctype = addunion(Text);
  scan(&Token);

  // Scan in the list of members
  while (1)
  {
    // Get the next member. m is used as a dummy
    t = declarationList(&m, C_MEMBER, T_SEMI, T_RBRACE, &unused);
    if (t == -1)
      fatal("Bad type in member list");
    if (Token.token == T_SEMI)
      scan(&Token);
    if (Token.token == T_RBRACE)
      break;
  }

  // Attach to the struct type's node
  rbrace();
  if (Membhead == NULL)
    fatals("No members in struct", ctype->name);
  ctype->member = Membhead;
  Membhead = Membtail = NULL;

  // Set the offset of the initial member
  // and find the first free byte after it
  m = ctype->member;
  m->st_posn = 0;
  offset = typesize(m->type, m->ctype);

  // Set the position of each successive member in the composite type
  // Unions are easy. For structs, align the member and find the next free byte
  for (m = m->next; m != NULL; m = m->next)
  {
    // Set the offset for this member
    if (type == P_STRUCT)
      m->st_posn = genAlign(m->type, offset, 1);
    else
      m->st_posn = 0;

    // Get the offset of the next free byte after this member
    offset += typesize(m->type, m->ctype);
  }

  // Set the overall size of the composite type
  ctype->size = offset;
  return (ctype);
}

// Parse an enum declaration
void enum_declaration()
{
  struct symtable *etype = NULL;
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

// Parse the declaration of a variable or function.
// The type and any following '*'s have been scanned, and we
// have the identifier in the Token variable.
// The class argument is the symbol's class.
// Return a pointer to the symbol's entry in the symbol table
struct symtable *symbol_declaration(int type, struct symtable *ctype,
                                    int class, struct ASTnode **tree)
{
  struct symtable *sym = NULL;
  char *varname = strDup(Text);

  // Ensure that we have an identifier.
  // We copied it above so we can scan more tokens in, e.g.
  // an assignment expression for a local variable.
  ident();

  // Deal with function declarations
  if (Token.token == T_LPAREN)
  {
    return (functionDeclaration(varname, type, ctype, class));
  }

  // See if this array or scalar variable has already been declared
  switch (class)
  {
  case C_EXTERN:
  case C_STATIC:
  case C_GLOBAL:
  case C_LOCAL:
  case C_PARAM:
    if (findlocl(varname) != NULL)
      fatals("Duplicate local variable declaration", varname);
  case C_MEMBER:
    if (findmember(varname) != NULL)
      fatals("Duplicate struct/union member declaration", varname);
  }

  // Add the array or scalar variable to the symbol table
  if (Token.token == T_LBRACKET)
  {
    sym = array_declaration(varname, type, ctype, class);
    *tree = NULL; // Local arrays are not initialised
  }
  else
    sym = scalarDeclaration(varname, type, ctype, class, tree);
  return (sym);
}
