#ifndef _PARSER_H_
#define _PARSER_H_

#include "../lib/lib.h"
#include "../model/model.h"
#include "../lexer/lexer.h"

extern int Looplevel;			// Depth of nested loops
extern int Switchlevel;		// Depth of nested switches

struct ASTnode *expressionList(int endToken);
struct ASTnode *funcCall();
struct ASTnode *arrayAccess(struct ASTnode *left);
struct ASTnode *memberAccess(struct ASTnode *left, int withpointer);
struct ASTnode *parenthesisedExpression(int previousTokenPrecedence);
struct ASTnode *primaryFactor(int previousTokenPrecedence);
struct ASTnode *binaryExpression(int previousTokenPrecedence);
struct ASTnode *prefixExpression(int previousTokenPrecedence);


struct ASTnode *postfixExpression(int ptp);


int parseType(struct symTable **ctype, int *class);
int typedef_declaration(struct symTable **ctype);
int type_of_typedef(char *name, struct symTable **ctype);
int parseStars(int type);
int parseCast(struct symTable **ctype);
int declarationList(struct symTable **ctype, int class, int et1, int et2,
		     struct ASTnode **gluetree);


struct symTable *arrayDeclaration(char *varname, int type,
					  struct symTable *ctype, int class); 
void enumDeclaration();
struct symTable *compositeDeclaration(int type); 
struct symTable *symbolDeclaration(int type, struct symTable *ctype,
                                    int class, struct ASTnode **tree);
struct symTable *functionDeclaration(char *funcname, int type,
					     struct symTable *ctype,
					     int class);
void globalDeclarations(void);
struct symTable *scalarDeclaration(char *varname, int type,
					   struct symTable *ctype,
					   int class, struct ASTnode **tree); 

struct ASTnode *forStatement();
struct ASTnode *ifStatement();
struct ASTnode *whileStatement();
struct ASTnode *switchStatement();
struct ASTnode *statement();
struct ASTnode *compoundStatement(int inswitch);
struct ASTnode *returnStatement();
struct ASTnode *breakStatement();
struct ASTnode *continueStatement(); 

int literal(int type);

void match(int t, char *what);
void semi(void);
void lbrace(void);
void rbrace(void);
void lparen(void);
void rparen(void);
void ident(void);
void comma(void);

struct ASTnode *optimise(struct ASTnode *n);

#endif