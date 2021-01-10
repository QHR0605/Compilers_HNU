#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include "analyze.h"
#include "globals.h"

/* the list of line numbers of the source
 * code in which a variable is referenced
 */
typedef struct LineListRec {
    int lineno;
    struct LineListRec *next;
} * LineList;

/* The record in the bucket lists for
 * each variable, including name,
 * assigned memory location, and
 * the list of line numbers in which
 * it appears in the source code
 */
typedef struct BucketListRec {
    char *name;
    struct LineListRec *lines;
    TreeNode *node;
    struct BucketListRec *next;
} * BucketList;

int pushParam(TreeNode *param);

TreeNode *popParam(void);

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert(char *name, int lineno, TreeNode *t);

int get_global_var_num(void);

TreeNode *st_func_lookup(char *name);

/* Function st_lookup returns the memory
 * location of a variable or -1 if not found
 */
BucketList st_lookup(char *name, TreeNode *t);

/* Procedure printSymTab prints a formatted
 * listing of the symbol table contents
 * to the listing file
 */
void printSymTab(FILE *listing);

#endif
