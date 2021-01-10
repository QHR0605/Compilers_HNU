#include "symtab.h"
#include "parse.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* SIZE is the size of the hash table */
#define SIZE 211

/* SHIFT is the power of two used as multiplier
   in hash function  */
#define SHIFT 4

/* stack for call */
TreeNode *paramStack[SIZE];
int top = 0;

int pushParam(TreeNode *param) {
    if (top == SIZE)
        return -1;
    paramStack[top++] = param;
    return 0;
}

TreeNode *popParam() {
    if (top == 0)
        return NULL;
    else
        return paramStack[--top];
}

/* the hash function */
static int hash(char *key) {
    int temp = 0;
    int i = 0;

    if (key == NULL) {
        printf("cannot hash!\n");
        exit(-1);
    }

    while (key[i] != '\0') {
        temp = ((temp << SHIFT) + key[i]) % SIZE;
        ++i;
    }
    return temp;
}

/* the hash table */
static BucketList hashTable[SIZE];

void st_init(void) {
    memset(hashTable, 0, sizeof(BucketList) * SIZE);
}

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert(char *name, int lineno, TreeNode *t) {
    int h = hash(name);
    BucketList l = hashTable[h];
    while (l != NULL) {
        if (strcmp(name, l->name) != 0)
            l = l->next;
        else {
            if ((l->node->loc <= t->loc) && (l->node->scope == t->scope)) {
                printf("error!\n");
                exit(-1);
            }
            l = l->next;
        }
    }
    // while ((l != NULL) && (strcmp(name,l->name) != 0))
    //  l = l->next;
    if (l == NULL) /* variable not yet in table */
    {
        l = (BucketList)malloc(sizeof(struct BucketListRec));
        l->name = copyString(name);
        l->lines = (LineList)malloc(sizeof(struct LineListRec));
        l->lines->lineno = lineno;

        l->node = t;

        l->next = hashTable[h];
        hashTable[h] = l;
    } else {
        printf("error!\n");
        exit(-1);
    }
} /* st_insert */

int get_global_var_num(void) {
    int num = 0;
    int i = 0;
    for (i = 0; i < SIZE; i++) {
        BucketList l = hashTable[i];
        if (l != NULL) {
            /* If global and not function, num ++ */
            if ((l->node->scope == 0) && (l->node->kind.stmt != FuncDecK))
                num++;
        }
    }
    return num;
}

TreeNode *st_func_lookup(char *name) {
    int h = hash(name);
    BucketList l = hashTable[h];

    while (l != NULL) {
        if (strcmp(name, l->name) == 0)
            return l->node;
        l = l->next;
    }
    return NULL;
}

/* Function st_lookup returns the memory
 * location of a variable or -1 if not found
 */
BucketList st_lookup(char *name, TreeNode *t) {
    int scopeCheck = TRUE;
    int found = FALSE;
    int h = hash(name);
    BucketList l = hashTable[h];

    while (l != NULL) {
        if (strcmp(name, l->name) != 0)
            l = l->next;
        else {
            /* global variable */
            if ((isDeclaration(t) == FALSE) && l->node->scope == 0) {
                found = TRUE;
                break;
            }
            if ((l->node->loc <= t->loc) && (l->node->scope == t->scope)) {
                found = TRUE;
                break;
            }
            l = l->next;
        }
    }

    if ((t->nodekind == ExpK) && (t->kind.exp == CallK)) {
        l = hashTable[h];

        while ((l != NULL) && (strcmp(name, l->name) != 0))
            l = l->next;

        if (l == NULL)
            return NULL;
        else
            return l;
    }

    if ((found == TRUE) && (l != NULL))
        return l;
    else
        return NULL;
}

/* Procedure printSymTab prints a formatted
 * listing of the symbol table contents
 * to the listing file
 */
void printSymTab(FILE *listing) {
    int i;
    fprintf(listing, "ID Name    Type              location scope line numbers\n");
    fprintf(listing, "---------- ----------------- -------- ----- ------------\n");
    for (i = 0; i < SIZE; ++i) {
        if (hashTable[i] != NULL) {
            BucketList l = hashTable[i];
            while (l != NULL) {
                TreeNode *t = l->node;
                LineList lines = l->lines;
                if (t == NULL)
                    exit(-1);
                fprintf(listing, " %-9s ", l->name);
                if (t->type == Integer)
                    fprintf(listing, " int");
                else if (t->type == Void)
                    fprintf(listing, " void");
                else if (t->type == IntegerArray)
                    fprintf(listing, " array");

                if (t->nodekind == StmtK) {
                    if (t->kind.stmt == FuncDecK)
                        fprintf(listing, "(function)\t ");
                    else if (
                        (t->kind.stmt == VarDecK) || (t->kind.stmt == ArrayDecK) ||
                        (t->kind.stmt == ParamK))
                        fprintf(listing, "(variable)\t ");
                }
                fprintf(listing, "%d      ", t->loc);
                fprintf(listing, "%d    ", t->scope);
                while (lines != NULL) {
                    fprintf(listing, "%d ", lines->lineno);
                    lines = lines->next;
                }
                fprintf(listing, "\n");
                l = l->next;
            }
        }
    }
} /* printSymTab */
