#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "globals.h"
#include "symtab.h"
#include "util.h"

#define MAXTABLESIZE 211
#define SHIFT 4
#define IDENT_LEN 12
#define NEW_SCOPE "A_NEW_SCOPE"


static BucketListPtr hashTable[MAXTABLESIZE];
static BucketListPtr hashStack;

extern int TraceAnalyse;
int scopeDepth;

static int hash(char *key);
static char *getType(TreeNode *node);
static void printScope(BucketListPtr l);

static int hash(char *key) {
    int temp = 0;
    int i = 0;
    while (key[i] != '\0') {
        temp = ((temp << SHIFT) + key[i]) % MAXTABLESIZE;
        ++i;
    }
    return temp;
}
 
void st_insert(char *name, TreeNode *symbol, int lineno) {
    int h = hash(name);
    BucketListPtr l = hashTable[h];
    l = (BucketList *)malloc(sizeof(BucketList));
    l->name = copyString(name);
    l->declaration = symbol;
    l->lineFirstReferenced = lineno;
    l->next = hashTable[h];
    hashTable[h] = l;

    l = (BucketList *)malloc(sizeof(BucketList));
    l->name = copyString(name);
    l->declaration = symbol;
    l->lineFirstReferenced = lineno;
    l->next = hashStack;
    hashStack = l;
}

BucketListPtr st_lookup(char *name) {
    int h = hash(name);
    BucketListPtr l = hashTable[h];
    int found = FALSE;

    while((l != NULL) && (strcmp(name, l->name) != 0))
        l = l->next;
    if (l == NULL)
        return NULL;
    else return l;
}

void new_scope() {
    BucketListPtr newNode;

    newNode = (BucketList *)malloc(sizeof(BucketList));
    newNode->name = copyString(NEW_SCOPE);
    newNode->declaration = NULL;
    newNode->lineFirstReferenced = 0;
    newNode->next = hashStack;
    hashStack = newNode;
}

void end_scope() {
    BucketListPtr l;
    BucketListPtr next;
    int h;
    while ((hashStack != NULL) && (strcmp(NEW_SCOPE, hashStack->name)) != 0) {
        h = hash(hashStack->name);
        l = hashTable[h];

        assert((hashStack != NULL) && (hashTable[h] != NULL));
        assert(strcmp(hashStack->name, l->name) == 0);

        next = hashTable[h]->next;
        free(hashTable[h]);
        hashTable[h] = next;

        next = hashStack->next;
        free(hashStack);
        hashStack = next;
    }
    assert(strcmp(hashStack->name, NEW_SCOPE) == 0);
    next = hashStack->next;
    free(hashStack);
    hashStack = next;
}

void out_scope() {
    BucketListPtr l;
    l = hashStack;
    if ((l != NULL) && (strcmp(NEW_SCOPE, l->name)))
        printScope(l);
}

static void printScope(BucketListPtr l) {
    char id[IDENT_LEN + 1];
    char *type;

    if ((l->next != NULL) && (strcmp(l->next->name, NEW_SCOPE) != 0))
        printScope(l->next);

    memset(id, ' ', IDENT_LEN);
    memmove(id, l->name, strlen(l->name));
    id[IDENT_LEN] = '\0';

    type = getType(l->declaration);
    fprintf(
        listing, "%3d        %s   %7d     %c    %s\n", scopeDepth, id,
        l->lineFirstReferenced, l->declaration->isParameter ? 'Y' : 'N', type);
    free(type);
}

static char *getType(TreeNode *node) {
    char stringBuffer[100];

    if ((node == NULL) || (node->nodekind != DecK))
        strcpy(stringBuffer, "ERROR");
    else {
        switch (node->kind.dec) {
        case IdDecK:
            sprintf(stringBuffer, "Identifier: %s", typeName(node->variableDataType));
            break;
        case ArrayDecK:
            sprintf(
                stringBuffer, "Array: %s elements: %d", typeName(node->variableDataType),
                node->val);
            break;
        case FuncDecK:
            sprintf(
                stringBuffer, "Function: return type %s", typeName(node->functionReturnType));
            break;
        default:
            strcpy(stringBuffer, "UNKNOWN");
            break;
        }
    }
    return copyString(stringBuffer);
}