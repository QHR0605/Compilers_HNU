#include "globals.h"
#include "symtab.h"
#include "code.h"
#include "cgen.h"

#define ADDRESS 1
#define VALUE 0
static int getInfo = 0;
static int saved_config = FALSE;
static int siblingRecur = TRUE;
static char *currentFuncName = NULL;

int globalIndex = 0;
typedef struct varNode {
    char *name;
    int index;
    struct varNode *next;
} VarNode;
static VarNode *globalVars = NULL;

static void push_global(char *name) {
    VarNode *v = (VarNode *)malloc(sizeof(VarNode));
    v->name = copyString(name);
    v->index = globalIndex++;

    v->next = globalVars;
    globalVars = v;
}
static void traverseGlobal(TreeNode *root) {
    if (root == NULL)
        return;

    if (root->nodekind == StmtK) {
        if ((root->kind.stmt == VarDecK) || (root->kind.stmt == ArrayDecK))
            push_global(root->attr.name);
    }

    traverseGlobal(root->sibling);
}
static int isGlobalVar(TreeNode *t) {
    VarNode *v = globalVars;
    if (t->declNode->scope == 0) {
        while (v != NULL) {
            if (strcmp(v->name, t->attr.name) == 0)
                return TRUE;
            v = v->next;
        }
    }
    return FALSE;
}

typedef struct funcInfo {
    char *name;
    int para_num;
    int var_num;

    int inst_addr;

    TreeNode *dec;
    VarNode *vars;
    VarNode *params;
    struct funcInfo *next;
} FuncInfo;
static FuncInfo *functionList = NULL;

static FuncInfo *lookup_func(char *name) {
    FuncInfo *func_ret = functionList;
    while (func_ret != NULL) {
        if (strcmp(func_ret->name, name) == 0)
            return func_ret;
        func_ret = func_ret->next;
    }
    return NULL;
}
static int offset(char *name) {
    FuncInfo *f = lookup_func(currentFuncName);
    VarNode *vTmp;

    if (f != NULL) {
        vTmp = f->vars;
        while (vTmp != NULL) {
            if (strcmp(vTmp->name, name) == 0) {
                return vTmp->index;
            }
            vTmp = vTmp->next;
        }
        vTmp = f->params;
        while (vTmp != NULL) {
            if (strcmp(vTmp->name, name) == 0) {
                return vTmp->index;
            }
            vTmp = vTmp->next;
        }
    }
    return -1;
}
static int isParam(char *name) {
    int func_ret;
    int ofs = offset(name);
    if (ofs == -1)
        return -1;

    FuncInfo *f = lookup_func(currentFuncName);
    VarNode *params = f->params;
    while (params != NULL) {
        if (strcmp(params->name, name) == 0)
            return TRUE;
        params = params->next;
    }
    return FALSE;
}
static void push_func(FuncInfo *f) {
    f->next = functionList;
    functionList = f;
}
static FuncInfo *makeFuncInfo(TreeNode *dec) {
    if (dec == NULL)
        printf("dec is null in makeFuncInfo");

    int index = 0;
    TreeNode *tmp;
    VarNode *vTmp;
    TreeNode *params;
    TreeNode *compound_stmt;
    TreeNode *local_declarations;

    FuncInfo *info = (FuncInfo *)malloc(sizeof(FuncInfo));
    info->dec = dec;
    info->name = copyString(dec->attr.name);
    info->inst_addr = emitSkip(0);

    if (strcmp(dec->attr.name, "input") == 0) {
        info->var_num = 0;
        info->vars = NULL;
        info->para_num = 0;
        info->params = NULL;
        return info;
    } else if (strcmp(dec->attr.name, "output") == 0) {
        info->var_num = 0;
        info->vars = NULL;
        info->para_num = 1;
        vTmp = (VarNode *)malloc(sizeof(VarNode));
        vTmp->index = 0;
        vTmp->name = copyString(dec->child[0]->attr.name);

        if (info->params == NULL)
            info->params = vTmp;
        else {
            vTmp->next = info->params;
            info->params = vTmp;
        }
        return info;
    }

    params = dec->child[0];
    compound_stmt = dec->child[1];
    local_declarations = compound_stmt->child[0];

    if (params->type == Void)
        info->para_num = 0;
    else {
        tmp = params;
        while (tmp != NULL) {
            if (tmp->kind.exp == IdArrayK) {
                info->para_num += tmp->child[0]->attr.val;
            }
            else info->para_num++;
            vTmp = (VarNode *)malloc(sizeof(VarNode));
            if (tmp->kind.exp == IdArrayK) {
                vTmp->index = index;
                index += tmp->child[0]->attr.val;
            }
            else vTmp->index = index++;
            vTmp->name = copyString(tmp->attr.name);

            if (info->params == NULL)
                info->params = vTmp;
            else {
                vTmp->next = info->params;
                info->params = vTmp;
            }
            tmp = tmp->sibling;
        }
    }

    if (local_declarations == NULL)
        info->var_num = 0;
    else {
        index = 0;
        tmp = local_declarations;
        while (tmp != NULL) {
            if (tmp->kind.exp == IdArrayK) {
                info->var_num += tmp->child[0]->attr.val;
            }
            else info->var_num++;
            vTmp = (VarNode *)malloc(sizeof(VarNode));
            if (tmp->kind.exp == IdArrayK) {
                vTmp->index = index;
                index += tmp->child[0]->attr.val;
            }
            else vTmp->index = index++;
            vTmp->name = copyString(tmp->attr.name);

            if (info->vars == NULL)
                info->vars = vTmp;
            else {
                vTmp->next = info->vars;
                info->vars = vTmp;
            }
            tmp = tmp->sibling;
        }
    }
    return info;
}

