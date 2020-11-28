#ifndef _GLOBALS_H_
#define _GLOBALS_H_
#include "globals.h"
using namespace std;
#endif

/* LL.cc */
class LL {
  private:
    int prod_num;
    int nonterminal_num;
    int terminal_num;

    vector<string> prod_;
    set<string> nonterminal;
    vector<TokenType> terminal;

    set<string> nullable;
    unordered_map<string, set<TokenType>> first;
    unordered_map<string, set<TokenType>> follow;
    unordered_map<int, set<TokenType>> first_s;
/* scan.cc */
    int getNext_(int &buf_pos, string s) {
        if (buf_pos < s.length()) {
            return s[buf_pos++];
        } else {
            return EOF;
        }
    }
    void backtrace_(int &buf_pos, char c) {
        buf_pos--;
    }
    TokenType getRealToken_() {
        int i;
        for (i = 0; i < 8; i++) {
            if (currString == reserved[i].str) {
                return reserved[i].token;
            }
        }
        return ID;
    }

    TokenType getToken_(string s) {
        int pos = 0;

        StateType state = START;
        TokenType currToken;
        int save = 0;
        while (state != DONE) {
            int c = getNext_(pos, s);
            switch (state) {
            case START:
                if (c == ' ' || c == '\t' || c == '\n') {
                    state = START;
                } else if (c == '{') {
                    state = INCOMMENT;
                } else if (isdigit(c)) {
                    currString[name_pos++] = c;
                    save = 1;
                    state = INNUM;
                } else if (isalpha(c)) {
                    currString[name_pos++] = c;
                    save = 1;
                    state = INID;
                } else if (c == ':') {
                    currString[name_pos++] = c;
                    state = INASSIGN;
                } else {
                    state = DONE;
                    switch (c) {
                    case EOF:
                        currToken = ENDFILE;
                        break;
                    case '+':
                        currToken = PLUS;
                        break;
                    case '-':
                        currToken = MINUS;
                        break;
                    case '*':
                        currToken = TIMES;
                        break;
                    case '/':
                        currToken = DIVIDE;
                        break;
                    case '=':
                        currToken = EQULE;
                        break;
                    case '<':
                        currToken = LE;
                        break;
                    case '(':
                        currToken = LB;
                        break;
                    case ')':
                        currToken = RB;
                        break;
                    case ';':
                        currToken = SEMI;
                        break;
                    default:
                        currString[name_pos++] = c;
                        currToken = ERROR;
                    }
                }
                break;
            case INNUM:
                if (!isdigit(c)) {
                    backtrace_(pos, c);
                    state = DONE;
                    currToken = NUM;
                } else
                    currString[name_pos++] = c;
                break;
            case INID:
                if (!isalpha(c)) {
                    backtrace_(pos, c);
                    state = DONE;
                    currToken = ID;
                } else
                    currString[name_pos++] = c;
                break;
            case INASSIGN:
                if (c == '=') {
                    state = DONE;
                    currToken = ASSIGN;
                } else {
                    backtrace_(pos, c);
                    state = DONE;
                    currToken = ERROR;
                }
                break;
            case INCOMMENT:
                if (c == EOF) {
                    state = DONE;
                    currToken = ENDFILE;
                } else if (c == '}') {
                    state = START;
                }
                break;
            case DONE:
                break;
            default:
                state = DONE;
                currString[name_pos++] = c;
                currToken = ERROR;
            }
            if (state == DONE) {
                currString[name_pos] = '\0';
                if (currToken == ID) {
                    currToken = getRealToken_();
                }
                name_pos = 0;
            }
        }
        return currToken;
    }
    void printResult_(TokenType curr) {
        switch (curr) {
        case IF:
            printf("if\t");
            break;
        case THEN:
            printf("then\t");
            break;
        case ELSE:
            printf("else\t");
            break;
        case END:
            printf("end\t");
            break;
        case REPEAT:
            printf("repeat\t");
            break;
        case UNTIL:
            printf("until\t");
            break;
        case READ:
            printf("read\t");
            break;
        case WRITE:
            printf("write\t");
            break;
        case ASSIGN:
            printf(":=\t");
            break;
        case EQULE:
            printf("=\t");
            break;
        case LE:
            printf("<\t");
            break;
        case PLUS:
            printf("+\t");
            break;
        case MINUS:
            printf("-\t");
            break;
        case TIMES:
            printf("*\t");
            break;
        case DIVIDE:
            printf("/\t");
            break;
        case LB:
            printf("(\t");
            break;
        case RB:
            printf(")\t");
            break;
        case SEMI:
            printf(";\t");
            break;
        case NUM:
            printf("number\t");
            break;
        case ID:
            printf("identifier\t");
            break;
        case ENDFILE:
            printf("EOF\t");
            break;
        case ERROR:
            printf("Error\t");
            break;
        default:
            printf("Error\t");
        }
    }
/* LL.cc */
    void LL_getSet() {
        get_nullable();
        check_nullable();
        get_First();
        check_First();
        get_Follow();
        check_Follow();
        get_First_s();
        check_first_s();
    }

