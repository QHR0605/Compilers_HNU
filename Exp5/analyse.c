#include "analyse.h"
#include "globals.h"
#include "symtab.h"
#include "util.h"

static void traverse(TreeNode *t, void (*preProc)(TreeNode *), void (*postProc)(TreeNode *));
static void checkNode(TreeNode *t);
static void insertNode(TreeNode *t);
static void nullProc(TreeNode *t);
void insertGlobals(TreeNode *tree);

static void baseFunc(void);

void buildsymtab(TreeNode *t) {
    if (TraceAnalyse) {
        fprintf(listing, "Scope     Identifier        Line   paramters Symbol type\n");
    }

    baseFunc();
    insertNode(t);
    insertGlobals(t);

    if (TraceAnalyse) {
        fprintf(listing, "Function: GLOBALS\n");
        out_scope();
    }
}

void typeCheck(TreeNode *t) {
    traverse(t, nullProc, checkNode);
}

static void baseFunc(void) {
    TreeNode *input;
    TreeNode *output;
    TreeNode *arg;

    input = newDecNode(FuncDecK);
    input->name = copyString("input");
    input->functionReturnType = Integer;
    input->expressionType = Function;

    arg = newDecNode(IdDecK);
    arg->name = copyString("arg");
    arg->variableDataType = Integer;
    arg->expressionType = Integer;

    output = newDecNode(FuncDecK);
    output->name = copyString("output");
    output->functionReturnType = Void;
    output->expressionType = Function;
    output->child[0] = arg;

    st_insert("input", input, 0);
    st_insert("output", output, 0);
}

static void insertNode(TreeNode *t) {
    int i;
    BucketListPtr l;
    static TreeNode *returnfunc = NULL;

    if (t != NULL) {
        if (t->nodekind == DecK) {
            l = st_lookup(t->name);
            if (l != NULL) {
                fprintf(listing, "line %d: error: redeclaration of '%s'\n",
                        t->lineno, t->name);
                Error = TRUE;
            }else {
                st_insert(t->name, t, t->lineno);
                if (t->kind.dec == FuncDecK) {
                    returnfunc = t;

                    if (TraceAnalyse)
                        fprintf(listing, "Function: %s\n", t->name);

                    new_scope();
                    ++scopeDepth;
                }
            }
        }

        if ((t->nodekind == StmtK) && (t->kind.stmt == CompoundK)) {
            new_scope();
            ++scopeDepth;
        }
        
        if (((t->nodekind == ExpK) && (t->kind.exp == IdK)) ||
            ((t->nodekind == StmtK) && (t->kind.stmt == CallK))) {
            l = st_lookup(t->name);
            if (l == NULL) {
                fprintf(listing, "line %d: error: '%s' was not declared in this scope\n",
                        t->lineno, t->name);
                Error = TRUE;
            } else {
                t->declaration = l->declaration;
            }
        }

        if ((t->nodekind == StmtK) && (t->kind.stmt == ReturnK)) {
            t->declaration = returnfunc;
        }

        for (i = 0; i < MAXCHILDREN; ++i)
            insertNode(t->child[i]);

        if (((t->nodekind == DecK) && (t->kind.dec == FuncDecK)) ||
            ((t->nodekind == StmtK) && (t->kind.stmt == CompoundK))) {

            if (TraceAnalyse)
                out_scope();

            --scopeDepth;
            end_scope();
        }

        insertNode(t->sibling);
    }
}

