#include "globals.h"
#include "LL.h"

void printSyntaxTree(TreeNode *t, int depth) {
    // freopen("output/SyntaxTree", "w", stdout);
    // cout << t->name << endl;
    for (int i = 0; i < depth; i++) {
        cout << "\t\t";
    }
    if (t->nodekind == ExpK) {
        cout << "|";
        printResult(t->attr.op, t->name);
    } else {
        cout << "|" << t->name << endl;
    }
    for (int i = 0; i < t->max_child; i++) {
        printSyntaxTree(t->child[i], depth + 1);
    }
    // fclose(stdout);
    // cout.clear();
    // freopen("CON", "w", stdout);
    return;
}
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
TreeNode *newStmtNode(string name) {
    TreeNode *t = new TreeNode;
    t->lineno = line_no;
    t->child_no = 0;
    t->max_child = -1;
    t->nodekind = StmtK;
    t->name = name;
    return t;
}
TreeNode *newChildNode(TreeNode *fa, string name) {
    TreeNode *t = new TreeNode;
    set<string> nonterminal = ll1.get_nonterminal();
    if (nonterminal.find(name) != nonterminal.end()) {
        t = newStmtNode(name);
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
    // cout << fa->name << endl;
    return t;
}
void rename(TreeNode *t) {
#ifdef _DEBUG_INFO_
    cout << "in rename" << endl;
    cout << t->name << endl;
#endif
    t->name = currString;
#ifdef _DEBUG_INFO_
    cout << t->name << endl;
    cout << "out" << endl;
#endif
}
void resize_treenode(TreeNode *t, TreeNode *head) {
    t->child_no = 0;
    if (t->nodekind == ExpK)
        t->max_child = 0;
    else {
#ifdef _DEBUG_INFO_
        cout << "resize!" << endl;
        cout << t->name << " ";
        printResult(token);
        cout << "end-resize!" << endl;
#endif
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

#ifdef _DEBUG_INFO_
        cout << "back..." << endl;
#endif
        while (t->child_no >= t->max_child && t->max_child != -1 && t->nodekind == StmtK &&
               t->father != NULL) {
#ifdef _DEBUG_INFO_
            cout << t->name << endl;
            cout << t->child_no << " " << t->max_child << endl;
#endif
            t = t->father;
            if (t->child_no < t->max_child) {
                t = t->child[t->child_no++];
                if (t->max_child == -1) {
                    break;
                }
            }
        }
#ifdef _DEBUG_INFO_
        cout << "done! " << endl;
#endif
    }
    return t;
}
void show_tree(TreeNode *t) {
    cout << "father: " << t->name << endl;
    for (int i = t->child_no; i < t->max_child; i++) {
        cout << t->child[i]->name << " ";
    }
    cout << endl << endl;
}
