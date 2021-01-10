说在前面：由于这并不是一个稳定的版本，而且代码写的比较乱，所以可能存在诸多未知的bug。可以在issue提出或通过email联系。

**文件结构：**

```
│  globals.h
│  LL.cc
│  parse.cc
│  scan.cc
│  README.md
│
├─input
│      neg.tny
│      pos.tny
│      TINY_DEF
│
└─output
        LL1_result
        stack
        SyntaxTree
```

__globals.h__：包含头文件的声明，枚举类型的定义以及数据结构的定义

__LL.h__：该实验使用LL(1)文法，在该 c 文件中读入了产生式，计算了FIRST集，FOLLOW集，NULLABLE集和FIRST_S集，并构造了状态转移表。将最后各个集合以及状态转移表输出到 output/LL1_result 文件内。

（状态转移表第一列为产生式头部的非终结符，第一行为终结符，对应的单元为其在当前终结符token的情况下转移到的产生式序号）

__scan.cc__：重用的实验一的词法分析器部分

__syntaxtree.cc__：用来生成语法树的程序。如果在parse.cc内定义了 SYNTAX_TREE 宏，则会启用该程序

__parse.cc__：用来进行语法分析的程序。每一步的stack信息保存在 output/stack 内，格式如下：

```
read
identifier
stmt'
EOF
---------------
Token = reserved word: read
```

上方为stack的信息，顶行为栈顶；下方则是当前得到的token

在文件的末尾，如果输出YES，表示语法正确，否则会输出相关错误信息。

语法树保存在 output/SyntaxTree 内，不过具体的格式也没有很好看的形式，大概懂什么意思即可。每一列输出的为同一层，在该列的后面且该行的下面为其子结点，如：

```
|program
		|stmt-sequence
				|statement
						|read-stmt
								|reserved word: read
								|ID, name= \
				|stmt'
```

statement和stmt'是sibling结点，位于同一层，都是stmt-sequence的子结点；read-stmt是statement的子结点但不是stmt'的子结点


__input/TINY_DEF__：为输入的产生式（手动消除左递归和提取左公因子后）具体输入格式为：

```
产生式的数目n
2~n+1行为输入的产生式
非终结符个数m1
非终结符名称（m1个）
终结符个数m2
终结符名称（m2个）
```

由于终结符identifier定义的枚举类型为ID，但是在做词法分析时，由于其并不是保留字，所以会解析为ID，所以使用identifier即可；而对于NUM，则需要定义为数字。

空串用`$`表示

__input/*.tny__：输入的tny测试样例，pos为正例，neg为反例

__output/*__：输出，具体如前面描述
