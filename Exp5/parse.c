#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static TokenType token;

static TreeNode *declaration_list(void);
static TreeNode *declaration(void);
static TreeNode *var_declaration(void);
static TreeNode *param_list(void);
static TreeNode *param(void);

static TreeNode *compound_statement(void);
static TreeNode *local_declarations(void);
static TreeNode *statement_list(void);
static TreeNode *statement(void);
static TreeNode *expression_statement(void);
static TreeNode *selection_statement(void);
static TreeNode *iteration_statement(void);
static TreeNode *return_statement(void);
static TreeNode *expression(void);
static TreeNode *simple_expression(TreeNode *passdown);
static TreeNode *additive_expression(TreeNode *passdown);
static TreeNode *term(TreeNode *passdown);
static TreeNode *factor(TreeNode *passdown);
static TreeNode *args(void);
static TreeNode *arg_list(void);

static TreeNode *ident_statement(void);

static void syntaxError(char *message) {
    fprintf(listing, "Syntax error at line %d: %s", lineno, message);
    Error = TRUE;
}

static void match(TokenType expected) {
    if (token == expected)
        token = getToken();
    else {
        syntaxError("unexpected token ");
        printToken(token, tokenString);
        fprintf(listing, "\n");
    }
}

static ExpType matchType() {
    ExpType t_type = Void;
    switch (token) {
    case INT:
        t_type = Integer;
        token = getToken();
        break;
    case VOID:
        t_type = Void;
        token = getToken();
        break;
    default: {
        syntaxError("expected a type identifier but got a ");
        printToken(token, tokenString);
        fprintf(listing, "\n");
        break;
    }
    }
    return t_type;
}

static int isAType(TokenType tok) {
    if ((tok == INT) || (tok == VOID))
        return TRUE;
    else
        return FALSE;
}

static TreeNode *declaration_list(void) {
    TreeNode *tree;
    TreeNode *ptr;
    tree = declaration();
    ptr = tree;
    while (token != ENDOFFILE) {
        TreeNode *q;
        q = declaration();
        if ((q != NULL) && (ptr != NULL)) {
            ptr->sibling = q;
            ptr = q;
        }
    }
    return tree;
}

static TreeNode *declaration(void) {
    TreeNode *tree = NULL;
    ExpType decType;
    char *identifier;

    decType = matchType();
    identifier = copyString(tokenString);
    match(ID);

    switch (token) {
    case SEMI:
        tree = newDecNode(IdDecK);
        if (tree != NULL) {
            tree->variableDataType = decType;
            tree->name = identifier;
        }
        match(SEMI);
        break;
    case LSQUARE:
        tree = newDecNode(ArrayDecK);
        if (tree != NULL) {
            tree->variableDataType = decType;
            tree->name = identifier;
        }
        match(LSQUARE);
        if (tree != NULL)
            tree->val = atoi(tokenString);
        match(NUM);
        match(RSQUARE);
        match(SEMI);
        break;
    case LPAREN:
        tree = newDecNode(FuncDecK);
        if (tree != NULL) {
            tree->functionReturnType = decType;
            tree->name = identifier;
        }
        match(LPAREN);
        if (tree != NULL)
            tree->child[0] = param_list();
        match(RPAREN);
        if (tree != NULL)
            tree->child[1] = compound_statement();
        break;
    default:
        syntaxError("unexpected token ");
        printToken(token, tokenString);
        fprintf(listing, "\n");
        token = getToken();
        break;
    }
    return tree;
}

static TreeNode *var_declaration(void) {
    TreeNode *tree = NULL;
    ExpType decType;
    char *identifier;

    decType = matchType();
    identifier = copyString(tokenString);
    match(ID);

    switch (token) {
    case SEMI:
        tree = newDecNode(IdDecK);
        if (tree != NULL) {
            tree->variableDataType = decType;
            tree->name = identifier;
        }
        match(SEMI);
        break;
    case LSQUARE:
        tree = newDecNode(ArrayDecK);
        if (tree != NULL) {
            tree->variableDataType = decType;
            tree->name = identifier;
        }
        match(LSQUARE);
        if (tree != NULL)
            tree->val = atoi(tokenString);
        match(NUM);
        match(RSQUARE);
        match(SEMI);
        break;
    default:
        syntaxError("unexpected token ");
        printToken(token, tokenString);
        fprintf(listing, "\n");
        token = getToken();
        break;
    }
    return tree;
}

