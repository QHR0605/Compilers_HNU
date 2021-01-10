#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static TokenType token; /* holds current token */

/* function prototypes for recursive calls */
static TreeNode *declaration_list(void);
static TreeNode *declaration(void);
static TreeNode *var_declaration(void);
static TreeNode *params(void);
static TreeNode *param_list(void);
static TreeNode *param(void);
static TreeNode *compound_stmt(void);
static TreeNode *local_declarations(void);
static TreeNode *statement_list(void);
static TreeNode *statement(void);
static TreeNode *expression_stmt(void);
static TreeNode *compound_stmt(void);
static TreeNode *selection_stmt(void);
static TreeNode *iteration_stmt(void);
static TreeNode *return_stmt(void);
static TreeNode *expression(void);
static TreeNode *simple_expression(void);
static TreeNode *additive_expression(void);
static TreeNode *term(void);
static TreeNode *factor(void);
static TreeNode *args(void);
static TreeNode *arg_list(void);

/* assistant functions */
static void unexpectedTokenHandling(void);

static void syntaxError(char *message) {
    fprintf(listing, "\n>>> ");
    fprintf(listing, "Syntax error at line %d: %s", lineno, message);
    Error = TRUE;
}

static void match(TokenType expected) {
    if (token == expected)
        token = getToken();
    else {
        syntaxError("unexpected token -> ");
        printToken(token, tokenString);
        fprintf(listing, "      ");
    }
}

static void unexpectedTokenHandling(void) {
    syntaxError("unexpected token -> ");
    printToken(token, tokenString);
    fprintf(listing, "\n");
    token = getToken();
}

int isOp(TokenType token) {
    if (isRelop(token) || isAddop(token) || isMulop(token))
        return TRUE;
    else
        return FALSE;
}

int isRelop(TokenType token) {
    if ((token == LG) || (token == LE) || (token == SM) || (token == SE) || (token == EQ) ||
        (token == NEQ))
        return TRUE;
    else
        return FALSE;
}

int isAddop(TokenType token) {
    if ((token == PLUS) || (token == MINUS))
        return TRUE;
    else
        return FALSE;
}

int isMulop(TokenType token) {
    if ((token == TIMES) || (token == OVER))
        return TRUE;
    else
        return FALSE;
}

/* EBNF: declaration-list -> declaration {declaration} */
static TreeNode *declaration_list(void) {
    TreeNode *t = declaration();
    TreeNode *p = t;
    TreeNode *newNode = NULL;

    /* {declaration} */
    while (token != ENDFILE) {
        newNode = declaration();
        if ((newNode != NULL) && (p != NULL)) {
            p->sibling = newNode;
            p = newNode;
        }
    }
    return t;
}

/* EBNF: declaration -> var-declaration | func-declaration
 * both declaration types begin with 'type-specifier ID'
 * type-specifier -> int | void */
static TreeNode *declaration(void) {
    TreeNode *t = NULL;
    TreeNode *newNode = NULL;
    char *name = NULL;

    /* type-specifier */
    if (token == VOID) {
        t = newStmtNode(VarDecK);
        if (t != NULL)
            t->type = Void;
        match(VOID);
    } else if (token == INT) {
        t = newStmtNode(VarDecK);
        if (t != NULL)
            t->type = Integer;
        match(INT);
    } else
        return NULL;

    /* ID */
    if (token == ID) {
        if (t != NULL)
            t->attr.name = copyString(tokenString);
        match(ID);
    } else
        return NULL;

    /* var-declaration -> type-specifier ID";" */
    if (token == SEMI) {
        if (t != NULL)
            t->kind.stmt = VarDecK;
        match(SEMI);
    }
    /* var-declaration -> type-specifier ID "["NUM"]"";" */
    else if (token == SLPAREN) {
        if (t != NULL) {
            t->kind.stmt = ArrayDecK;
            t->type = IntegerArray;
        }
        match(SLPAREN);
        newNode = newExpNode(ConstK);
        if ((newNode != NULL) && (t != NULL)) {
            newNode->attr.val = atoi(tokenString);
            t->child[0] = newNode;
        }
        match(NUM);
        match(SRPAREN);
        match(SEMI);
    }
    /* func-declaration ->
     * type-specifier ID "("params")" compound-stmt */
    else if (token == LPAREN) {
        if (t != NULL) {
            t->kind.stmt = FuncDecK;
            match(LPAREN);
            t->child[0] = params();
            match(RPAREN);
            t->child[1] = compound_stmt();
        }
    } else
        unexpectedTokenHandling();

    return t;
}

