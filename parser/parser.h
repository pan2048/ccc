#ifndef _PARSER_H_
#define _PARSER_H_

#include "../lib/lib.h"
#include "../model/model.h"
#include "../lexer/lexer.h"

extern int Looplevel;			// Depth of nested loops
extern int Switchlevel;		// Depth of nested switches

struct ASTnode *expressionList(int endtoken);
struct ASTnode *funccall();
struct ASTnode *array_access(struct ASTnode *left);
struct ASTnode *member_access(struct ASTnode *left, int withpointer);
struct ASTnode *paren_expression(int ptp);
struct ASTnode *primaryFactor(int ptp);
struct ASTnode *binexpr(int ptp);
struct ASTnode *prefix(int ptp);
int op_precedence(int tokentype);
int rightassoc(int tokentype);
int binastop(int tokentype);
struct ASTnode *postfix(int ptp);

int is_new_symbol(struct symtable *sym, int class, 
		  int type, struct symtable *ctype) ;
int parse_type(struct symtable **ctype, int *class);
int parse_stars(int type);
int parse_cast(struct symtable **ctype);
int declarationList(struct symtable **ctype, int class, int et1, int et2,
		     struct ASTnode **gluetree);


struct symtable *array_declaration(char *varname, int type,
					  struct symtable *ctype, int class); 
void enum_declaration();
struct symtable *composite_declaration(int type); 
struct symtable *symbol_declaration(int type, struct symtable *ctype,
                                    int class, struct ASTnode **tree);
struct symtable *functionDeclaration(char *funcname, int type,
					     struct symtable *ctype,
					     int class);
void globalDeclarations(void);
struct symtable *scalarDeclaration(char *varname, int type,
					   struct symtable *ctype,
					   int class, struct ASTnode **tree); 

struct ASTnode *forStatement();
struct ASTnode *ifStatement();
struct ASTnode *whileStatement();
struct ASTnode *switchStatement();
struct ASTnode *singleStatement();
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