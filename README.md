主要完成的部分：`analyse.c` 和 `symtab.c`

`analyse.c`为语义分析

`symtab.c`为符号表

通过`main.c`加载之前所做的各部分代码并执行语义分析

使用：

make进行整体的编译，生成scanner可执行文件

./scanner xxx.c- 对 xxx.c-程序进行语义分析