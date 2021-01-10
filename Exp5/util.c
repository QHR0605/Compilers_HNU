#include "globals.h"
#include "util.h"

#include <string.h>

static TreeNode *allocNewNode(void);

void printToken(TokenType token, const char *lexeme) {
    switch (token) {
    case IF:
    case ELSE:
    case INT:
    case RETURN:
    case VOID:
    case WHILE:
        fprintf(listing, "reserved word \"%s\"", lexeme);
        break;
    case PLUS:
        fprintf(listing, "+");
        break;
    case MINUS:
        fprintf(listing, "-");
        break;
    case TIMES:
        fprintf(listing, "*");
        break;
    case DIVIDE:
        fprintf(listing, "/");
        break;
    case LT:
        fprintf(listing, "<");
        break;
    case GT:
        fprintf(listing, ">");
        break;
    case ASSIGN:
        fprintf(listing, "=");
        break;
    case NE:
        fprintf(listing, "!=");
        break;
    case SEMI:
        fprintf(listing, ";");
        break;
    case COMMA:
        fprintf(listing, ",");
        break;
    case LPAREN:
        fprintf(listing, "(");
        break;
    case RPAREN:
        fprintf(listing, ")");
        break;
    case LBRACE:
        fprintf(listing, "{");
        break;
    case RBRACE:
        fprintf(listing, "}");
        break;
    case LSQUARE:
        fprintf(listing, "[");
        break;
    case RSQUARE:
        fprintf(listing, "]");
        break;
    case LTE:
        fprintf(listing, "<=");
        break;
    case GTE:
        fprintf(listing, ">=");
        break;
    case EQ:
        fprintf(listing, "==");
        break;
    case NUM:
        fprintf(listing, "NUM, value = %s", lexeme);
        break;
    case ID:
        fprintf(listing, "ID, name = \"%s\"", lexeme);
        break;
    case ENDOFFILE:
        fprintf(listing, "EOF");
        break;
    case ERROR:
        fprintf(listing, "ERROR %s", lexeme);
        break;
    default: /* Should never happen. */
        fprintf(listing, "UNKNOWN TOKEN %d", token);
    }
}

static int indentno = 0;

#define INDENT indentno += 4
#define UNINDENT indentno -= 4

static void printSpaces(void) {
    int i;

    for (i = 0; i < indentno; ++i)
        fprintf(listing, " ");
}

char *typeName(ExpType type) {
    static char i[] = "integer";
    static char v[] = "void";
    static char invalid[] = "<<invalid type>>";

    switch (type) {
    case Integer:
        return i;
        break;
    case Void:
        return v;
        break;
    default:
        return invalid;
    }
}

void printTree(TreeNode *tree) {
    int i;

    INDENT;

    while (tree != NULL)
    {
        printSpaces();

        if (tree->nodekind == DecK) {
            switch (tree->kind.dec) {
            case IdDecK:
                fprintf(
                    listing, "[Scalar declaration \"%s\" of type \"%s\"]\n", tree->name,
                    typeName(tree->variableDataType));
                break;
            case ArrayDecK:
                fprintf(
                    listing,
                    "[Array declaration \"%s\" of size %d"
                    " and type \"%s\"]\n",
                    tree->name, tree->val, typeName(tree->variableDataType));
                break;
            case FuncDecK:
                fprintf(
                    listing,
                    "[Function declaration \"%s()\""
                    " of return type \"%s\"]\n",
                    tree->name, typeName(tree->functionReturnType));
                break;
            default:
                fprintf(listing, "<<<unknown declaration type>>>\n");
                break;
            }
        } else if (tree->nodekind == ExpK) {
            switch (tree->kind.exp) {
            case OpK:
                fprintf(listing, "[Operator \"");
                printToken(tree->op, "");
                fprintf(listing, "\"]\n");
                break;
            case IdK:
                fprintf(listing, "[Identifier \"%s", tree->name);
                if (tree->val != 0) /* array indexing */
                    fprintf(listing, "[%d]", tree->val);
                fprintf(listing, "\"]\n");
                break;
            case ConstK:
                fprintf(listing, "[Literal constant \"%d\"]\n", tree->val);
                break;
            case AssignK:
                fprintf(listing, "[Assignment]\n");
                break;
            default:
                fprintf(listing, "<<<unknown expression type>>>\n");
                break;
            }
        } else if (tree->nodekind == StmtK) {
            switch (tree->kind.stmt) {
            case CompoundK:
                fprintf(listing, "[Compound statement]\n");
                break;
            case IfK:
                fprintf(listing, "[IF statement]\n");
                break;
            case WhileK:
                fprintf(listing, "[WHILE statement]\n");
                break;
            case ReturnK:
                fprintf(listing, "[RETURN statement]\n");
                break;
            case CallK:
                fprintf(listing, "[Call to function \"%s()\"]\n", tree->name);
                break;
            default:
                fprintf(listing, "<<<unknown statement type>>>\n");
                break;
            }
        } else
            fprintf(listing, "<<<unknown node kind>>>\n");

        for (i = 0; i < MAXCHILDREN; ++i)
            printTree(tree->child[i]);

        tree = tree->sibling;
    }

    UNINDENT;
}

TreeNode *newStmtNode(StmtKind kind) {
    TreeNode *t;

    t = allocNewNode();
    if (t) {
        t->nodekind = StmtK;
        t->kind.stmt = kind;
    }

    return t;
}

TreeNode *newExpNode(ExpKind kind) {
    TreeNode *t;

    t = allocNewNode();
    if (t) {
        t->nodekind = ExpK;
        t->kind.exp = kind;
    }

    return t;
}

TreeNode *newDecNode(DecKind kind) {
    TreeNode *t;

    t = allocNewNode();
    if (t) {
        t->nodekind = DecK;
        t->kind.dec = kind;
    }

    return t;
}

static TreeNode *allocNewNode(void) {
    TreeNode *t;
    int i;

    t = (TreeNode *)malloc(sizeof(TreeNode));
    if (!t) {
        fprintf(listing, "*** Out of memory at line %d.\n", lineno);
    } else {
        for (i = 0; i < MAXCHILDREN; ++i)
            t->child[i] = NULL;
        t->sibling = NULL;
        t->lineno = lineno;
        t->declaration = NULL;
        t->isParameter = FALSE;
        t->isGlobal = FALSE;
        t->assemblyAreaSize = 0;
        t->localSize = 0;
        t->offset = 0;
    }

    return t;
}

char *copyString(char *source) {
    int sLength;
    char *newString;

    if (!source)
        return NULL;

    sLength = strlen(source) + 1;
    newString = (char *)malloc(sLength);

    if (!newString)
        fprintf(listing, "*** Out of memory on line %d.\n", lineno);
    else
        strcpy(newString, source);

    return newString;
}
