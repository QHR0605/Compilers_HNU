#include <iostream>
#include <string.h>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <set>
#define TEST_CASE_3
#if defined TEST_CASE_1 || defined TEST_CASE_2 || defined TEST_CASE_3
#define TEST_CASE
#endif
using namespace std;

const int MAXN = 105;

char c_set[MAXN]; // 字符集
int c_set_len;    // 字符集长度

int state[MAXN]; // 状态集合
int state_len;   // 状态个数

int done_state_nfa[MAXN]; // 结束状态集合
int done_state_dfa[MAXN]; // 结束状态集合
int done_state_mindfa[MAXN];
int done_state_nfa_len; // 结束状态个数
int done_state_dfa_len;
int done_state_mindfa_len;
int start_state_nfa;
int start_state_dfa[MAXN], start_state_mindfa[MAXN];
int start_dfa_len, start_mindfa_len;

unordered_map<char, vector<int>> NFA_STATE[MAXN];

unordered_map<char, int> DFA_STATE[MAXN]; // DFA状态
unordered_map<int, int> DFA_NFA;
int mark[MAXN];
int dfa_state_len;

unordered_map<char, int> minDFA_STATE[MAXN]; // minDFA状态
unordered_map<int, int> minDFA_DFA;
int fa[MAXN];
int minDFA_len;

