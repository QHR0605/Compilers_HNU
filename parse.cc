#include "globals.h"
#include "LL.h"
#include "scan.cc"
#include "syntaxtree.cc"
using namespace std;

stack<string> stk;
set<string> nonterminal;

void print_stack_info(stack<string> s) { // 打印栈的信息
    stack<string> ss = s;
    while (!ss.empty()) {
        cout << ss.top() << endl;
        ss.pop();
    }
    cout << "---------------" << endl;

    cout << "Token = ";
    printResult(token);
    cout << endl;
}
int main() {
    FILE *fp = fopen("input/pos.tny", "r");
    nonterminal = ll1.get_nonterminal(); // 非终结集
    freopen("output/stack", "w", stdout);

    stk.push("program");
    token = ERROR;
    token = getToken(fp);

    // 生成根节点
    TreeNode *t = new TreeNode;
    t = newStmtNode("program");
    // 根节点指针，用于发生错误时输出语法树
    TreeNode *head = t;

    while (!stk.empty()) {
        print_stack_info(stk);

        string top = stk.top();
        stk.pop();

        resize_treenode(t, head);

        if (nonterminal.find(top) == nonterminal.end()) {
            TokenType tt = ll1.get_Terminal(top);
            if (tt == token) {
                rename(t);
                token = getToken(fp);
                // 语法树回溯, 因为到达了叶子节点
                t = get_Free_Node(t, 0);
            } else { // ERROR!
                ERROR_FUNC(head, "", tt);
            }
        } else {
            vector<int> nxt_prod = ll1.LL1_table[top][token];
            if (nxt_prod.size() == 0) { // ERROR！
                ERROR_FUNC(head, top);
            } else {
                vector<string> ret = ll1.prod[nxt_prod[0]];
                for (int i = ret.size() - 1; i >= 1; i--) {
                    if (ret[i] == "$") // 空串不压栈
                        continue;
                    stk.push(ret[i]);
                    // 生成当前结点子结点，但不分配最大子结点大小
                    t->child[i - 1] = newChildNode(t, ret[i]);
                }
            }
            // 看当前非叶子结点是否需要回溯：可能所以子结点都被遍历
            // 若不需要，则进入下一个子结点继续
            t = get_Free_Node(t, 1);
        }
    }
    // 语法分析成功
    cout << "YES" << endl;
    fclose(stdout);
    cout.clear();
    freopen("CON", "w", stdout);

    // 打印语法树
    // 上述算法在成功结束时，构造语法树的指针t一定会回到根节点
    freopen("output/SyntaxTree", "w", stdout);
    printSyntaxTree(t, 0);
    fclose(stdout);
    cout.clear();
    freopen("CON", "w", stdout);
}