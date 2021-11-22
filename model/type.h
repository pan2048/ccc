#ifndef _TYPE_H_
#define _TYPE_H_

// types.c
int intType(int type);
int typeIsPointer(int type);
int pointerTo(int type);
int valueAt(int type);
int typeSize(int type, struct symTable *ctype);
struct ASTnode *modifyType(struct ASTnode *tree, int rtype,
			    struct symTable *rctype, int op);
int valueAt(int type); 

#endif