    void check_Transtable() {
        cout << setw(15) << " |";
        for (auto v : terminal) {
            printResult_(v);
            cout << setw(15) << "|";
        }
        cout << endl;
        for (auto idn : LL1_table) {
            cout << setw(15) << idn.first << "|";
            for (auto v : terminal) {
                for (auto vec : (idn.second)[v]) {
                    cout << vec << ",";
                }
                cout << setw(15) << "|";
            }
            cout << endl;
        }
    }
    void get_Transtable() {
        for (auto it : first_s) {
            int idx = it.first;
            string head = prod[idx][0];
            for (auto trans : it.second) {
                LL1_table[head][trans].push_back(idx);
            }
        }
    }

    vector<string> parse(const string &production) {
        vector<string> ans;
        int i;
        int before = 0;
        for (i = 0; i < production.length(); i++) {
            if (production[i] == '-' && production[i + 1] == '>') {
                string head = production.substr(0, i - 1);
                ans.push_back(head);
                i += 3;
                before = i;
                break;
            }
        }
        for (; i < production.length(); i++) {
            if (production[i] == ' ') {
                string nxt = production.substr(before, i - before);
                ans.push_back(nxt);
                before = i + 1;
            }
        }
        string nxt = production.substr(before, i - before);
        ans.push_back(nxt);
        return ans;
    }

    void get_nullable() {
        for (auto s : prod_) {
            int end;
            for (int i = 0; i < s.length(); i++) {
                if (s[i] == '-' && s[i + 1] == '>')
                    end = i - 1;
                if (s[i] == '$') {
                    string ret = s.substr(0, end);
                    // cout << ret << endl;
                    nullable.insert(ret);
                }
            }
        }
        return;
    }

    void get_First() {
        int flag = 1;
        while (flag) {
            flag = 0;
            for (int i = 0; i < prod_num; i++) {
                string head = prod[i][0];
                set<TokenType> origin = first[head];
                int size = origin.size();
                for (int j = 1; j < prod[i].size(); j++) {
                    if (prod[i][j] == "$")
                        continue;
                    string nxt = prod[i][j];
                    if (nonterminal.find(nxt) != nonterminal.end()) {
                        // cout << "I got it!\n";
                        set<TokenType> temp = first[nxt];
                        set_union(
                            origin.begin(), origin.end(), temp.begin(), temp.end(),
                            inserter(origin, origin.begin()));
                        if (nullable.find(nxt) == nullable.end()) {
                            break;
                        }
                    } else {
                        TokenType token = get_Terminal(nxt, head);
                        origin.insert(token);
                        break;
                    }
                }
                if (origin.size() != size)
                    flag = 1;
                first[head] = origin;
            }
        }
    }

    void get_Follow() {
        int flag = 1;
        while (flag) {
            flag = 0;
            for (int i = 0; i < prod_num; i++) {
                string head = prod[i][0];
                set<TokenType> temp = follow[head];

                for (int j = prod[i].size() - 1; j >= 1; j--) {
                    if (prod[i][j] == "$")
                        continue;
                    string nxt = prod[i][j];
                    if (nonterminal.find(nxt) != nonterminal.end()) {
                        set<TokenType> upd = follow[nxt];
                        int size = upd.size();
                        set_union(
                            temp.begin(), temp.end(), upd.begin(), upd.end(),
                            inserter(upd, upd.begin()));
                        if (upd.size() != size) {
                            flag = 1;
                        }
                        follow[nxt] = upd;
                        if (nullable.find(nxt) == nullable.end()) {
                            temp.clear();
                            temp = first[nxt];
                        } else {
                            set<TokenType> ret = first[nxt];
                            set_union(
                                temp.begin(), temp.end(), ret.begin(), ret.end(),
                                inserter(temp, temp.begin()));
                        }
                    } else {
                        TokenType token = get_Terminal(nxt, head);
                        temp.clear();
                        temp.insert(token);
                    }
                }
            }
        }
    }

