#include "LL.cc"
#include "scan.cc"
#ifndef _GLOBALS_H_
#define _GLOBALS_H_
#include "globals.h"
#endif
using namespace std;

#define _DEBUG_INFO_
#define SYNTAX_TREE
TokenType token;
stack<string> stk;
set<string> nonterminal;

LL ll1;

void printSyntaxTree(TreeNode *t, int depth) {
    // freopen("output/SyntaxTree", "w", stdout);
    // cout << t->name << endl;
    for (int i = 0; i < depth; i++) {
        cout << "\t\t";
    }
    if (t->nodekind == ExpK) {
        cout << "|";
        printResult(t->attr.op);
    } else
        cout << "|" << t->name << endl;
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
#ifdef SYNTAX_TREE
    fclose(stdout);
    cout.clear();
    freopen("CON", "w", stdout);
    freopen("output/SyntaxTree", "w", stdout);
    printSyntaxTree(head, 0);
    fclose(stdout);
    cout.clear();
    freopen("CON", "w", stdout);
#endif
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
    if (nonterminal.find(name) != nonterminal.end()) {
        t = newStmtNode(name);
    } else {
        t->lineno = line_no;
        t->child_no = 0;
        t->max_child = 0;
        t->nodekind = ExpK;
        TokenType tt = ll1.get_Terminal(name);
        t->attr.op = tt;
        t->name = currString;
    }
    t->father = fa;
    // cout << fa->name << endl;
    return t;
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
void show_tree(TreeNode *t) {
    cout << "father: " << t->name << endl;
    for (int i = t->child_no; i < t->max_child; i++) {
        cout << t->child[i]->name << " ";
    }
    cout << endl << endl;
}
void print_stack(stack<string> s) {
    stack<string> ss = s;
    while (!ss.empty()) {
        cout << ss.top() << endl;
        ss.pop();
    }
    cout << "---------------" << endl;
}
int main() {
    FILE *fp = fopen("input/pos.tny", "r");
    nonterminal = ll1.get_nonterminal();
    freopen("output/stack", "w", stdout);

    stk.push("program");
    token = ERROR;
    token = getToken(fp);

#ifdef SYNTAX_TREE
    TreeNode *t = new TreeNode;
    t = newStmtNode("program");
    TreeNode *head = t;
#endif

    while (!stk.empty()) {

        print_stack(stk);
        cout << "Token = ";
        printResult(token);
        cout << endl;
        // printf("%s\n", currString);
        string top = stk.top();
        stk.pop();
#ifdef SYNTAX_TREE
        resize_treenode(t, head);
#endif
#ifdef _DEBUG_INFO_
        cout << t->name << " = name" << endl;
        cout << t->child_no << " " << t->max_child << endl << endl;
#endif
        if (nonterminal.find(top) == nonterminal.end()) {
            TokenType tt = ll1.get_Terminal(top);
            if (tt == token) {
                token = getToken(fp);
#ifdef SYNTAX_TREE
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
                    while (t->child_no >= t->max_child && t->max_child != -1 &&
                           t->nodekind == StmtK && t->father != NULL) {
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
#endif
                // printResult(token);
            } else { // ERROR
#ifdef SYNTAX_TREE
                ERROR_FUNC(head, "", tt);
#else
                cout << "ERROR!" << endl;
                cout << "now token: " << token << endl;
                cout << "top token: ";
                printResult(tt);
                exit(0);
#endif
            }
        } else {
            vector<int> nxt_prod = ll1.LL1_table[top][token];
            if (nxt_prod.size() == 0) {
#ifdef SYNTAX_TREE
                ERROR_FUNC(head, top);
#else
                cout << "ERROR!" << endl;
                cout << "nonterminal: " << top << endl;
                cout << "now token: ";
                printResult(token);
                exit(0);
#endif
            } else {
                vector<string> ret = ll1.prod[nxt_prod[0]];
                for (int i = ret.size() - 1; i >= 1; i--) {
#ifdef _DEBUG_INFO_
                    cout << i << endl;
#endif
                    if (ret[i] == "$")
                        continue;
#ifdef SYNTAX_TREE
                    t->child[i - 1] = newChildNode(t, ret[i]);
#endif
                    stk.push(ret[i]);
                }
            }
#ifdef SYNTAX_TREE
#ifdef _DEBUG_INFO_
            show_tree(t);
#endif
            if (t->max_child == 0) {
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
                    while (t->child_no >= t->max_child && t->max_child != -1 &&
                           t->nodekind == StmtK && t->father != NULL) {

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
#ifdef _DEBUG_INFO_
                cout << "now at " << t->name << endl;
#endif
            } else
                t = t->child[t->child_no++];
#endif
        }
    }
    cout << "YES" << endl;
    fclose(stdout);
    cout.clear();
    freopen("CON", "w", stdout);

#ifdef SYNTAX_TREE
    // cout << t->name << endl;
    freopen("output/SyntaxTree", "w", stdout);
    printSyntaxTree(t, 0);
    fclose(stdout);
    cout.clear();
    freopen("CON", "w", stdout);
#endif
}