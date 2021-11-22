#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_

// Symbol table structure
struct symtable
{
    char *name;              // Name of a symbol
    int type;                // Primitive type for the symbol
    struct symtable *ctype;  // If struct/union, ptr to that type
    int stype;               // Structural type for the symbol
    int class;               // Storage class for the symbol
    int size;                // Total size in bytes of this symbol
    int nelems;              // Functions: # params. Arrays: # elements
#define st_endlabel st_posn  // For functions, the end label
    int st_posn;             // For locals, the negative offset
                             // from the stack base pointer
    int *initlist;           // List of initial values
    struct symtable *next;   // Next symbol in one list
    struct symtable *member; // First member of a function, struct,
};    


// Primitive types. The bottom 4 bits is an integer
// value that represents the level of indirection,
// e.g. 0= no pointer, 1= pointer, 2= pointer pointer etc.
enum {
  P_NONE, P_VOID = 16, P_CHAR = 32, P_INT = 48, P_LONG = 64,
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
extern struct symtable *Globhead, *Globtail;	  // Global variables and functions
extern struct symtable *Loclhead, *Locltail;	  // Local variables
extern struct symtable *Parmhead, *Parmtail;	  // Local parameters
extern struct symtable *Membhead, *Membtail;	  // Temp list of struct/union members
extern struct symtable *Structhead, *Structtail; // List of struct types
extern struct symtable *Unionhead, *Uniontail;   // List of union types
extern struct symtable *Enumhead,  *Enumtail;    // List of enum types and values
extern struct symtable *Typehead,  *Typetail;    // List of typedefs

extern struct symtable *Functionid; 	// Symbol ptr of the current function

void clear_symtable(void);
void freeloclsyms(void);
void freestaticsyms(void);
void dumptable(struct symtable *head, char *name, int indent);
void dumpsymtables(void);


// sym.c
void appendsym(struct symtable **head, struct symtable **tail,
	       struct symtable *node);
struct symtable *newsym(char *name, int type, struct symtable *ctype,
			int stype, int class, int nelems, int posn);
struct symtable *addglob(char *name, int type, struct symtable *ctype,
			 int stype, int class, int nelems, int posn);
struct symtable *addlocl(char *name, int type, struct symtable *ctype,
			 int stype, int nelems);
struct symtable *addparm(char *name, int type, struct symtable *ctype,
			 int stype);
struct symtable *addstruct(char *name);
struct symtable *addunion(char *name);
struct symtable *addmemb(char *name, int type, struct symtable *ctype,
			 int stype, int nelems);
struct symtable *addenum(char *name, int class, int value);
struct symtable *addtypedef(char *name, int type, struct symtable *ctype);
struct symtable *findglob(char *s);
struct symtable *findlocl(char *s);
struct symtable *findsymbol(char *s);
struct symtable *findmember(char *s);
struct symtable *findstruct(char *s);
struct symtable *findunion(char *s);
struct symtable *findenumtype(char *s);
struct symtable *findenumval(char *s);
struct symtable *findtypedef(char *s);
#endif