/* var-declaration -> type-specifier ID ["["NUM"]"]";" */
static TreeNode *var_declaration(void) {
    TreeNode *t = NULL;
    TreeNode *newNode = NULL;

    /* type-specifier */
    switch (token) {
    case VOID:
        t = newStmtNode(VarDecK);
        if (t != NULL)
            t->type = Void;
        match(VOID);
        break;
    case INT:
        t = newStmtNode(VarDecK);
        if (t != NULL)
            t->type = Integer;
        match(INT);
        break;
    default:
        unexpectedTokenHandling();
        break;
    }

    /* ID */
    switch (token) {
    case ID:
        if (t != NULL)
            t->attr.name = copyString(tokenString);
        match(ID);
        break;
    default:
        unexpectedTokenHandling();
        break;
    }

    switch (token) {
    /* var-declaration -> type-specifier ID";" */
    case SEMI:
        match(SEMI);
        break;
    /* var-declaration -> type-specifier ID "["NUM"]"";" */
    case SLPAREN:
        t->kind.stmt = ArrayDecK;
        match(SLPAREN);
        newNode = newExpNode(ConstK);
        if ((t != NULL) && (newNode != NULL)) {
            newNode->attr.val = atoi(tokenString);
            t->child[0] = newNode;
        }
        match(NUM);
        match(SRPAREN);
        match(SEMI);
        break;
    default:
        unexpectedTokenHandling();
        break;
    }
    return t;
}

/* EBNF: params -> param-list | void */
static TreeNode *params(void) {
    TreeNode *t = NULL;

    if (token == VOID) {
        t = newStmtNode(ParamK);
        t->attr.name = NULL;
        t->type = Void;
        match(VOID);
    } else
        t = param_list();
    return t;
}

/* EBNF: param-list -> param {"," param} */
static TreeNode *param_list(void) {
    TreeNode *t = param();
    TreeNode *p = t;
    TreeNode *newNode = NULL;

    /* {"," param} */
    while ((t != NULL) && (token == COMMA)) {
        match(COMMA);
        newNode = param();
        if (newNode != NULL) {
            p->sibling = newNode;
            p = newNode;
        }
    }
    return t;
}

/* EBNF: param -> type-specifier ID ["[""]"] */
static TreeNode *param(void) {
    TreeNode *t = NULL;
    t = newStmtNode(ParamK);

    /* type-specifier */
    if (token == INT) {
        t->type = Integer;
        match(INT);
    } else if (token == VOID) {
        t->type = Void;
        match(VOID);
    } else
        unexpectedTokenHandling();

    /* ID */
    if ((t != NULL) && (token == ID)) {
        t->attr.name = copyString(tokenString);
        match(ID);
    }

    /* ["[""]"]*/
    if (token == SLPAREN) {
        match(SLPAREN);
        match(SRPAREN);
        if (t->type == Integer)
            t->type = IntegerArray;
    }
    return t;
}

/* EBNF: compound-stmt -> "{"local-declarations statement-list"}" */
static TreeNode *compound_stmt(void) {
    TreeNode *t = NULL;

    if (token == CLPAREN) /* "{" */
    {
        t = newStmtNode(ComK);
        match(CLPAREN);
        if (t != NULL) {
            t->child[0] = local_declarations();
            t->child[1] = statement_list();
        }
        match(CRPAREN);
    } else
        unexpectedTokenHandling();

    return t;
}

/* EBNF: local-declarations ->
 * var-declaration {var-declaration} | empty */
static TreeNode *local_declarations(void) {
    TreeNode *t = NULL;
    TreeNode *p = NULL;
    TreeNode *newNode = NULL;

    if ((token == INT) || (token == VOID)) {
        t = var_declaration();
        p = t;
    } else /* empty */
        return NULL;

    /* {var-declarations} */
    while ((token == INT) || (token == VOID)) {
        newNode = var_declaration();
        if ((p != NULL) && (newNode != NULL)) {
            p->sibling = newNode;
            p = newNode;
        }
    }
    return t;
}

