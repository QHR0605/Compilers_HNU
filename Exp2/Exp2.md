[toc]

# **实验二** **NFA确定化和DFA最小化（4学时）**

## **（一）NFA转DFA（2小时）**

#### **一、实验目的**

学习和掌握将NFA转为DFA的子集构造法。

#### **二、实验任务**

（1）存储NFA与DFA；

（2）编程实现子集构造法将NFA转换成DFA。

#### **三、实验内容**

1. 确定NFA与DFA的存储格式。

    要求为3个以上测试NFA准备好相应有限自动机的存储文件。（可利用实验一（二）的基础）

2. 用C或C++语言编写将NFA转换成DFA的子集构造法的程序。

3. 测试验证程序的正确性。

    测试不易。可求出NFA与DFA的语言集合的某个子集（如长度小于某个N），再证实两个语言集合完全相同！

4. 测试用例参考：将下列语言用RE表示，再转换成NFA使用：

    (a) 以a开头和结尾的小字字母串；a (a|b|…|z)*a | a

    (b) 不包含三个连续的b的，由字母a与b组成的字符串；(e | b | bb) (a | ab | abb)*

    (c)  ( a a | b ) * ( a | b b )\*

## **（二）DFA最小化（2小时）**

#### **一、实验目的**

学会编程实现等价划分法最小化DFA。

#### **二、实验任务**

先完善DFA，再最小化DFA。

#### **三、实验内容**

（1）准备3个以上测试DFA文件。（提示：其中一定要有没有最小化的DFA）

（2）用C或C++语言编写用等价划分法最小化DFA的程序。

（3）经测试无误。测试不易。可求出两个DFA的语言集合的某个子集（如长度小于某个N），再证实两个语言集合完全相同！

#### **四、通用NFA存储格式的建议（用以上的第三个测试NFA为例）**

