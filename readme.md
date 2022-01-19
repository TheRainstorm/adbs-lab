### 说明

本项目为USTC高级数据库课程实验，需要实现一个缓冲区管理器

### 目录说明

- src/：缓冲区管理器的类代码，编译成一个库
- test/：测试代码，用于测试命中率等
- data-5w-50w-zipf.txt：包含了50万次满足Zipfan分布-0.8的页面请求

### 编译

```
cd test
make
```

### 运行

```bash
➜  test git:(master) ✗ ./test_BMgr
usage: ./test_BMgr --tracefile=string [options] ...
options:
  -l, --lru          use LRU replace algorithm
  -c, --clock        use clock replace algorithm
  -r, --random       use random replace algorithm
  -f, --tracefile    trace file name (string)
  -?, --help         print this message
```

### 参考

- 部分代码参考自：[zdszero/ustc-adbs-lab(github.com)](https://github.com/zdszero/ustc-adbs-lab)

- test/cmdline.h为第三方库，用于解析命令行参数