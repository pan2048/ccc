#include "token.h"

struct token Token;		// Last token scanned
struct token Peektoken;		// A look-ahead token

// List of token strings, for debugging purposes
char *TokenString[] = {
    "EOF",  
    "=", "+=", "-=", "*=", "/=", "%=",
    "?", "||", "&&", "|", "^", "&",
    "==", "!=", "<", ">", "<=", ">=", "<<", ">>",
    "+", "-", "*", "/", "%", "++", "--", "~", "!",
    "void", "char", "int", "long",
    "if", "else", "while", "for", "return",
    "struct", "union", "enum", "typedef",
    "extern", "break", "continue", "switch",
    "case", "default", "sizeof", "static",
    "intLiteral", "strLiteral", ";", "identifier",
    "{", "}", "(", ")", "[", "]", ",", ".",
    "->", ":"};


// Return true if a token is right-associative,
// false otherwise.
int tokenRightAssociate(int tokentype)
{
  if (tokentype >= T_ASSIGN && tokentype <= T_ASSLASH)
    return (1);
  return (0);
}

// Convert a binary operator token into a binary AST operation.
// We rely on a 1:1 mapping from token to AST operation
int tokenBinaryAstOperation(int tokentype)
{
  if (tokentype > T_EOF && tokentype <= T_MOD)
    return (tokentype);
  fatals("Syntax error, token", TokenString[tokentype]);
  return (0); // Keep -Wall happy
}

// Operator precedence for each token. Must
// match up with the order of tokens in defs.h
int OpPrec[] = {
    0, 10, 10,      // T_EOF, T_ASSIGN, T_ASPLUS,
    10, 10,         // T_ASMINUS, T_ASSTAR,
    10, 10,         // T_ASSLASH, T_ASMOD,
    15,             // T_QUESTION,
    20, 30,         // T_LOGOR, T_LOGAND
    40, 50, 60,     // T_OR, T_XOR, T_AMPER
    70, 70,         // T_EQ, T_NE
    80, 80, 80, 80, // T_LT, T_GT, T_LE, T_GE
    90, 90,         // T_LSHIFT, T_RSHIFT
    100, 100,       // T_PLUS, T_MINUS
    110, 110, 110   // T_STAR, T_SLASH, T_MOD
};

// Check that we have a binary operator and
// return its precedence.
int tokenOperatorPrecedence(int tokentype)
{
  int prec;
  if (tokentype > T_MOD)
    fatals("Token with no precedence in op_precedence:", TokenString[tokentype]);
  prec = OpPrec[tokentype];
  if (prec == 0)
    fatals("Syntax error, token", TokenString[tokentype]);
  return (prec);
}
