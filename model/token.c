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