static TreeNode *param(void) {
    TreeNode *tree;
    ExpType parmType;
    char *identifier;

    parmType = matchType();
    identifier = copyString(tokenString);
    match(ID);

    if (token == LSQUARE) {
        match(LSQUARE);
        match(RSQUARE);
        tree = newDecNode(ArrayDecK);
    } else
        tree = newDecNode(IdDecK);
    if (tree != NULL) {
        tree->name = identifier;
        tree->val = 0;
        tree->variableDataType = parmType;
        tree->isParameter = TRUE;
    }
    return tree;
}

static TreeNode *param_list(void) {
    TreeNode *tree;
    TreeNode *ptr;
    TreeNode *newNode;

    if (token == VOID) {
        match(VOID);
        return NULL;
    }
    tree = param();
    ptr = tree;
    while ((tree != NULL) && (token == COMMA)) {
        match(COMMA);
        newNode = param();
        if (newNode != NULL) {
            ptr->sibling = newNode;
            ptr = newNode;
        }
    }
    return tree;
}

static TreeNode *compound_statement(void) {
    TreeNode *tree = NULL;
    match(LBRACE);
    if ((token != RBRACE) && (tree = newStmtNode(CompoundK))) {
        if (isAType(token))
            tree->child[0] = local_declarations();
        if (token != RBRACE)
            tree->child[1] = statement_list();
    }
    match(RBRACE);
    return tree;
}

static TreeNode *local_declarations(void) {
    TreeNode *tree;
    TreeNode *ptr;
    TreeNode *newNode;
    if (isAType(token))
        tree = var_declaration();
    if (tree != NULL) {
        ptr = tree;
        while (isAType(token)) {
            newNode = var_declaration();
            if (newNode != NULL) {
                ptr->sibling = newNode;
                ptr = newNode;
            }
        }
    }
    return tree;
}

static TreeNode *statement_list(void) {
    TreeNode *tree = NULL;
    TreeNode *ptr;
    TreeNode *newNode;

    if (token != RBRACE) {
        tree = statement();
        ptr = tree;
        while (token != RBRACE) {
            newNode = statement();
            if ((ptr != NULL) && (newNode != NULL)) {
                ptr->sibling = newNode;
                ptr = newNode;
            }
        }
    }

    return tree;
}

static TreeNode *statement(void) {
    TreeNode *tree = NULL;

    switch (token) {
    case IF:
        tree = selection_statement();
        break;
    case WHILE:
        tree = iteration_statement();
        break;
    case RETURN:
        tree = return_statement();
        break;
    case LBRACE:
        tree = compound_statement();
        break;
    case ID:
    case SEMI:
    case LPAREN:
    case NUM:
        tree = expression_statement();
        break;
    default:
        syntaxError("unexpected token ");
        printToken(token, tokenString);
        fprintf(listing, "\n");
        token = getToken();
        break;
    }
    return tree;
}

static TreeNode *expression_statement(void) {
    TreeNode *tree = NULL;
    if (token == SEMI)
        match(SEMI);
    else if (token != RBRACE) {
        tree = expression();
        match(SEMI);
    }
    return tree;
}

static TreeNode *selection_statement(void) {
    TreeNode *tree;
    TreeNode *expr;
    TreeNode *ifStmt;
    TreeNode *elseStmt = NULL;

    match(IF);
    match(LPAREN);
    expr = expression();
    match(RPAREN);
    ifStmt = statement();

    if (token == ELSE) {
        match(ELSE);
        elseStmt = statement();
    }
    tree = newStmtNode(IfK);
    if (tree != NULL) {
        tree->child[0] = expr;
        tree->child[1] = ifStmt;
        tree->child[2] = elseStmt;
    }
    return tree;
}

static TreeNode *iteration_statement(void) {
    TreeNode *tree;
    TreeNode *expr;
    TreeNode *stmt;

    match(WHILE);
    match(LPAREN);
    expr = expression();
    match(RPAREN);
    stmt = statement();
    tree = newStmtNode(WhileK);
    if (tree != NULL) {
        tree->child[0] = expr;
        tree->child[1] = stmt;
    }
    return tree;
}

static TreeNode *return_statement(void) {
    TreeNode *tree;
    TreeNode *expr = NULL;

    match(RETURN);
    tree = newStmtNode(ReturnK);
    if (token != SEMI)
        expr = expression();
    if (tree != NULL)
        tree->child[0] = expr;
    match(SEMI);
    return tree;
}