![img](file:///C:/Users/a2783/AppData/Local/Temp/msohtmlclip1/01/clip_image001.png)

```
2 // 字符集中的字符个数 (以下两行也可合并成一行)
a b // 以空格分隔的字符集。
4 // 状态个数 (以下两行也可合并成一行)
1 2 3 4 // 状态编号。若约定总是用从1开始的连续数字表示，则此行可省略
1 // 开始状态的编号。若约定为1，则此行可省略
1 // 结束状态个数。若约定为1，则此行可省略
3 // 结束状态的编号
3 2 1 // 状态1的所有出去的转换。按字符集中的字符顺序给出，并在最左边加上一列关于e的转换。-1表示出错状态。若下一个状态有多个时，多个状态用逗号分隔。
-1 1 -1
-1 3 4
-1 -1 3
```

**五、集合状态的内部表示的建议**

使用示性函数。也即：

1. 含有单个状态的状态集合用2的幂次表示。即状态1 ~ N分别用数21 ~ 2N 表示。

2. 数的存储：若用32位整型(\__int32)、64位整型(__int64)存储，可分别表示32个或64个状态。更多的状态表示需要使用整型数组。

3. 含有多个状态的状态集合也用数来表示。若两个状态集合A与B用数表示为m和n，则状态集合AÈB与AÇB的数可用“位运算”表示，分别为m|n和m&n。

4. 若想知道某个状态集合A（用数m表示）中是否包含原先的第i个状态，也可使用基于“位运算”来判断：若（m | 2i ）> 0，则包含，否则不包含。 


实验通过测试后，按规定时间上交源代码、测试样例、输出文件（如有输出文件）和电子版实验报告。

 

## 实验步骤

#### 确定NFA和DFA的存储格式

NFA格式：

```
2       // 输入的字符集的个数
ab       // 字符集 中间无空格
14       // NFA状态总数n：编号从0开始到n-1，默认起始状态为0
1       // 终结状态个数
13       // 终结状态编号
1,7 -1 -1   // 以下n行输入当前状态的转换
2,4 -1 -1   // 无转换用-1代替
-1 3 -1    // 最左一侧为ε的转换
6 -1 -1    // 其余列按照输入的字符集的顺序填写
-1 -1 5    // 有多个转换状态时用 , 分隔(不加空格)
6 -1 -1
1,7 -1 -1
-1 8 -1
9,11 -1 -1
-1 10 -1
13 -1 -1
-1 -1 12
13 -1 -1
-1 -1 -1
```

DFA格式与NFA相同

#### 子集构造算法和DFA最小化算法的实现

##### 输入部分

###### NFA在程序中的存储：

```c++
const int MAXN = 105;

char c_set[MAXN]; // 字符集
int c_set_len;    // 字符集长度

int state[MAXN]; // 状态集合
int state_len;   // 状态个数

int done_state_nfa[MAXN]; // 结束状态集合
int done_state_nfa_len; // 结束状态个数
int start_state_nfa;	// 起始状态

unordered_map<char, vector<int>> NFA_STATE[MAXN]; // NFA转移表
```

集合的表示采用64位整形，每位代表一个状态，为1则该状态在集合中。

结束状态由于不止有一个，所以使用一个数组存储（这里的存储方式不再是每位表示一个状态集合，而是直接保存状态的编号），起始状态保存方式相同。

使用哈希表构造NFA的转移表，由于是非确定状态机，所以每一个状态对应的输出可能有多个转移，所以使用vetcor数组存放转移到的状态。具体表示含义如下：

```
NFA_STATE[i][c]: 第i个状态在输入为c的情况下的转移
```

###### 将输入格式转换为NFA的存储形式

```C++
    printf("输入字符集中的字符个数: ");
    scanf("%d", &c_set_len);
    c_set_len++;
    c_set[0] = '#';
    printf("输入字符集: (不要加空格)");
    scanf("%s", c_set + 1);
    c_set[c_set_len] = '\0';
```

读取字符集，首先读取字符的个数来分配空间，给第一个字符留空，表示$\epsilon$，使用#表示，其余依次读取输入的字符

```C++
	printf("输入状态个数: (状态从0递增开始)");
    scanf("%d", &state_len);
    for (int i = 0; i < state_len; i++) {
        state[i] = i;
    }

    printf("起始状态为0\n");
    start_state_nfa = 0;
```

状态的编号默认从0开始递增，所以只需输入状态的总数，默认起始状态为0

```C++
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
```

输入结束状态，要判断是否合法：即是不是存在的状态，不存在则不可输入，否则放入结束状态集中。

```C++
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
```

输入转移表的部分。首先以字符串的形式读入，因为要解析字符串，把以','分隔的数字读出。解析字符串时，判断有没有',‘（line 7-9），没有则直接进行转换，调用stoi函数转换为整型；如果有，则循环寻找','，并且切割出子串，转换为整型，直到没有','，在剩余字符串中（line 10-18），最后将这一个vector数组放到对应的状态的输入转移中。

读入部分完成。



##### 子集构造算法

###### DFA在程序中的存储

```C++
int done_state_dfa[MAXN]; // 结束状态集合
int done_state_dfa_len;
int start_state_dfa[MAXN]; // 其实只有一个起始状态，这里没必要使用数组。
int start_dfa_len;

unordered_map<char, int> DFA_STATE[MAXN]; // DFA状态
unordered_map<int, int> DFA_NFA;
int mark[MAXN];
int dfa_state_len;
```

与NFA不同的是，由于DFA是确定的，所以对于某一个字符的转移只有一个下一状态。

DFA_NFA表示NFA的状态和DFA的状态相对应的形式，即一个DFA状态是由哪个NFA状态得到的。

mark数组表示DFA的状态有没有被标记过

###### 算法步骤

>从初始状态出发，计算$\epsilon-closure$，作为新的状态A
>
>标记状态A，对每个输入求$move(A, c)$，再求其$\epsilon-closure$，如果是个新的状态，则加入到DFA状态集合中，同时更新$Dtran[A,c] = B$状态转移表
>
>当所有状态都被标记后，算法结束。

###### $\epsilon-closure$计算方法

```C++
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
```

首先从状态集合state中取出包含的状态（line 5-10），之后，对每个状态求$\epsilon$的转移，根据NFA转移表得到下一状态，加入到状态集合ans中（line 11 - 17），最后返回状态集合ans

由于这只是单步的$\epsilon-closure$，而要求的为无限多步，所以

```C++
    int closure = epsilon_closure(init_state);
    while (closure != epsilon_closure(closure)) {
        closure = epsilon_closure(closure);
    }
```

一直计算，直到闭包不再变化，此时得到了所有的$\epsilon-closure$

###### $move$计算方法

```C++
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
```

与$\epsilon-closure$相似，只不过对于转移得到的状态，不再是$\epsilon$，而是对于当前的输入字符。同样的，先从状态集合state中抽出状态放到state_中，之后找到转移得到的状态，放入新状态集合ans中，返回ans

###### 子集构造算法具体实现

初始化部分：

```C++
    done_state_dfa_len = 0;
    dfa_state_len = 0;
    start_dfa_len = 0;
    memset(mark, 0, sizeof(mark));
    set<int> already_exist;
    int init_state = 1;
```

首先将DFA相关的值赋值为0，将mark数组置0，mark数组用于标记是不是还有DFA的状态没被标记，already_exist集合用于判断是不是已经存在当前状态；init_state为1，由于起始状态为0，所以状态集合的最低位为1，十进制表示为1。

```C++
    int closure = epsilon_closure(init_state);
    while (closure != epsilon_closure(closure)) {
        closure = epsilon_closure(closure);
    }
```

初始的计算闭包

```C++
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
```

首先判断这一个$\epsilon-closure$是不是结束状态，如果包含了NFA的结束状态，那么对应的DFA的结束状态要进行设置，对于起始状态的处理也是同样的。（line 1-9）

之后，DFA的0状态对应于这一个$\epsilon-closure$，放到DFA_NFA中

```C++
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

```

这里进行while循环，如果所有DFA状态都被标记，那么可以结束，all_marked为1，结束循环。否则找到第一个未标记的状态进行标记。

```C++
        int now_nfa_state = DFA_NFA[i]; // 得到对应的NFA状态
        already_exist.insert(now_nfa_state);
```

通过找到的第一个未被标记的状态得到NFA对应的状态集合，并将其插入到已存在中，那么接下来就是对其求$\epsilon-closure$和move步了。

```C++
        for (int j = 1; j < c_set_len; j++) {              // 遍历所有输入
            int new_state = move(now_nfa_state, c_set[j]); // move步
            int closure = epsilon_closure(new_state);      // closure步
            while (closure != epsilon_closure(closure)) {
                closure = epsilon_closure(closure);
            }
```

首先求对输入的move步，得到新状态再求$\epsilon-closure$。

```C++
			if (closure == 0) { // 没有对应的转换
                DFA_STATE[i][c_set[j]] = {-1};
            }
```

如果closure = 0的话，表示没有对应的转换，那么将转移表中该项设置为-1

```c++
			else {
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
```

否则的话，如果当前的状态不存在，则发现了一个新状态，插入到set中，并且判断是不是结束状态，是不是起始状态（line 4-12），再将他对应的DFA，NFA状态插入DFA_NFA的对应关系表中。

```C++
                // 向当前状态当前输入的转换中添加新状态
                int k;
                for (k = 0; k < dfa_state_len; k++) { // 找要添加的DFA状态
                    if (DFA_NFA[k] == closure) {
                        break;
                    }
                }
                DFA_STATE[i][c_set[j]] = k;
```

在这之后，就可以更新DFA的转移表，首先找到这一个新状态 k 的编号，放到状态 i 在 c[j] 的转移下

###### 完整代码

```c++
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
```

验证部分放在最后

##### DFA最小化算法

###### 最小化DFA在程序中的存储

```C++
int done_state_mindfa[MAXN];
int done_state_mindfa_len;
int start_state_mindfa[MAXN];
int start_mindfa_len;

unordered_map<char, int> minDFA_STATE[MAXN]; // minDFA状态
unordered_map<int, int> minDFA_DFA;			// minDFA和DFA的对应关系
int fa[MAXN];								// 用于DFA集合的划分
int minDFA_len;
```

这里多增加了一个fa数组，用于在做最小化时，判断一个集合内的这些DFA状态转移方向是否相同。

其余与之前DFA相同。

###### 算法步骤

![image-20201126170839636](C:\Users\a2783\AppData\Roaming\Typora\typora-user-images\image-20201126170839636.png)

###### 接受状态和非接受状态

```C++
int findEnd() {
    int ans = 0;
    for (int i = 0; i < done_state_dfa_len; i++) {
        ans |= (1 << done_state_dfa[i]);
    }
    return ans;
}
```

由于最初的划分要分为这两个状态集，所以根据DFA的结束状态，很轻松的就可以找到一个结束状态集，根据存储状态的方式和集合的性质，可以使用：

```C++
    int end = findEnd();
    int netend = (1 << (dfa_state_len)) - 1 - end;
```

来得到非接收状态。

###### 初始化部分

```c++
    if (netend == 0) {
        minDFA_DFA[0] = end;
        minDFA_len = 1;
    } else {
        minDFA_DFA[0] = netend;
        minDFA_DFA[1] = end;
        minDFA_len = 2;
    }
```

由于存在一种可能：只有接收状态，所以要进行判断。

###### fa数组的作用

```C++
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
```

从最小化DFA和DFA对应的关系中，可以得到minDFA状态的编号，将其对应的DFA的状态的fa值均设置为其编号，用于在之后进行判断这一个DFA集是不是可分得。这里只需要注意fa数组的含义：fa[i] 编号为i的DFA状态在哪个minDFA状态集中

###### DFA最小化算法具体实现

```C++
    for (int i = 0; i < minDFA_len; i++) {
        if (divide(minDFA_DFA[i], i)) {
            i--;
        }
    }
```

当所有集合均不可分时，则结束，否则当前仍是可分得，所以使i - 1，相当于回溯的过程。

判断划分以及是否可分的实现：

```C++
int divide(int state, int idx) {
    vector<int> divide_list[minDFA_len + 1];

    vector<int> state_;
    int cnt = 0;
    for (int i = 1; i <= (1 << state_len); i <<= 1, cnt++) {
        if (state & i) {
            state_.push_back(cnt);
        }
    }
```

从当前的minDFA状态集合中抽取出DFA的状态，放到state_中

```C++
    int ret = 0;
    for (int c = 1; c < c_set_len; c++) {
        for (int i = 0; i <= minDFA_len; i++) {
            divide_list[i].clear();
        }
        char input = c_set[c];
```

对于每个输入input，进行判断其集合是不是可分

```C++
        for (auto v : state_) {
            int nxt = DFA_STATE[v][input];
            if (nxt == -1) {
                divide_list[minDFA_len].push_back(v);
                continue;
            }
            int set = fa[nxt];
            divide_list[set].push_back(v);
        }
```

对于每一个状态，得到其在当前输入下的转移nxt，要注意的是，nxt = -1即没有转移时，也算是一种可分情况，这时将其放入到divide_list的最后一个元素而不是放到fa[nxt]的位置；否则都放入divide_list中fa[nxt]的位置，表示这些DFA状态转移到了fa[nxt]的minDFA状态。

```C++
		int cnt = 0;
        for (int i = 0; i <= minDFA_len; i++) {
            if (divide_list[i].size() != 0)
                cnt++;
        }
        if (cnt == 1) {
            continue;
        }
```

之后判断，如果只有一个divide_list不为空，则表示不可分，继续查看下一个输入。

```C++
		else {
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
```

否则可分，对于找到的第一个divide_list新集合，把他放到当前的集合的位置，因为当前集合已经被拆开了，之后对于每一个新集合，扩张minDFA状态，放入其中。并在最后更新fa数组，返回1，进行回溯。cal_Set是根据状态计算状态集合

如果最后返回了0，那么表示不可分，算法结束。

###### 转移表和接收状态起始状态的获取

```C++
void getTrans() {
    for (int i = 0; i < minDFA_len; i++) {
        int set = minDFA_DFA[i];
        int cnt = 0;
        int ret = 0;
        for (int k = 1; k <= (1 << dfa_state_len); k <<= 1, cnt++) {
            if (set & k)
                break;
        }
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
```

对于转移表，抽取出minDFA对应的当前DFA的状态集合，这一部分由于对于每一个输入，所有状态的转移是一样的，所有只需找一个DFA状态即可。之后得到DFA转移表中，当前状态在当前输入下的转移，找到其fa的位置，即对应的minDFA的转移，放入转移表中。

```C++
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
```

获取接收状态和起始状态与DFA相同，不再细说。

###### 完整代码

```C++
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
```

#### 实验结果验证

##### 测试样例1：

正则表达式：(a|b)\*abb(a|b)*

NFA输入

```
2
ab
18
1
17
1,7 -1 -1
2,4 -1 -1
-1 3 -1
6 -1 -1
-1 -1 5
6 -1 -1
1,7 -1 -1
-1 8 -1
-1 -1 9
-1 -1 10
11,17 -1 -1
12,14 -1 -1
-1 13 -1
16 -1 -1
-1 -1 15
16 -1 -1
11,17 -1 -1
-1 -1 -1
```

输出的转移表及对应关系

```
NFA转移表
	#	a	b	
0	1,7	-1	-1	
1	2,4	-1	-1	
2	-1	3	-1	
3	6	-1	-1	
4	-1	-1	5	
5	6	-1	-1	
6	1,7	-1	-1	
7	-1	8	-1	
8	-1	-1	9	
9	-1	-1	10	
10	11,17	-1	-1	
11	12,14	-1	-1	
12	-1	13	-1	
13	16	-1	-1	
14	-1	-1	15	
15	16	-1	-1	
16	11,17	-1	-1	
17	-1	-1	-1	
DONE STATE: 17 
START STATE: 0

DFA-NFA
DFA	NFA	
0	{0,1,2,4,7,}
1	{1,2,3,4,6,7,8,}
2	{1,2,4,5,6,7,}
3	{1,2,4,5,6,7,9,}
4	{1,2,4,5,6,7,10,11,12,14,17,}
5	{1,2,3,4,6,7,8,11,12,13,14,16,17,}
6	{1,2,4,5,6,7,11,12,14,15,16,17,}
7	{1,2,4,5,6,7,9,11,12,14,15,16,17,}
8	{1,2,4,5,6,7,10,11,12,14,15,16,17,}

DFA转移表
	a	b	
0	1	2	
1	1	3	
2	1	2	
3	1	4	
4	5	6	
5	5	7	
6	5	6	
7	5	8	
8	5	6	
DONE STATE: 4 5 6 7 8 
START STATE: 0 

minDFA-DFA
minDFA	DFA	
0	{0,2,}
1	{4,5,6,7,8,}
2	{3,}
3	{1,}

minDFA转移表
	a	b	
0	3	0	
1	1	1	
2	3	1	
3	3	2	
DONE STATE: 1 
START STATE: 0 
```

使用随机生成样例进行测试：

```C++
/* (a|b)*abb(a|b)* */
#include <iostream>
#include <random>
#include <time.h>
using namespace std;

int main() {
    srand(time(0));
    freopen("../input_file/input3_", "w", stdout);
    char s[2] = {'a', 'b'};
    for (int i = 0; i <= 4; i++) {
        for (int times = 0; times < 3; times++) {
            for (int j = 0; j < i; j++) {
                int choice = rand() % 2;
                cout << s[choice];
            }
            cout << "abb";
            for (int j = 0; j < i; j++) {
                int choice = rand() % 2;
                cout << s[choice];
            }
        }
        cout << endl;
    }
    for (int num = 0; num < 9; num++) {
        int times = rand() % 15 + 1;
        for (int i = 0; i < times; i++) {
            int choice = rand() % 2;
            cout << s[choice];
        }
        cout << endl;
    }
}
```

生成的输入字符串：

```
abbabbabb
babbbaabbababba
aaabbaabbabbbbbbabbba
bababbaabaababbaabbababbaba
bbbaabbbbabbabbabbbabaabbbabbbbba
ba
aaaaaaababaaa
babbaaa
aabaab
bbaaabbbbbaa
b
babbaaabbbba
ab
bbbabaa
```

输出结果：

```
abbabbabb:
NFA ACCEPT
DFA ACCEPT
minDFA ACCEPT

babbbaabbababba:
NFA ACCEPT
DFA ACCEPT
minDFA ACCEPT

aaabbaabbabbbbbbabbba:
NFA ACCEPT
DFA ACCEPT
minDFA ACCEPT

bababbaabaababbaabbababbaba:
NFA ACCEPT
DFA ACCEPT
minDFA ACCEPT

bbbaabbbbabbabbabbbabaabbbabbbbba:
NFA ACCEPT
DFA ACCEPT
minDFA ACCEPT

ba:
NFA FAIL
DFA FAIL
minDFA FAIL

aaaaaaababaaa:
NFA FAIL
DFA FAIL
minDFA FAIL

babbaaa:
NFA ACCEPT
DFA ACCEPT
minDFA ACCEPT

aabaab:
NFA FAIL
DFA FAIL
minDFA FAIL

bbaaabbbbbaa:
NFA ACCEPT
DFA ACCEPT
minDFA ACCEPT

b:
NFA FAIL
DFA FAIL
minDFA FAIL

babbaaabbbba:
NFA ACCEPT
DFA ACCEPT
minDFA ACCEPT

ab:
NFA FAIL
DFA FAIL
minDFA FAIL

bbbabaa:
NFA FAIL
DFA FAIL
minDFA FAIL
```

可以看出三种状态机同为ACCEPT或FAIL。

##### 测试样例2：

正则表达式：(a|b)*a(a|b)

NFA输入

```
2
ab
14
1
13
1,7 -1 -1
2,4 -1 -1
-1 3 -1
6 -1 -1
-1 -1 5
6 -1 -1
1,7 -1 -1
-1 8 -1
9,11 -1 -1
-1 10 -1
13 -1 -1
-1 -1 12
13 -1 -1
-1 -1 -1
```

输出的转移表及对应关系

```
NFA转移表
	#	a	b	
0	1,7	-1	-1	
1	2,4	-1	-1	
2	-1	3	-1	
3	6	-1	-1	
4	-1	-1	5	
5	6	-1	-1	
6	1,7	-1	-1	
7	-1	8	-1	
8	9,11	-1	-1	
9	-1	10	-1	
10	13	-1	-1	
11	-1	-1	12	
12	13	-1	-1	
13	-1	-1	-1	
DONE STATE: 13 
START STATE: 0

DFA-NFA
DFA	NFA	
0	{0,1,2,4,7,}
1	{1,2,3,4,6,7,8,9,11,}
2	{1,2,4,5,6,7,}
3	{1,2,3,4,6,7,8,9,10,11,13,}
4	{1,2,4,5,6,7,12,13,}

DFA转移表
	a	b	
0	1	2	
1	3	4	
2	1	2	
3	3	4	
4	1	2	
DONE STATE: 3 4 
START STATE: 0 

minDFA-DFA
minDFA	DFA	
0	{0,2,}
1	{3,}
2	{1,}
3	{4,}

minDFA转移表
	a	b	
0	2	0	
1	1	3	
2	1	3	
3	2	0	
DONE STATE: 1 3 
START STATE: 0 
```

使用随机生成样例进行测试：

```C++
/* (a|b)*a(a|b) */
#include <iostream>
#include <random>
#include <time.h>
using namespace std;

int main() {
    srand(time(0));
    freopen("../input_file/input1_", "w", stdout);
    char s[2] = {'a', 'b'};
    for (int i = 0; i <= 4; i++) {
        for (int times = 0; times < 3; times++) {
            for (int j = 0; j < i; j++) {
                int choice = rand() % 2;
                cout << s[choice];
            }
            cout << 'a';
            int choice = rand() % 2;
            cout << s[choice] << endl;
        }
    }
    for (int num = 0; num < 8; num++) {
        int times = rand() % 7 + 1;
        for (int i = 0; i < times; i++) {
            int choice = rand() % 2;
            cout << s[choice];
        }
        cout << endl;
    }
}
```

生成的输入字符串：

```
ab
ab
aa
bab
aaa
bab
bbaa
aaab
aaaa
bbbab
abbab
abbaa
aabbaa
abaaaa
baaaaa
bb
bbaa
abbbb
bbb
ba
a
bbaba
aba
```

输出结果：

```
ab:
NFA ACCEPT
DFA ACCEPT
minDFA ACCEPT

ab:
NFA ACCEPT
DFA ACCEPT
minDFA ACCEPT

aa:
NFA ACCEPT
DFA ACCEPT
minDFA ACCEPT

bab:
NFA ACCEPT
DFA ACCEPT
minDFA ACCEPT

aaa:
NFA ACCEPT
DFA ACCEPT
minDFA ACCEPT

bab:
NFA ACCEPT
DFA ACCEPT
minDFA ACCEPT

bbaa:
NFA ACCEPT
DFA ACCEPT
minDFA ACCEPT

aaab:
NFA ACCEPT
DFA ACCEPT
minDFA ACCEPT

aaaa:
NFA ACCEPT
DFA ACCEPT
minDFA ACCEPT

bbbab:
NFA ACCEPT
DFA ACCEPT
minDFA ACCEPT

abbab:
NFA ACCEPT
DFA ACCEPT
minDFA ACCEPT

abbaa:
NFA ACCEPT
DFA ACCEPT
minDFA ACCEPT

aabbaa:
NFA ACCEPT
DFA ACCEPT
minDFA ACCEPT

abaaaa:
NFA ACCEPT
DFA ACCEPT
minDFA ACCEPT

baaaaa:
NFA ACCEPT
DFA ACCEPT
minDFA ACCEPT

bb:
NFA FAIL
DFA FAIL
minDFA FAIL

bbaa:
NFA ACCEPT
DFA ACCEPT
minDFA ACCEPT

abbbb:
NFA FAIL
DFA FAIL
minDFA FAIL

bbb:
NFA FAIL
DFA FAIL
minDFA FAIL

ba:
NFA FAIL
DFA FAIL
minDFA FAIL

a:
NFA FAIL
DFA FAIL
minDFA FAIL

bbaba:
NFA FAIL
DFA FAIL
minDFA FAIL

aba:
NFA FAIL
DFA FAIL
minDFA FAIL
```



通过上面测试可以看出，程序正确



#### 关于源码使用提示 或 见README.md

__generator:__ 随机生成对应的正则表达式的正例字符串和反例字符串

生成哪一组测试样例即调用相应的 __input*\_roll.cc__ 程序即可

__input_file:__ 输入文件

__input*:__ 输入的NFA图

输入格式：

```
2              // 输入的字符集的个数
ab             // 字符集 中间无空格
14             // NFA状态总数n：编号从0开始到n-1，默认起始状态为0
1              // 终结状态个数
13             // 终结状态编号
1,7 -1 -1      // 一下n行输入当前状态的转换
2,4 -1 -1      // 无转换用-1代替
-1 3 -1        // 最左一侧为ε的转换
6 -1 -1        // 其余列按照输入的字符集的顺序填写
-1 -1 5        // 有多个转换状态时用 , 分隔(不加空格)
6 -1 -1
1,7 -1 -1
-1 8 -1
9,11 -1 -1
-1 10 -1
13 -1 -1
-1 -1 12
13 -1 -1
-1 -1 -1
```

__input*_:__ 构造出的测试样例

__output_file:__ 输出文件

__out*:__ 输出的NFA DFA minDFA的状态转移表，以及对应的状态集合，终结状态集合

__check*:__ 用于比对输入的测试样例能否被接受，ACCEPT为接受，FAIL为不可接受

__Exp2.cc:__ 实验二文件

使用时，请注意宏定义的声明，使用哪一组输入文件即定义如:

```c
// 使用input1以及input1_:
#define TEST_CASE_1
// 使用input2以及input2_:
#define TEST_CASE_2
// 使用input3以及input3_:
#define TEST_CASE_3
```

只能声明1 2 3中的一个



#### 完整实验代码

```C++
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
```