static void push_reg(int reg, char *str) {
    emitRM("LDA", esp, -1, esp, str);
    emitRM("ST", reg, 0, esp, "");
}
static void pop_reg(int reg, char *str) {
    emitRM("LDA", esp, 1, esp, str);
    emitRM("LD", reg, -1, esp, "");
}

static void func_in(char *name) {
    FuncInfo *f = lookup_func(name);
    int size = f->var_num;

    if (TraceCode) emitComment("-> function head");
    push_reg(ebp, "PUSH EBP");
    emitRM("LDA", ebp, 0, esp, "MOV EBP, ESP");
    emitRM("LDA", esp, -size, esp, "ESP -= # var");
    if (TraceCode) emitComment("<- function head");
}
static void func_ret() {
    if (TraceCode) emitComment("-> function tail");
    emitRM("LDA", esp, 0, ebp, "MOV ESP, EBP");
    pop_reg(ebp, "POP EBP");
    pop_reg(pc, "Return addr; POP PC");
    if (TraceCode) emitComment("<- function tail");
}
static void call(FuncInfo *f) {
    char *name = f->name;
    int inst_addr = f->inst_addr;
    int arg_num = f->para_num;

    if (TraceCode) {
        emitComment("-> call");
        emitComment(name);
    }

    emitRM("LDA", ac, 3, pc, "ac = pc + 3 (next pc)");
    push_reg(ac, "PUSH AC (return address)");
    emitRM("LDC", pc, inst_addr, 0, "pc = address (jmp to called function)");
    emitRM("LDA", esp, arg_num, esp, "esp = esp + arg_num");

    if (TraceCode) emitComment("<- call");
}

/* prototype for internal recursive code generator */
static void cGen(TreeNode *tree);

/* Procedure genStmt generates code at a statement node */
static void genStmt(TreeNode *tree) {
    TreeNode *p1, *p2, *p3;
    int savedLoc1, savedLoc2, currentLoc;
    int loc;
    switch (tree->kind.stmt) {

    case IfK:
        if (TraceCode) emitComment("-> if");
        p1 = tree->child[0];
        p2 = tree->child[1];
        p3 = tree->child[2];
        
        cGen(p1);
        savedLoc1 = emitSkip(1);
        emitComment("if: jump to else belongs here");

        cGen(p2);
        savedLoc2 = emitSkip(1);
        emitComment("if: jump to end belongs here");
        currentLoc = emitSkip(0);
        emitBackup(savedLoc1);
        emitRM_Abs("JEQ", ac, currentLoc, "if: jmp to else");
        emitRestore();

        cGen(p3);
        currentLoc = emitSkip(0);
        emitBackup(savedLoc2);
        emitRM_Abs("LDA", pc, currentLoc, "jmp to end");
        emitRestore();
        if (TraceCode) emitComment("<- if");
        break; /* if_k */

    case AssignK:
        if (TraceCode) emitComment("-> assign");
        
        getInfo = ADDRESS;
        cGen(tree->child[0]);
        push_reg(ac, "push ac");

        getInfo = VALUE;
        cGen(tree->child[1]);
        pop_reg(ac1, "pop ac1");

        emitRM("ST", ac, 0, ac1, "dMem[ac1]=ac");

        if (TraceCode) emitComment("<- assign");
        break;

    case ParamK:
    case VarDecK:
    case ArrayDecK:
        break;

    case FuncDecK:
        currentFuncName = copyString(tree->attr.name);

        if (TraceCode) {
            emitComment("-> function declaration");
            emitComment(tree->attr.name);
        }

        push_func(makeFuncInfo(tree));
        func_in(tree->attr.name);
        cGen(tree->child[1]);
        func_ret();

        if (TraceCode) emitComment("-> function declaration");
        break;

    case ComK:
        if (TraceCode) emitComment("-> compound");
        cGen(tree->child[1]);
        if (TraceCode) emitComment("<- compound");
        break;

    case WhileK:
        if (TraceCode) emitComment("-> while");
        savedLoc1 = emitSkip(0);
        emitComment("while: comes back here after body");
        cGen(tree->child[0]);
        savedLoc2 = emitSkip(1);
        emitComment("while: conditional jmp to end");
        cGen(tree->child[1]);
        emitRM("LDC", pc, savedLoc1, 0, "while: go back; pc=addr of condition");

        currentLoc = emitSkip(0);
        emitBackup(savedLoc2);
        emitRM_Abs("JEQ", ac, currentLoc, "while: if ac==0, pc=addr of endwhile");
        emitRestore();
        if (TraceCode) emitComment("<- while");
        break;

    case ReturnK:
        if (TraceCode) emitComment("-> return");
        if ((tree->child[0] != NULL) && (tree->child[0]->type == Integer))
            cGen(tree->child[0]);
        func_ret();
        if (TraceCode) emitComment("<- return");
        break;

    default:
        break;
    }
} /* genStmt */

