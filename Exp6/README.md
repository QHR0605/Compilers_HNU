### 如何运行

使用shell命令:

```sh
$ make
$ ./cminus test/xxxx.cm
$ ./tm test/xxxx.tm
```

或者使用shell脚本（其实就是上面的命令）

通过make生成 cminus 和 tm
将原 .cm 文件通过 cminus 生成 .tm 文件
将 .tm 文件通过 tm 虚拟机来进行执行