static void checkNode(TreeNode *t) {
    switch (t->nodekind) {
    case DecK:
        switch (t->kind.dec) {
        case IdDecK:
            t->expressionType = t->variableDataType;
            break;
        case ArrayDecK:
            t->expressionType = Array;
            break;
        case FuncDecK:
            t->expressionType = Function;
            break;
        }
        break;
    case StmtK:
        switch (t->kind.stmt) {
        case IfK:
            if (t->child[0]->expressionType != Integer) {
                fprintf(listing, "line %d: error: '%s' should be integer\n",
                        t->lineno, t->child[0]->name);
                Error = TRUE;
            }
            break;
        case WhileK:
            if (t->child[0]->expressionType != Integer) {
                fprintf(listing, "line %d: error: '%s' should be integer\n",
                        t->child[0]->lineno, t->child[0]->name);
                Error = TRUE;
            }
            break;
        case CallK: {
            int flag = 0;
            TreeNode *decl;
            TreeNode *node;
            decl = t->declaration->child[0];
            node = t->child[0];

            while ((decl != NULL) && (node != NULL)) {
                if (decl->expressionType != node->expressionType) {
                    flag = FALSE;
                    break;
                }
                if (decl)
                    decl = decl->sibling;
                if (node)
                    node = node->sibling;
            }
            if (((decl == NULL) && (node != NULL)) || ((decl != NULL) && (node == NULL)))
                flag = FALSE;
            else
                flag = TRUE;

            if (!flag) {
                fprintf(listing, "line %d: error: Function '%s' don't match with define '%s'\n",
                        t->lineno, t->declaration->name, t->name);
                Error = TRUE;
            }
            t->expressionType = t->declaration->functionReturnType;
            break;
        }
        case ReturnK:
            if (t->declaration->functionReturnType == Integer) {
                if ((t->child[0] == NULL) || (t->child[0]->expressionType != Integer)) {
                    fprintf(listing, "line %d: error: in function %s should return integer\n",
                            t->lineno, t->declaration->name);
                    Error = TRUE;
                }
            } else if (t->declaration->functionReturnType == Void) {
                if (t->child[0] != NULL) {
                    fprintf(listing, "line %d: error: in function %s should return void\n",
                            t->lineno, t->declaration->name);
                }
            }
            break;
        case CompoundK:
            t->expressionType = Void;
            break;
        }
        break;
    case ExpK:
        switch (t->kind.exp) {
        case AssignK:
        case OpK:
            if ((t->child[0]->expressionType == Integer) &&
                (t->child[1]->expressionType == Integer))
                t->expressionType = Integer;
            else {
                fprintf(listing, "line %d: error: '%s' and '%s' shoule all be integer\n",
                        t->lineno, t->child[0]->name, t->child[1]->name);
                Error = TRUE;
            }
            break;
        case IdK:
            if (t->declaration->expressionType == Integer) {
                if (t->child[0] == NULL)
                    t->expressionType = Integer;
                else {
                    fprintf(listing, "line %d: error: '%s' is not a array\n",
                            t->lineno, t->name);
                    Error = TRUE;
                }
            } else if (t->declaration->expressionType == Array) {
                if (t->child[0] == NULL)
                    t->expressionType = Array;
                else {
                    if (t->child[0]->expressionType == Integer)
                        t->expressionType = Integer;
                    else {
                        fprintf(listing, "line %d: error: the elements '%s' of array '%s' should be integer\n",
                                t->lineno, t->child[0]->name, t->name);
                        Error = TRUE;
                    }
                }
            } else {
                fprintf(listing, "line %d: type error\n", t->lineno);
                Error = TRUE;
            }
            break;
        case ConstK:
            t->expressionType = Integer;
            break;
        }
        break;
    }
}

void insertGlobals(TreeNode *t) {
    if (t != NULL) {
        if ((t->nodekind == DecK) && ((t->kind.dec == IdDecK) || (t->kind.dec == ArrayDecK))) {
            t->isGlobal = TRUE;
        }
        insertGlobals(t->sibling);
    }
}

static void traverse(TreeNode *t, void (*preProc)(TreeNode *), void (*postProc)(TreeNode *)) {
    if (t != NULL) {
        preProc(t);
        int i;
        for (i = 0; i < MAXCHILDREN; ++i)
            traverse(t->child[i], preProc, postProc);
        postProc(t);
        traverse(t->sibling, preProc, postProc);
    }
}

static void nullProc(TreeNode *t) {
    if (t == NULL)
        return;
    else
        return;
}
