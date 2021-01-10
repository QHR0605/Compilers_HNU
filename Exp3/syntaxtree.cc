#include "globals.h"
#include "LL.h"

/* * 
 * 打印语法树
 * @param: 
 * t: 从t结点开始打印
 * depth: 当前的深度（用于缩进控制）
 * */
void printSyntaxTree(TreeNode *t, int depth) {
    for (int i = 0; i < depth; i++)
        cout << "\t\t";

    if (t->nodekind == ExpK) {
        cout << "|";
        printResult(t->attr.op, t->name);
    } else cout << "|" << t->name << endl;

    for (int i = 0; i < t->max_child; i++)
        printSyntaxTree(t->child[i], depth + 1);
    return;
}

// 错误处理函数
void ERROR_FUNC(TreeNode *head, string top = "", TokenType token_ = ENDFILE) {
    if (top == "" && token_ != ENDFILE) {
        cout << "ERROR!" << endl;
        cout << "now token: " << token << endl;
        cout << "top token: ";
        printResult(token_);
    } else if (token_ == ENDFILE && top != "") {
        cout << "ERROR!" << endl;
        cout << "nonterminal: " << top << endl;
        cout << "now token: ";
        printResult(token);
    } else {
        cout << "ERROR!" << endl;
        cout << "now token: " << token << endl;
        cout << "nonterminal: " << top << endl;
        cout << "top token: ";
        printResult(token);
    }
    fclose(stdout);
    cout.clear();
    freopen("CON", "w", stdout);
    freopen("output/SyntaxTree", "w", stdout);
    printSyntaxTree(head, 0);
    fclose(stdout);
    cout.clear();
    freopen("CON", "w", stdout);
    exit(0);
}

// 生成新的 StmtK 结点 即非终结符
TreeNode *newRootNode(string name) {
    TreeNode *t = new TreeNode;
    t->lineno = line_no;
    t->child_no = 0;
    t->max_child = -1;
    t->nodekind = StmtK;
    t->name = name;
    return t;
}

// 生成子结点 根据子结点类型: 终结符 非终结符 判断生成哪一种结点
TreeNode *newNode(TreeNode *fa, string name) {
    TreeNode *t = new TreeNode;
    set<string> nonterminal = ll1.get_nonterminal();
    if (nonterminal.find(name) != nonterminal.end()) {
        t = newRootNode(name);
    } else {
        t->lineno = line_no;
        t->child_no = 0;
        t->max_child = 0;
        t->nodekind = ExpK;
        TokenType tt = ll1.get_Terminal(name);
        t->attr.op = tt;
        t->name = name;
    }
    t->father = fa;
    return t;
}

// 对结点进行重命名, 以显示当前ID或NUM接收到的具体的字符串
void rename(TreeNode *t) {
    t->name = currString;
}

// 重新分配当前结点的最大子结点个数
// 因为在生成子结点时, 没有对应的token, 无法得到正确的最大子结点个数
void resize_treenode(TreeNode *t, TreeNode *head) {
    t->child_no = 0;
    if (t->nodekind == ExpK)
        t->max_child = 0;
    else {
        vector<int> nxt_prod = ll1.LL1_table[t->name][token];
        if (nxt_prod.size() == 0) {
            ERROR_FUNC(head, t->name, token);
        }
        if (ll1.prod[nxt_prod[0]][1] == "$")
            t->max_child = 0;
        else
            t->max_child = ll1.prod[nxt_prod[0]].size() - 1;
    }
}

// 回溯, 找到最近的一个还有未被访问过的子结点的结点
TreeNode *get_Free_Node(TreeNode *t, int pos) {
    if (pos == 1) {
        if (t->max_child != 0) {
            return t->child[t->child_no++];
        }
    }
    if (t->nodekind == ExpK && t->father != NULL) {
        t = t->father;
        if (t->child_no < t->max_child) {
            t = t->child[t->child_no++];
        }
    }
    if (t->nodekind == StmtK) {
        while (t->child_no >= t->max_child && t->max_child != -1 && t->nodekind == StmtK &&
               t->father != NULL) {
            t = t->father;
            if (t->child_no < t->max_child) {
                t = t->child[t->child_no++];
                if (t->max_child == -1) {
                    break;
                }
            }
        }
    }
    return t;
}
