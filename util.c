#include "globals.h"
#include "util.h"

/* Procedure printToken prints a token
 * and its lexeme to the listing file
 */

void printToken(TokenType token, const char *tokenString) {
    switch (token) {
    case ELSE:
    case IF:
    case INT:
    case RETURN:
    case VOID:
    case WHILE:
        fprintf(listing, "reserved word: %s\n", tokenString);
        break;
    case PLUS:
        fprintf(listing, "+\n");
        break;
    case MINUS:
        fprintf(listing, "-\n");
        break;
    case TIMES:
        fprintf(listing, "*\n");
        break;
    case OVER:
        fprintf(listing, "/\n");
        break;

    case LG:
        fprintf(listing, ">\n");
        break;
    case LE:
        fprintf(listing, ">=\n");
        break;
    case SM:
        fprintf(listing, "<\n");
        break;
    case SE:
        fprintf(listing, "<=\n");
        break;

    case EQ:
        fprintf(listing, "==\n");
        break;
    case NEQ:
        fprintf(listing, "!=\n");
        break;
    case ASSIGN:
        fprintf(listing, "=\n");
        break;
    case SEMI:
        fprintf(listing, ";\n");
        break;
    case COMMA:
        fprintf(listing, ",\n");
        break;

    case LPAREN:
        fprintf(listing, "(\n");
        break;
    case RPAREN:
        fprintf(listing, ")\n");
        break;
    case SLPAREN:
        fprintf(listing, "[\n");
        break;
    case SRPAREN:
        fprintf(listing, "]\n");
        break;
    case CLPAREN:
        fprintf(listing, "{\n");
        break;
    case CRPAREN:
        fprintf(listing, "}\n");
        break;
    case ENDFILE:
        fprintf(listing, "EOF\n");
        break;
    case NUM:
        fprintf(listing, "NUM, val = %s\n", tokenString);
        break;
    case ID:
        fprintf(listing, "ID, name = %s\n", tokenString);
        break;
    case ERROR:
        fprintf(listing, "ERROR, %s\n", tokenString);
        break;
    default: /* should never happen */
        fprintf(listing, "Unknown token: %d\n", token);
    }
}

/* Function newStmtNode allocates memmory for
 * a new node for syntax tree construction
 */
TreeNode *newNode(void) {
    TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
    int i = 0;

    if (t == NULL)
        fprintf(listing, "Out of memory error at line %d\n", lineno);
    else {
        for (i = 0; i < MAXCHILDREN; i++)
            t->child[i] = NULL;
        t->sibling = NULL;
        t->lineno = lineno;
        t->type = Void;
        t->attr.name = NULL;
        t->loc = 0;
        t->scope = 0;
    }
    return t;
}

/* Function newExpNode creates a new statement
 * node for syntax tree construction
 */
TreeNode *newStmtNode(StmtKind kind) {
    TreeNode *t = newNode();
    if (t != NULL) {
        t->nodekind = StmtK;
        t->kind.stmt = kind;
    }
    return t;
}

/* Function newExpNode creates a new expression
 * node for syntax tree construction
 */
TreeNode *newExpNode(ExpKind kind) {
    TreeNode *t = newNode();
    if (t != NULL) {
        t->nodekind = ExpK;
        t->kind.exp = kind;
    }
    return t;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char *copyString(char *s) {
    int n;
    char *t;
    if (s == NULL)
        return NULL;
    n = strlen(s) + 1;
    t = malloc(n);
    if (t == NULL)
        fprintf(listing, "Out of memory error at line %d\n", lineno);
    else
        strcpy(t, s);
    return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static int indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno += 2
#define UNINDENT indentno -= 2

/* printSpaces indents by printing spaces */
static void printSpaces(void) {
    int i;
    for (i = 0; i < indentno; i++)
        fprintf(listing, " ");
}

/* procedure printTree prints a syntax tree to the
 * listing file using indentation to indicate subtrees
 */
void printTree(TreeNode *tree) {
    int i;
    INDENT;
    while (tree != NULL) {
        printSpaces();
        if (tree->nodekind == StmtK) {
            switch (tree->kind.stmt) {
            case IfK:
                fprintf(listing, "If(condition, body, (optional)else)\n");
                break;
            case AssignK:
                fprintf(listing, "Assign (dest)(source):\n");
                break;
            case VarDecK:
                fprintf(
                    listing, "Var Dec(following const:array length(optional)): %s\n",
                    tree->attr.name);
                break;
            case ArrayDecK:
                fprintf(
                    listing, "Var Dec(following const:array length(optional)): %s\n",
                    tree->attr.name);
                break;
            case FuncDecK:
                if (tree->type == Integer)
                    fprintf(listing, "int Function Dec: %s\n", tree->attr.name);
                else if (tree->type == Void)
                    fprintf(listing, "void Function Dec: %s\n", tree->attr.name);
                break;
            case ParamK:
                if (tree->type == Void)
                    fprintf(listing, "Para: Void\n");
                else if (tree->type == Integer)
                    fprintf(listing, "Para: (int) %s\n", tree->attr.name);
                else if (tree->type == IntegerArray)
                    fprintf(listing, "Para: (int) %s (array)\n", tree->attr.name);
                break;
            case ComK:
                fprintf(listing, "Compound Stmt\n");
                break;
            case WhileK:
                fprintf(listing, "While\n");
                break;
            case ReturnK:
                fprintf(listing, "Return\n");
                break;
            default:
                fprintf(listing, "Unknown ExpNode kind\n");
                break;
            }
        } else if (tree->nodekind == ExpK) {
            switch (tree->kind.exp) {
            case OpK:
                fprintf(listing, "Op: ");
                printToken(tree->attr.op, "\0");
                break;
            case ConstK:
                fprintf(listing, "Const: %d\n", tree->attr.val);
                break;
            case IdK:
                fprintf(listing, "Id: %s\n", tree->attr.name);
                break;
            case IdArrayK:
                fprintf(listing, "Id: %s(following: array index)\n", tree->attr.name);
                break;
            case CallK:
                fprintf(listing, "Call(followings are args): %s\n", tree->attr.name);
                break;
            default:
                fprintf(listing, "Unknown ExpNode kind\n");
                break;
            }
        } else
            fprintf(listing, "Unknown node kind\n");
        for (i = 0; i < MAXCHILDREN; i++)
            printTree(tree->child[i]);
        tree = tree->sibling;
    }
    UNINDENT;
}