static TreeNode *expression(void) {
    TreeNode *tree = NULL;
    TreeNode *lvalue = NULL;
    TreeNode *rvalue = NULL;
    int gotLvalue = FALSE;

    if (token == ID) {
        lvalue = ident_statement();
        gotLvalue = TRUE;
    }
    if ((gotLvalue == TRUE) && (token == ASSIGN)) {
        if ((lvalue != NULL) && (lvalue->nodekind == ExpK) && (lvalue->kind.exp == IdK)) {
            match(ASSIGN);
            rvalue = expression();
            tree = newExpNode(AssignK);
            if (tree != NULL) {
                tree->child[0] = lvalue;
                tree->child[1] = rvalue;
            }
        } else {
            syntaxError("attempt to assign to something not an lvalue\n");
            token = getToken();
        }
    } else
        tree = simple_expression(lvalue);
    return tree;
}

static TreeNode *simple_expression(TreeNode *passdown) {
    TreeNode *tree;
    TreeNode *lExpr = NULL;
    TreeNode *rExpr = NULL;
    TokenType operator;

    lExpr = additive_expression(passdown);
    if ((token == LTE) || (token == GTE) || (token == GT) || (token == LT) || (token == EQ) ||
        (token == NE)) {
        operator= token;
        match(token);
        rExpr = additive_expression(NULL);
        tree = newExpNode(OpK);
        if (tree != NULL) {
            tree->child[0] = lExpr;
            tree->child[1] = rExpr;
            tree->op = operator;
        }
    } else
        tree = lExpr;
    return tree;
}

static TreeNode *additive_expression(TreeNode *passdown) {
    TreeNode *tree;
    TreeNode *newNode;

    tree = term(passdown);
    while ((token == PLUS) || (token == MINUS)) {
        newNode = newExpNode(OpK);
        if (newNode != NULL) {
            newNode->child[0] = tree;
            newNode->op = token;
            tree = newNode;
            match(token);
            tree->child[1] = term(NULL);
        }
    }
    return tree;
}

static TreeNode *term(TreeNode *passdown) {
    TreeNode *tree;
    TreeNode *newNode;

    tree = factor(passdown);
    while ((token == TIMES) || (token == DIVIDE)) {
        newNode = newExpNode(OpK);
        if (newNode != NULL) {
            newNode->child[0] = tree;
            newNode->op = token;
            tree = newNode;
            match(token);
            newNode->child[1] = factor(NULL);
        }
    }
    return tree;
}

static TreeNode *factor(TreeNode *passdown) {
    TreeNode *tree = NULL;

    if (passdown != NULL) {
        return passdown;
    }
    if (token == ID) {
        tree = ident_statement();
    } else if (token == LPAREN) {
        match(LPAREN);
        tree = expression();
        match(RPAREN);
    } else if (token == NUM) {
        tree = newExpNode(ConstK);
        if (tree != NULL) {
            tree->val = atoi(tokenString);
            tree->variableDataType = Integer;
        }
        match(NUM);
    } else {
        syntaxError("unexpected token ");
        printToken(token, tokenString);
        fprintf(listing, "\n");
        token = getToken();
    }
    return tree;
}

static TreeNode *ident_statement(void) {
    TreeNode *tree;
    TreeNode *expr = NULL;
    TreeNode *arguments = NULL;
    char *identifier;

    if (token == ID)
        identifier = copyString(tokenString);
    match(ID);
    if (token == LPAREN) {
        match(LPAREN);
        arguments = args();
        match(RPAREN);
        tree = newStmtNode(CallK);
        if (tree != NULL) {
            tree->child[0] = arguments;
            tree->name = identifier;
        }
    } else {
        if (token == LSQUARE) {
            match(LSQUARE);
            expr = expression();
            match(RSQUARE);
        }
        tree = newExpNode(IdK);
        if (tree != NULL) {
            tree->child[0] = expr;
            tree->name = identifier;
        }
    }
    return tree;
}

static TreeNode *args(void) {
    TreeNode *tree = NULL;
    if (token != RPAREN)
        tree = arg_list();
    return tree;
}

static TreeNode *arg_list(void) {
    TreeNode *tree;
    TreeNode *ptr;
    TreeNode *newNode;
    tree = expression();
    ptr = tree;
    while (token == COMMA) {
        match(COMMA);
        newNode = expression();
        if ((ptr != NULL) && (tree != NULL)) {
            ptr->sibling = newNode;
            ptr = newNode;
        }
    }
    return tree;
}

TreeNode *parse(void) {
    TreeNode *t;
    token = getToken();
    t = declaration_list();
    if (token != ENDOFFILE)
        syntaxError("Unexpected symbol at end of file\n");
    return t;
}