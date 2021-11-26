#ifndef _TYPE_H_
#define _TYPE_H_

// types.c
int typeIsInt(int type);
int typeIsPointer(int type);
int typePointerTo(int type);
int typeSize(int type, struct symTable *ctype);
struct ASTnode *typeModify(struct ASTnode *tree, int rtype,
			    struct symTable *rctype, int op);
int typeValueAt(int type); 


#endif