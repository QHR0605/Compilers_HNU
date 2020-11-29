// #define _DEBUG_INFO_
#define SYNTAX_TREE

#include "scan.cc"
#include "globals.h"
#include "LL.h"

#ifdef SYNTAX_TREE
#include "syntaxtree.cc"
#endif
using namespace std;

stack<string> stk;
set<string> nonterminal;

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
                rename(t);
                token = getToken(fp);
#ifdef SYNTAX_TREE
                t = get_Free_Node(t, 0);
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
            t = get_Free_Node(t, 1);
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