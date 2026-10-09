# Q_3：链表队列

题目：把队列模块转换为链表实现。

## 结构与接口

queue.h 声明接口；queue.c 隐藏节点类型及队首、队尾、数量；main.c 为自动测试。

```text
front → [value|next] → [value|next] → [value|NULL] ← rear
```

沿用第二题 enqueue、dequeue、queue_peek、queue_size、destroy_queue 接口，并提供 queue_is_empty。链式实现按节点申请空间，无数组容量，不再提供 resize_queue 或 queue_capacity。没有固定容量不代表无限，malloc 失败或数量不能增加时入队返回 false。

节点归模块独占拥有，调用方不能接触或释放节点。enqueue 先检查 size_t 数量溢出，再申请一个节点，完整初始化后尾插；空队列首节点同时成为队首和队尾。申请失败不改变旧链接和数量。

dequeue 通过 int * 输出数据，返回 bool 表示成功；空队列或空输出指针失败且不修改输出。先读取数据和后继，再释放旧队首；删除最后节点必须把 rear 置 NULL。queue_peek 只读，不改变数量。destroy_queue 循环保存后继再释放所有节点，恢复空状态，可重复调用，之后可重新入队。

链接操作入队、出队、查看均为 O(1)，另有分配和释放成本；销毁为 O(n)，保存 n 个元素的空间为 O(n)，每个节点额外有指针及分配管理成本。单队列模块供串行调用，不提供并发保证。

## PowerShell 编译与运行

```powershell
Set-Location 'H:\study\MYCODE\codestudy\c_language_basics\17_abstract_data_types\Q_3'
& 'C:/mingw64/bin/gcc.exe' -std=c11 -g -Wall -Wextra -Wpedantic -Wshadow -Wconversion main.c queue.c -o main.exe
if ($LASTEXITCODE -eq 0) { .\main.exe }

& 'C:/mingw64/bin/gcc.exe' -std=c11 -g -Wall -Wextra -Wpedantic -Wshadow -Wconversion -DQUEUE_TEST main.c queue.c -o queue_test.exe
if ($LASTEXITCODE -eq 0) { .\queue_test.exe }
```

必须一起编译 main.c 和 queue.c，不链接其他题目的 main。程序自动测试，无需输入，失败打印 FAIL 并返回非零退出码。

## 实际测试记录

普通构建：`Checks: 1029, failures: 0`。
测试构建：`Checks: 2048, failures: 0`。
两种构建在上述警告选项下无警告。

覆盖初始空状态、空出队与输出保持、无效输出指针、首次入队、最后节点出队后重新入队、查看不移除、交错入出队、零/负数/重复值、1000 个元素 FIFO 顺序及数量、非空销毁、重复销毁与销毁后再用。

测试构建模拟首次和非空时 malloc 失败，验证状态和后续全序列保持；包装分配与释放函数计数，验证出队、销毁和最终未释放节点计数为零。计数不能替代完整内存检查器，不声称已验证所有非法访问。SIZE_MAX 数量保护已实现，但不通过分配 SIZE_MAX 个节点实际触发它。