void check_move_tabel() {
    printf("\nNFA转移表\n");
    cout << "\t";
    for (int j = 0; j < c_set_len; j++) {
        cout << c_set[j] << "\t";
    }
    cout << endl;
    for (int i = 0; i < state_len; i++) {
        cout << i << "\t";
        for (int j = 0; j < c_set_len; j++) {
            int size = NFA_STATE[i][c_set[j]].size();
            cout << NFA_STATE[i][c_set[j]][0];
            for (int k = 1; k < size; k++) {
                cout << "," << NFA_STATE[i][c_set[j]][k];
            }
            cout << "\t";
        }
        cout << endl;
    }
    printf("DONE STATE: ");
    for (int i = 0; i < done_state_nfa_len; i++) {
        cout << done_state_nfa[i] << " ";
    }
    cout << endl;
    printf("START STATE: %d\n", start_state_nfa);
}
void check_DFA_move_tabel() {
    printf("\nDFA-NFA\n");
    cout << "DFA\tNFA\t\n";
    for (int i = 0; i < dfa_state_len; i++) {
        cout << i << "\t";
        int cnt = 0;
        int state = DFA_NFA[i];
        cout << "{";
        for (int j = 1; j <= (1 << state_len); j <<= 1, cnt++) {
            if (state & j)
                cout << cnt << ",";
        }
        cout << "}\n";
    }
    printf("\nDFA转移表\n");
    cout << "\t";
    for (int j = 1; j < c_set_len; j++) {
        cout << c_set[j] << "\t";
    }
    cout << endl;
    for (int i = 0; i < dfa_state_len; i++) {
        cout << i << "\t";
        for (int j = 1; j < c_set_len; j++) {
            cout << DFA_STATE[i][c_set[j]];
            cout << "\t";
        }
        cout << endl;
    }
    printf("DONE STATE: ");
    for (int i = 0; i < done_state_dfa_len; i++) {
        cout << done_state_dfa[i] << " ";
    }
    cout << endl;
    printf("START STATE: ");
    for (int i = 0; i < start_dfa_len; i++) {
        cout << start_state_dfa[i] << " ";
    }
    cout << endl;
}
void check_minDFA_move_tabel() {
    printf("\nminDFA-DFA\n");
    cout << "minDFA\tDFA\t\n";
    for (int i = 0; i < minDFA_len; i++) {
        cout << i << "\t";
        int cnt = 0;
        int state = minDFA_DFA[i];
        cout << "{";
        for (int j = 1; j <= (1 << state_len); j <<= 1, cnt++) {
            if (state & j)
                cout << cnt << ",";
        }
        cout << "}\n";
    }

    printf("\nminDFA转移表\n");
    cout << "\t";
    for (int j = 1; j < c_set_len; j++) {
        cout << c_set[j] << "\t";
    }
    cout << endl;
    for (int i = 0; i < minDFA_len; i++) {
        cout << i << "\t";
        for (int j = 1; j < c_set_len; j++) {
            cout << minDFA_STATE[i][c_set[j]];
            cout << "\t";
        }
        cout << endl;
    }

    printf("DONE STATE: ");
    for (int i = 0; i < done_state_mindfa_len; i++) {
        cout << done_state_mindfa[i] << " ";
    }
    cout << endl;
    printf("START STATE: ");
    for (int i = 0; i < start_mindfa_len; i++) {
        cout << start_state_mindfa[i] << " ";
    }
    cout << endl;
}
/* 求DFA */
int epsilon_closure(int state) { // state位表示状态
    int ans = 0;
    vector<int> state_; // 得到的状态集合
    int cnt = 0;
    for (int i = 1; i <= (1 << state_len); i <<= 1, cnt++) {
        if (state & i) {
            state_.push_back(cnt);
            ans |= i;
        }
    }
    for (auto v : state_) { // 对该状态集合中每个状态求 ε闭包
        for (auto vec : NFA_STATE[v][c_set[0]]) {
            if (vec != -1) {
                ans |= (1 << vec);
            }
        }
    }

    return ans;
}
int move(int state, char c) { // state位表示状态 c为当前输入字符
    int ans = 0;
    vector<int> state_;
    int cnt = 0;
    for (int i = 1; i <= (1 << state_len); i <<= 1, cnt++) {
        if (state & i)
            state_.push_back(cnt);
    }
    for (auto v : state_) {
        for (auto vec : NFA_STATE[v][c]) {
            if (vec != -1) {
                ans |= (1 << vec);
            }
        }
    }
    return ans;
}
void getDFA() {
    done_state_dfa_len = 0;
    dfa_state_len = 0;
    start_dfa_len = 0;
    memset(mark, 0, sizeof(mark));
    set<int> already_exist;
    int init_state = 1;
    int closure = epsilon_closure(init_state);
    while (closure != epsilon_closure(closure)) {
        closure = epsilon_closure(closure);
    }
    for (int k = 0; k < done_state_nfa_len; k++) {
        if (closure & (1 << done_state_nfa[k])) {
            done_state_dfa[done_state_dfa_len++] = dfa_state_len;
            break;
        }
    }
    if (closure & (1 << start_state_nfa)) {
        start_state_dfa[start_dfa_len++] = dfa_state_len;
    }
    DFA_NFA[dfa_state_len++] = closure;
    int all_marked = 0;
    while (!all_marked) { // 全都被标记过，结束
        int i;
        for (i = 0; i < dfa_state_len; i++) { // 找第一个未被标记的状态
            if (mark[i] == 0) {
                mark[i] = 1;
                break;
            }
        }
        if (i == dfa_state_len) { // 未找到，全被标记
            all_marked = 1;
            continue;
        }

        int now_nfa_state = DFA_NFA[i]; // 得到对应的NFA状态
        already_exist.insert(now_nfa_state);
        for (int j = 1; j < c_set_len; j++) {              // 遍历所有输入
            int new_state = move(now_nfa_state, c_set[j]); // move步
            int closure = epsilon_closure(new_state);      // closure步
            while (closure != epsilon_closure(closure)) {
                closure = epsilon_closure(closure);
            }
            if (closure == 0) { // 没有对应的转换
                DFA_STATE[i][c_set[j]] = {-1};
            } else {
                if (already_exist.find(closure) == already_exist.end()) { // 转换的状态不存在
                    already_exist.insert(closure);                        // 加入到set中
                    if (closure & (1 << start_state_nfa)) {
                        start_state_dfa[start_dfa_len++] = dfa_state_len;
                    }
                    for (int k = 0; k < done_state_nfa_len; k++) {
                        if (closure & (1 << done_state_nfa[k])) {
                            done_state_dfa[done_state_dfa_len++] = dfa_state_len;
                            break;
                        }
                    }
                    DFA_NFA[dfa_state_len++] = closure; // 有了一个新的DFA状态
                }
                // 向当前状态当前输入的转换中添加新状态
                int k;
                for (k = 0; k < dfa_state_len; k++) { // 找要添加的DFA状态
                    if (DFA_NFA[k] == closure) {
                        break;
                    }
                }
                DFA_STATE[i][c_set[j]] = k;
            }
        }
    }
}
/* DFA最小化 */
int findEnd() {
    int ans = 0;
    for (int i = 0; i < done_state_dfa_len; i++) {
        ans |= (1 << done_state_dfa[i]);
    }
    return ans;
}
int cal_Set(vector<int> ret) {
    int ans = 0;
    for (auto v : ret) {
        ans |= (1 << v);
    }
    return ans;
}
void set_fa() {
    for (int i = 0; i < minDFA_len; i++) {
        int state = minDFA_DFA[i];
        int cnt = 0;
        for (int j = 1; j <= (1 << dfa_state_len); j <<= 1, cnt++) {
            if (state & j) {
                fa[cnt] = i;
            }
        }
    }
    return;
}
int divide(int state, int idx) {
    vector<int> divide_list[minDFA_len + 1];

    vector<int> state_;
    int cnt = 0;
    for (int i = 1; i <= (1 << state_len); i <<= 1, cnt++) {
        if (state & i) {
            state_.push_back(cnt);
        }
    }
    // for (auto v: state_) cout << v << " ";
    // cout << endl;
    int ret = 0;
    for (int c = 1; c < c_set_len; c++) {
        for (int i = 0; i <= minDFA_len; i++) {
            divide_list[i].clear();
        }
        char input = c_set[c];
        for (auto v : state_) {
            int nxt = DFA_STATE[v][input];
            if (nxt == -1) {
                divide_list[minDFA_len].push_back(v);
                continue;
            }
            int set = fa[nxt];
            divide_list[set].push_back(v);
        }
        int cnt = 0;
        for (int i = 0; i <= minDFA_len; i++) {
            // cout << i << "->" << divide_list[i].size() << endl;
            if (divide_list[i].size() != 0)
                cnt++;
        }
        if (cnt == 1) {
            continue;
        } else {
            int len = minDFA_len;
            int ret = 0;
            for (int i = 0; i <= len; i++) {
                if (divide_list[i].size() != 0 && ret != 0) {
                    minDFA_DFA[minDFA_len++] = cal_Set(divide_list[i]);
                }
                else if (divide_list[i].size() != 0 && ret == 0) {
                    minDFA_DFA[idx] = cal_Set(divide_list[i]);
                    ret++;
                }
            }
            set_fa();
            return 1;
        }
    }
    return 0;
}
void getTrans() {
    for (int i = 0; i < minDFA_len; i++) {
        int set = minDFA_DFA[i];
        int cnt = 0;
        int ret = 0;
        for (int k = 1; k <= (1 << dfa_state_len); k <<= 1, cnt++) {
            if (set & k)
                break;
        }
        // cout << "cnt: " << cnt << " ";
        for (int c = 0; c < c_set_len; c++) {
            char input = c_set[c];
            int nxt = DFA_STATE[cnt][input];
            if (nxt == -1)
                minDFA_STATE[i][input] = -1;
            else
                minDFA_STATE[i][input] = fa[nxt];
        }
    }
}
void getDone_StartState() {
    done_state_mindfa_len = 0;
    start_mindfa_len = 0;
    for (int i = 0; i < minDFA_len; i++) {
        int set = minDFA_DFA[i];
        for (int k = 0; k < done_state_dfa_len; k++) {
            if (set & (1 << done_state_dfa[k])) {
                done_state_mindfa[done_state_mindfa_len++] = i;
                break;
            }
        }
        for (int k = 0; k < start_dfa_len; k++) {
            if (set & (1 << start_state_dfa[k])) {
                start_state_mindfa[start_mindfa_len++] = i;
                break;
            }
        }
    }
}
void minDFA() {
    int end = findEnd();
    int netend = (1 << (dfa_state_len)) - 1 - end;
    if (netend == 0) {
        minDFA_DFA[0] = end;
        minDFA_len = 1;
    } else {
        minDFA_DFA[0] = netend;
        minDFA_DFA[1] = end;
        minDFA_len = 2;
    }
    set_fa();
    for (int i = 0; i < minDFA_len; i++) {
        if (divide(minDFA_DFA[i], i)) {
            i--;
        }
    }
    getTrans();
    getDone_StartState();
}