/* EBNF: statement-list -> statement {statement} | empty */
static TreeNode *statement_list(void) {
    TreeNode *t = NULL;
    TreeNode *p = NULL;
    TreeNode *newNode = NULL;

    if (token != CRPAREN) {
        t = statement();
        p = t;
    } else /* empty */
        return NULL;

    /* {statement} */
    while (token != CRPAREN) {
        newNode = statement();
        if ((p != NULL) && (newNode != NULL)) {
            p->sibling = newNode;
            p = newNode;
        }
    }
    return t;
}

/* EBNF: statment -> expression-stmt | compound-stmt |
 * selection-stmt | iteration-stmt | return-stmt */
static TreeNode *statement(void) {
    TreeNode *t = NULL;

    switch (token) {
    /* First(expression-stmt)={ID,LPAREN,NUM} */
    case ID:
    case LPAREN:
    case NUM:
        t = expression_stmt();
        break;
    case CLPAREN:
        t = compound_stmt();
        break;
    case IF:
        t = selection_stmt();
        break;
    case WHILE:
        t = iteration_stmt();
        break;
    case RETURN:
        t = return_stmt();
        break;
    default:
        unexpectedTokenHandling();
        break;
    }
    return t;
}

/* EBNF: expression-stmt -> expression";" | ";"*/
static TreeNode *expression_stmt(void) {
    TreeNode *t = NULL;

    if (token == SEMI)
        match(SEMI);
    else if (token != CRPAREN) {
        t = expression();
        match(SEMI);
    }
    return t;
}

/* selection-stmt ->
 * if (expression) statement [else statement]*/
static TreeNode *selection_stmt(void) {
    TreeNode *t = NULL;

    if (token == IF) {
        t = newStmtNode(IfK);
        match(IF);
        match(LPAREN);
        if (t != NULL)
            t->child[0] = expression();
        match(RPAREN);
        if (t != NULL)
            t->child[1] = statement();
        /* [else statement] */
        if (token == ELSE) {
            match(ELSE);
            if (t != NULL)
                t->child[2] = statement();
        }
    }
    return t;
}

/* iteration-stmt -> while (expression) statement */
static TreeNode *iteration_stmt(void) {
    TreeNode *t = NULL;
    TreeNode *cond_exp = NULL;
    TreeNode *iter_stmt = NULL;

    if (token == WHILE) {
        t = newStmtNode(WhileK);
        match(WHILE);
        match(LPAREN);
        cond_exp = expression();
        match(RPAREN);
        iter_stmt = statement();
    }
    if (t != NULL) {
        t->child[0] = cond_exp;
        t->child[1] = iter_stmt;
    }
    return t;
}

/* return-stmt -> return [expression]";" */
static TreeNode *return_stmt(void) {
    TreeNode *t = NULL;

    if (token == RETURN) {
        match(RETURN);
        t = newStmtNode(ReturnK);
    }

    /* [expression] */
    if (token != SEMI)
        t->child[0] = expression();

    match(SEMI);
    return t;
}

/* expression -> var "=" expression | simple-expression
 * var -> ID ["["expression"]"]*/
static TreeNode *expression(void) {
    TreeNode *t = newExpNode(IdK);
    TreeNode *p = NULL;
    TreeNode *newNode = NULL;

    if (token == ID) {
        if (t != NULL)
            t->attr.name = copyString(tokenString);
        match(ID);

        /* ID "("args")" */
        if (token == LPAREN) {
            match(LPAREN);
            if (t != NULL) {
                t->nodekind = ExpK;
                t->kind.exp = CallK;
                t->child[0] = args();
            }
            match(RPAREN);
            return t;
        }
        /* ID "["expression"]" */
        else if (token == SLPAREN) {
            t->kind.exp = IdArrayK;
            match(SLPAREN);
            t->child[0] = expression();
            match(SRPAREN);
            /* ID "["expression"]" "=" expression */
            if (token == ASSIGN) {
                match(ASSIGN);
                newNode = newStmtNode(AssignK);
                if ((newNode != NULL) && (t != NULL)) {
                    newNode->child[0] = t;
                    t = newNode;
                    t->child[1] = expression();
                }
            }
            /* ID "["expression"]" op additive-expression */
            else if (isOp(token)) {
                newNode = newExpNode(OpK);
                if (newNode != NULL) {
                    newNode->child[0] = t;
                    newNode->attr.op = token;
                    t = newNode;
                }
                match(token);
                if (t != NULL)
                    t->child[1] = additive_expression();
            } else
                return t;
        }
        /* ID "=" expression */
        else if (token == ASSIGN) {
            match(ASSIGN);
            newNode = newStmtNode(AssignK);
            if ((newNode != NULL) && (t != NULL)) {
                newNode->child[0] = t;
                t = newNode;
                t->child[1] = expression();
            }
        }
        /* ID op additive-expression */
        else if (isOp(token)) {
            newNode = newExpNode(OpK);
            if (newNode != NULL) {
                newNode->child[0] = t;
                newNode->attr.op = token;
                t = newNode;
            }
            match(token);
            if (t != NULL)
                t->child[1] = additive_expression();
        } else if ((token != SEMI) && (token != COMMA) && (token != RPAREN) && (token != SRPAREN))
            unexpectedTokenHandling();
    } else if ((token == LPAREN) || (token == NUM))
        t = simple_expression();
    else
        unexpectedTokenHandling();

    return t;
}

