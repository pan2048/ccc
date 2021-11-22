#include "../parser.h"
#include "../../codegen/gen.h"

// Parse composite type declarations: structs or unions.
// Either find an existing struct/union declaration, or build
// a struct/union symbol table entry and return its pointer.
struct symTable *compositeDeclaration(int type)
{
  struct symTable *ctype = NULL;
  struct symTable *m;
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
      ctype = findStruct(Text);
    else
      ctype = findUnion(Text);
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
    ctype = addStruct(Text);
  else
    ctype = addUnion(Text);
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
  offset = typeSize(m->type, m->ctype);

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
    offset += typeSize(m->type, m->ctype);
  }

  // Set the overall size of the composite type
  ctype->size = offset;
  return (ctype);
}