int check_NFA(const string &s) {
    int state = (1 << start_state_nfa);
    int closure = epsilon_closure(state);
    while (closure != epsilon_closure(closure)) {
        closure = epsilon_closure(closure);
    }
    for (auto c : s) {
        int state = move(closure, c);
        closure = epsilon_closure(state);
        while (closure != epsilon_closure(closure)) {
            closure = epsilon_closure(closure);
        }
    }
    for (int i = 0; i < done_state_nfa_len; i++) {
        if (closure & (1 << done_state_nfa[i])) {
            cout << "NFA ACCEPT\n";
            return 1;
        }
    }
    cout << "NFA FAIL\n";
    return 0;
}
int check_DFA(const string &s) {
    for (int k = 0; k < start_dfa_len; k++) {
        int state = start_state_dfa[k];
        for (auto c : s) {
            state = DFA_STATE[state][c];
        }
        for (int i = 0; i < done_state_dfa_len; i++) {
            if (state == done_state_dfa[i]) {
                cout << "DFA ACCEPT\n";
                return 1;
            }
        }
    }
    cout << "DFA FAIL\n";
    return 0;
}
int check_minDFA(const string &s) {
    for (int k = 0; k < start_mindfa_len; k++) {
        int state = start_state_mindfa[k];
        for (auto c : s) {
            state = minDFA_STATE[state][c];
        }
        for (int i = 0; i < done_state_mindfa_len; i++) {
            if (state == done_state_mindfa[i]) {
                cout << "minDFA ACCEPT\n";
                return 1;
            }
        }
    }
    cout << "minDFA FAIL\n";
    return 1;
}
void check_right() {
    string s;
    while (getline(cin, s)) {
        cout << s << ":\n";
        check_NFA(s);
        check_DFA(s);
        check_minDFA(s);
        cout << endl;
    }
}
int main() {
#ifdef TEST_CASE_1
    freopen("./input_file/input1", "r", stdin);
    freopen("./output_file/out1", "w", stdout);
#endif
#ifdef TEST_CASE_2
    freopen("./input_file/input2", "r", stdin);
    freopen("./output_file/out2", "w", stdout);
#endif
#ifdef TEST_CASE_3
    freopen("./input_file/input3", "r", stdin);
    freopen("./output_file/out3", "w", stdout);
#endif
    printf("输入字符集中的字符个数: ");
    scanf("%d", &c_set_len);
    c_set_len++;
    c_set[0] = '#';
    printf("输入字符集: (不要加空格)");
    scanf("%s", c_set + 1);
    c_set[c_set_len] = '\0';
    printf("输入状态个数: (状态从0递增开始)");
    scanf("%d", &state_len);
    for (int i = 0; i < state_len; i++) {
        state[i] = i;
    }

    printf("起始状态为0\n");
    start_state_nfa = 0;
    printf("输入结束状态个数: ");
    scanf("%d", &done_state_nfa_len);
    printf("输入结束状态: ");
    for (int i = 0; i < done_state_nfa_len; i++) {
        int a;
        scanf("%d", &a);
        if (a >= state_len) {
            printf("不合法\n");
            i--;
        } else
            done_state_nfa[i] = a;
    }
    printf("输入转移表: \n");
    for (int i = 0; i < state_len; i++) {
        for (int j = 0; j < c_set_len; j++) { // 解析输入的状态字符串
            string nxt;
            vector<int> ret;
            cin >> nxt;
            if (nxt.find(",") == string::npos) {
                ret.push_back(stoi(nxt));
            } else {
                int idx = 0;
                while (nxt.find(",") != string::npos) {
                    idx = nxt.find(",");
                    string num = nxt.substr(0, idx);
                    ret.push_back(stoi(num));
                    idx++;
                    nxt = nxt.substr(idx);
                }
                ret.push_back(stoi(nxt));
            }
            NFA_STATE[i][c_set[j]] = ret;
        }
    }
    check_move_tabel();
    getDFA();
    check_DFA_move_tabel();
    minDFA();
    check_minDFA_move_tabel();
#ifdef TEST_CASE
    fclose(stdin);
    fclose(stdout);
    cin.clear();
    cout.clear();
#endif
#ifdef TEST_CASE_1
    freopen("./input_file/input1_", "r", stdin);
    freopen("./output_file/check1", "w", stdout);
#endif
#ifdef TEST_CASE_2
    freopen("./input_file/input2_", "r", stdin);
    freopen("./output_file/check2", "w", stdout);
#endif
#ifdef TEST_CASE_3
    freopen("./input_file/input3_", "r", stdin);
    freopen("./output_file/check3", "w", stdout);
#endif
    check_right();
}