    void cal_First_s(int i) {
        string head = prod[i][0];
        set<TokenType> origin = first_s[i];
        for (int j = 1; j < prod[i].size(); j++) {
            string nxt = prod[i][j];
            if (nxt == "$")
                continue;
            if (nonterminal.find(nxt) != nonterminal.end()) {
                set<TokenType> temp = first[nxt];
                set_union(
                    temp.begin(), temp.end(), origin.begin(), origin.end(),
                    inserter(origin, origin.begin()));
                first_s[i] = origin;
                if (nullable.find(nxt) == nullable.end()) {
                    return;
                }
            } else {
                TokenType token = get_Terminal(nxt, head);
                origin.insert(token);
                first_s[i] = origin;
                return;
            }
        }
        set<TokenType> fl = follow[head];
        set_union(
            fl.begin(), fl.end(), origin.begin(), origin.end(), inserter(origin, origin.begin()));
        first_s[i] = origin;
    }
    void get_First_s() {
        for (int i = 0; i < prod_num; i++) {
            cal_First_s(i);
        }
    }

    void check_nullable() {
        cout << endl;
        cout << "NULLABLE: ";
        for (auto v : nullable) {
            cout << v << "\t";
        }
        cout << endl;
    }
    void check_First() {
        cout << endl;
        for (auto it : first) {
            cout << "nonterminal: " << it.first << endl;
            cout << "FIRST: ";
            for (auto c : it.second) {
                printResult_(c);
            }
            cout << endl;
        }
    }
    void check_Follow() {
        cout << "\n";
        for (auto it : follow) {
            cout << "nonterminal: " << it.first << endl;
            cout << "FOLLOW: ";
            for (auto c : it.second) {
                printResult_(c);
            }
            cout << endl;
        }
    }
    void check_first_s() {
        cout << endl;
        for (int i = 0; i < prod_num; i++) {
            cout << "production: " << prod_[i] << endl;
            cout << "FIRST_S: ";
            for (auto c : first_s[i]) {
                printResult_(c);
            }
            cout << endl;
        }
    }

  public:
    unordered_map<int, vector<string>> prod;
    unordered_map<string, unordered_map<TokenType, vector<int>>> LL1_table;
    LL() {
        freopen("input/TINY_DEF", "r", stdin);
        cin >> prod_num;
        // prod_num++;
        string rr;
        getline(cin, rr);
        for (int i = 0; i < prod_num; i++) {
            string ret;
            getline(cin, ret);
            prod_.push_back(ret);
            prod[i] = parse(ret);
        }
        cin >> nonterminal_num;
        for (int i = 0; i < nonterminal_num; i++) {
            string ret;
            cin >> ret;
            nonterminal.insert(ret);
        }
        cin >> terminal_num;
        for (int i = 0; i < terminal_num; i++) {
            string ret;
            getline(cin, ret);
            TokenType token = getToken_(ret);
            terminal.push_back(token);
        }
        fclose(stdin);
        cin.clear();
        freopen("CON", "r", stdin);
        freopen("output/check", "w", stdout);
        LL_getSet();
        get_Transtable();
        check_Transtable();
        fclose(stdout);
        cout.clear();
        freopen("CON", "w", stdout);
    }
    set<string> get_nonterminal() {
        return nonterminal;
    }
    TokenType get_Terminal(string M, string head="IDN") {
        TokenType token;
        if (M == "number") {
            token = NUM;
        } else if (M == "EOF") {
            token = ENDFILE;
        } else {
            token = getToken_(M);
            if (token == ERROR) {
                cout << "head: " << head << endl;
                cout << "nxt: " << M << endl;
                cout << "ERROR!!";
                exit(0);
            }
        }
        return token;
    }
};

// int main() {
//     LL l;
// }