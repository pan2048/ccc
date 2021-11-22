#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_

// Symbol table structure
struct symTable
{
    char *name;              // Name of a symbol
    int type;                // Primitive type for the symbol
    struct symTable *ctype;  // If struct/union, ptr to that type
    int stype;               // Structural type for the symbol
    int class;               // Storage class for the symbol
    int size;                // Total size in bytes of this symbol
    int nelems;              // Functions: # params. Arrays: # elements
#define st_endlabel st_posn  // For functions, the end label
    int st_posn;             // For locals, the negative offset
                             // from the stack base pointer
    int *initlist;           // List of initial values
    struct symTable *next;   // Next symbol in one list
    struct symTable *member; // First member of a function, struct,
};    


// Primitive types. The bottom 4 bits is an integer
// value that represents the level of indirection,
// e.g. 0= no pointer, 1= pointer, 2= pointer pointer etc.
enum {
  P_NONE_TYPE, P_VOID = 16, P_CHAR = 32, P_INT = 48, P_LONG = 64,
  P_STRUCT=80, P_UNION=96
};

// Storage classes
enum {
  C_GLOBAL = 1,			// Globally visible symbol
  C_LOCAL,			// Locally visible symbol
  C_PARAM,			// Locally visible function parameter
  C_EXTERN,			// External globally visible symbol
  C_STATIC,			// Static symbol, visible in one file
  C_STRUCT,			// A struct
  C_UNION,			// A union
  C_MEMBER,			// Member of a struct or union
  C_ENUMTYPE,			// A named enumeration type
  C_ENUMVAL,			// A named enumeration value
  C_TYPEDEF			// A named typedef
};


// Structural types
enum {
  S_VARIABLE, S_FUNCTION, S_ARRAY
};



// Symbol table lists
extern struct symTable *Globhead, *Globtail;	  // Global variables and functions
extern struct symTable *Loclhead, *Locltail;	  // Local variables
extern struct symTable *Parmhead, *Parmtail;	  // Local parameters
extern struct symTable *Membhead, *Membtail;	  // Temp list of struct/union members
extern struct symTable *Structhead, *Structtail; // List of struct types
extern struct symTable *Unionhead, *Uniontail;   // List of union types
extern struct symTable *Enumhead,  *Enumtail;    // List of enum types and values
extern struct symTable *Typehead,  *Typetail;    // List of typedefs

extern struct symTable *Functionid; 	// Symbol ptr of the current function

void clear_symtable(void);
void freeloclsyms(void);
void freestaticsyms(void);
void dumptable(struct symTable *head, char *name, int indent);
void dumpsymtables(void);


// sym.c
void appendsym(struct symTable **head, struct symTable **tail,
	       struct symTable *node);
struct symTable *newsym(char *name, int type, struct symTable *ctype,
			int stype, int class, int nelems, int posn);
struct symTable *addGlobal(char *name, int type, struct symTable *ctype,
			 int stype, int class, int nelems, int posn);
struct symTable *addLocal(char *name, int type, struct symTable *ctype,
			 int stype, int nelems);
struct symTable *addparm(char *name, int type, struct symTable *ctype,
			 int stype);
struct symTable *addStruct(char *name);
struct symTable *addUnion(char *name);
struct symTable *addmemb(char *name, int type, struct symTable *ctype,
			 int stype, int nelems);
struct symTable *addenum(char *name, int class, int value);
struct symTable *addtypedef(char *name, int type, struct symTable *ctype);
struct symTable *findGlobal(char *s);
struct symTable *findlocl(char *s);
struct symTable *findsymbol(char *s);
struct symTable *findmember(char *s);
struct symTable *findStruct(char *s);
struct symTable *findUnion(char *s);
struct symTable *findenumtype(char *s);
struct symTable *findenumval(char *s);
struct symTable *findtypedef(char *s);
int isNewSymbol(struct symTable *sym, int class, 
		  int type, struct symTable *ctype) ;
#endif