void getVar(TreeNode *var) {
    int isPar = isParam(var->attr.name);
    int isGlob = isGlobalVar(var);
    int ofs = offset(var->attr.name);

    if (isGlob == FALSE && ofs == -1) {
        printf("%s\n", var->attr.name);
        printf("some error! in getVar\n");
        return;
    }

    if (TraceCode) emitComment("-> get variable");
    if (isPar == TRUE) {
        if (var->kind.exp == IdK) {
            emitRM("LDA", ac, ofs + 2, ebp, "ac = ebp+offset+2");
        } else if (var->kind.exp == IdArrayK) {
            emitRM("LDA", ac1, ofs + 2, ebp, "ac1 = ebp+offset+2");
        }
    } else {
        if (var->nodekind == ExpK) {
            switch (var->kind.exp) {
            case IdK:
                if (isGlobalVar(var))
                    emitRM("LDA", ac, -ofs, gp, "ac = gp - offset");
                else
                    emitRM("LDA", ac, -1 - ofs, ebp, "ac = ebp-offset-1");
                break;
            case IdArrayK:
                if (isGlobalVar(var))
                    emitRM("LDA", ac1, -ofs, gp, "ac1 = gp - offset");
                else
                    emitRM("LDA", ac1, -1 - ofs, ebp, "ac1 = ebp-offset-1");
                break;
            default:
                break;
            }
        }
    }
    if (TraceCode) emitComment("<- get variable");
} /* getVar */
void Relop(char *op) {
    emitRO("SUB", ac, ac1, ac, "reg[ac]=reg[ac1]-reg[ac]");
    emitRM(op, ac, 2, pc, "conditional jmp: if true");
    emitRM("LDC", ac, 0, ac, "ac=0 (false case");
    emitRM("LDA", pc, 1, pc, "unconditional jmp");
    emitRM("LDC", ac, 1, ac, "ac=1 (true case");
}

/* Procedure genExp generates code at an expression node */
static void genExp(TreeNode *tree) {
    int loc;
    switch (tree->kind.exp) {

    case ConstK:
        if (TraceCode) emitComment("-> Const");
        emitRM("LDC", ac, tree->attr.val, 0, "ac=const value");
        if (TraceCode) emitComment("<- Const");
        break;

    case IdK:
        if (TraceCode) emitComment("-> Id");
        getVar(tree);
        if (getInfo == VALUE)
            emitRM("LD", ac, 0, ac, "ac=dMem[ac]");
        if (TraceCode) emitComment("<- Id");
        break;

    case IdArrayK:
        if (TraceCode) emitComment("-> Array Id");
        getVar(tree);
        saved_config = getInfo;
        getInfo = ADDRESS;
        cGen(tree->child[0]);
        getInfo = saved_config;
        emitRO("SUB", ac, ac1, ac, "ac=ac1-ac (array offset)");
        if (getInfo == VALUE)
            emitRM("LD", ac, 0, ac, "ac=dMem[ac]");
        if (TraceCode) emitComment("<- Array Id");
        break;

    case OpK:
        if (TraceCode) emitComment("-> Op");
        getInfo = VALUE;
        cGen(tree->child[0]);
        push_reg(ac, "PUSH AC");

        getInfo = VALUE;
        cGen(tree->child[1]);
        pop_reg(ac1, "POP AC1");

        switch (tree->attr.op) {
        case PLUS:
            emitRO("ADD", ac, ac1, ac, "reg[ac]=reg[ac1] + reg[ac]");
            break;
        case MINUS:
            emitRO("SUB", ac, ac1, ac, "reg[ac]=reg[ac1] - reg[ac]");
            break;
        case TIMES:
            emitRO("MUL", ac, ac1, ac, "reg[ac]=reg[ac1] * reg[ac]");
            break;
        case OVER:
            emitRO("DIV", ac, ac1, ac, "reg[ac]=reg[ac1] / reg[ac]");
            break;
        case EQ:
            Relop("JEQ");
            break;
        case NEQ:
            Relop("JNE");
            break;
        case LG:
            Relop("JGT");
            break;
        case LE:
            Relop("JGE");
            break;
        case SM:
            Relop("JLT");
            break;
        case SE:
            Relop("JLE");
            break;

        default:
            emitComment("BUG: Unknown operator");
            break;
        } /* case op */
        if (TraceCode) emitComment("<- Op");
        break; /* OpK */

    case CallK:
        if (TraceCode) {
            emitComment("-> call");
            emitComment(tree->attr.name);
        }

        TreeNode *p = tree->child[0];
        while (p != NULL) {
            pushParam(p);
            p = p->sibling;
        }

        siblingRecur = FALSE;
        while ((p = popParam()) != NULL) {
            getInfo = VALUE;
            cGen(p);
            push_reg(ac, "PUSH AC (for argument)");
        }
        siblingRecur = TRUE;

        FuncInfo *f = lookup_func(tree->attr.name);
        call(f);
        if (TraceCode) emitComment("<- call");
        break;

    default:
        break;
    }
} /* genExp */

