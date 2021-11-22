#ifndef _PARSER_H_
#define _PARSER_H_

#include "../lib/lib.h"
#include "../model/model.h"
#include "../lexer/lexer.h"

extern int Looplevel;			// Depth of nested loops
extern int Switchlevel;		// Depth of nested switches

// expr.c
struct ASTnode *expression_list(int endtoken);
struct ASTnode *binexpr(int ptp);

int is_new_symbol(struct symtable *sym, int class, 
		  int type, struct symtable *ctype) ;
int parse_type(struct symtable **ctype, int *class);
int parse_stars(int type);
int parse_cast(struct symtable **ctype);
struct symtable *symbol_declaration(int type, struct symtable *ctype,
                                    int class, struct ASTnode **tree);
struct symtable *array_declaration(char *varname, int type,
					  struct symtable *ctype, int class); 
struct symtable *composite_declaration(int type); 
void enum_declaration();

int declarationList(struct symtable **ctype, int class, int et1, int et2,
		     struct ASTnode **gluetree);
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
struct ASTnode *returnStatement(void);
struct ASTnode *breakStatement(void);
struct ASTnode *continueStatement(void); 

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