#include "../parser.h"

// Parse a single statement and return its AST.
struct ASTnode *singleStatement()
{
  struct ASTnode *stmt;
  struct symtable *ctype;
  int linenum = LineNumber;

  switch (Token.token)
  {
  case T_SEMI:
    // An empty statement
    semi();
    break;
  case T_LBRACE:
    // We have a '{', so this is a compound statement
    lbrace();
    stmt = compoundStatement(0);
    stmt->linenum = linenum;
    rbrace();
    return (stmt);
  case T_IDENT:
    // We have to see if the identifier matches a typedef.
    // If not, treat it as an expression.
    // Otherwise, fall down to the parse_type() call.
    if (findtypedef(Text) == NULL)
    {
      stmt = binexpr(0);
      stmt->linenum = linenum;
      semi();
      return (stmt);
    }
  case T_CHAR:
  case T_INT:
  case T_LONG:
  case T_STRUCT:
  case T_UNION:
  case T_ENUM:
  case T_TYPEDEF:
    // The beginning of a variable declaration list.
    declarationList(&ctype, C_LOCAL, T_SEMI, T_EOF, &stmt);
    semi();
    return (stmt); // Any assignments from the declarations
  case T_IF:
    stmt = ifStatement();
    stmt->linenum = linenum;
    return (stmt);
  case T_WHILE:
    stmt = whileStatement();
    stmt->linenum = linenum;
    return (stmt);
  case T_FOR:
    stmt = forStatement();
    stmt->linenum = linenum;
    return (stmt);
  case T_RETURN:
    stmt = returnStatement();
    stmt->linenum = linenum;
    return (stmt);
  case T_BREAK:
    stmt = breakStatement();
    stmt->linenum = linenum;
    return (stmt);
  case T_CONTINUE:
    stmt = continueStatement();
    stmt->linenum = linenum;
    return (stmt);
  case T_SWITCH:
    stmt = switchStatement();
    stmt->linenum = linenum;
    return (stmt);
  default:
    // For now, see if this is an expression.
    // This catches assignment statements.
    stmt = binexpr(0);
    stmt->linenum = linenum;
    semi();
    return (stmt);
  }
  return (NULL); // Keep -Wall happy
}