/* Procedure cGen recursively generates code by
 * tree traversal
 */
static void cGen(TreeNode *tree) {
    if (tree != NULL) {
        switch (tree->nodekind) {
        case StmtK:
            genStmt(tree);
            break;
        case ExpK:
            genExp(tree);
            break;
        default:
            break;
        }
        if (siblingRecur == TRUE)
            cGen(tree->sibling);
    }
}

/* after this function called,
 * reg[0] = 0
 * reg[gp] = max address of memory
 * reg[esp] = max address - (global variable number + 1)
 */
static void codeGenInit(void) {
    int size = get_global_var_num();

    emitComment("Standard prelude:");
    emitRM("LD", gp, 0, 0, "gp=dMem[0] (dMem[0]==maxaddress)");
    emitRM("ST", 0, 0, 0, "dMem[0]=0 (clear location 0)");
    emitRM("LDA", esp, -size + 1, gp, "esp=gp-global_var_num+1");
    emitComment("End of standard prelude.");
}
static void codeGenInput(void) {
    if (TraceCode) emitComment("-> pre-defined function: input");

    TreeNode *inputTree = st_func_lookup("input");
    FuncInfo *funcInfo = makeFuncInfo(inputTree);
    funcInfo->inst_addr = emitSkip(0);

    if ((inputTree != NULL) && (funcInfo != NULL))
        push_func(funcInfo);
    else
        exit(-1);

    func_in("input");
    emitRO("IN", ac, 0, 0, "input ac");
    func_ret();
    if (TraceCode) emitComment("<- pre-defined function: input");
}
static void codeGenOutput(void) {
    if (TraceCode) emitComment("-> pre-defined function: output");

    TreeNode *outputTree = st_func_lookup("output");
    FuncInfo *funcInfo = makeFuncInfo(outputTree);
    funcInfo->inst_addr = emitSkip(0);

    if ((outputTree != NULL) && (funcInfo != NULL))
        push_func(funcInfo);
    else
        exit(-1);

    func_in("output");
    emitRM("LD", ac, 2, ebp, "ac=dMem[ebp+2] (param 0)");
    emitRO("OUT", ac, 0, 0, "output ac");
    func_ret();
    if (TraceCode) emitComment("<- pre-defined function: output");
}

/**********************************************/
/* the primary function of the code generator */
/**********************************************/
/* Procedure codeGen generates code to a code
 * file by traversal of the syntax tree. The
 * second parameter (codefile) is the file name
 * of the code file, and is used to print the
 * file name as a comment in the code file
 */
void codeGen(TreeNode *syntaxTree, char *codefile) {
    char *s = (char *)malloc(strlen(codefile) + 7);
    int mainLoc;
    strcpy(s, "File: ");
    strcat(s, codefile);

    emitComment("C-MINUS Compilation to TM Code");
    emitComment(s);
    codeGenInit();

    emitComment("skip 5 instr: call main, waiting for addr of main");
    mainLoc = emitSkip(5);
    emitRO("HALT", 0, 0, 0, "stop program");

    codeGenInput();
    codeGenOutput();

    traverseGlobal(syntaxTree);
    cGen(syntaxTree);

    emitBackup(mainLoc);
    call(lookup_func("main"));
    emitRestore();

    emitComment("END OF C-MINUS Compilation to TM Code");
}