/* EBNF: simple-expression ->
 * addtivite-expression [relop additivie-expression] */
static TreeNode *simple_expression(void) {
    TreeNode *t = additive_expression();
    TreeNode *newNode;

    /* [relop additive-expression]  */
    if (isRelop(token)) {
        newNode = newExpNode(OpK);
        if (newNode != NULL) {
            newNode->child[0] = t;
            newNode->attr.op = token;
            t = newNode;
        }
        match(token); // match relop
        if (t != NULL)
            t->child[1] = additive_expression();
    }
    return t;
}

/* EBNF: additive-expression -> term {addop term} */
static TreeNode *additive_expression(void) {
    TreeNode *t = term();
    TreeNode *newNode;

    /* {addop term} */
    while (isAddop(token)) {
        newNode = newExpNode(OpK);
        if (newNode != NULL) {
            newNode->child[0] = t;
            newNode->attr.op = token;
            t = newNode;
        }
        match(token); // match addop
        if (t != NULL)
            t->child[1] = term();
    }
    return t;
}

/* EBNF: term -> factor {mulop factor} */
static TreeNode *term(void) {
    TreeNode *t = factor();
    TreeNode *newNode;

    /* {mulop factor} */
    while (isMulop(token)) {
        newNode = newExpNode(OpK);
        if (newNode != NULL) {
            newNode->child[0] = t;
            newNode->attr.op = token;
            t = newNode;
        }
        match(token); // match mulop
        if (t != NULL)
            t->child[1] = factor();
    }
    return t;
}

/* EBNF: factor -> "("expression")" | var | call | NUM */
static TreeNode *factor(void) {
    TreeNode *t;
    TreeNode *p;

    switch (token) {
    case LPAREN: /* "("expression")" */
        match(LPAREN);
        t = expression();
        match(RPAREN);
        break;
    case ID: /* var | call */
        t = newExpNode(IdK);
        if ((t != NULL) && (token == ID))
            t->attr.name = copyString(tokenString);
        match(ID);

        /* call -> ID"("args")" */
        if (token == LPAREN) {
            p = newExpNode(CallK);
            p->attr.name = copyString(t->attr.name);
            t = p;
            match(LPAREN);
            t->child[0] = args();
            match(RPAREN);
        }
        /* var -> ID"["expression"]" */
        else if (token == SLPAREN) {
            p = newExpNode(IdK);
            p->attr.name = t->attr.name;
            t = p;
            match(SLPAREN);
            t->child[0] = expression();
            match(SRPAREN);
        }
        break;
    case NUM: /* NUM */
        t = newExpNode(ConstK);
        if ((t != NULL) && (token == NUM))
            t->attr.val = atoi(tokenString);
        match(NUM);
        break;
    default:
        unexpectedTokenHandling();
        break;
    }
    return t;
}

/* EBNF: args -> arg-list | empty */
static TreeNode *args(void) {
    TreeNode *t = NULL;

    if (token != RPAREN)
        t = arg_list();
    return t;
}

/* EBNF: arg-list -> expression {, expression} */
static TreeNode *arg_list(void) {
    TreeNode *t = expression();
    TreeNode *p = t;
    TreeNode *newNode = NULL;

    /* {, expression} */
    while (token == COMMA) {
        match(COMMA);
        newNode = expression();
        if (p != NULL) {
            p->sibling = newNode;
            p = newNode;
        }
    }
    return t;
}

/* construct the parse tree. */
TreeNode *parse(void) {
    TreeNode *t;
    token = getToken();
    t = declaration_list();

    if (token != ENDFILE)
        syntaxError("Code ends before file\n");

    return t;
}
