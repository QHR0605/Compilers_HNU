#ifndef SYMTAB_H
#define SYMTAB_H

#include "globals.h"

typedef struct BucketListRec {
    char          *name;        /* the name of the identifier */
    TreeNode      *declaration;  /* pointer to the symbol's dec. node */
    int           lineFirstReferenced;   /* self-explanatory */
    struct BucketListRec *next;      /* next node on this bucket chain */
} BucketList, *BucketListPtr;

extern int scopeDepth;

void initSymbolTable();

void st_insert(char *name, TreeNode *symbolDefNode, int lineDefined);

BucketListPtr st_lookup(char *name);

void out_scope();

void new_scope();

void end_scope();


#endif