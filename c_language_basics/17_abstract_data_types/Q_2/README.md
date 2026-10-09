# Q_2：动态数组循环队列与 resize_queue

题目：把队列模块转换为动态分配数组，并增加类似第一题的 resize_queue。

## 文件与接口

queue.h 声明接口，queue.c 管理单个整数队列，main.c 执行自动检查。

`bool resize_queue(size_t new_length)` 接受一个参数：新容量，单位为元素个数。有效数量不变；容量小于数量则拒绝；相同容量直接成功；空队列缩零释放空间。调用方提供非负容量，有符号输入先验证再转换。enqueue 满时失败，不自动扩容；dequeue/queue_peek 返回成功状态，通过输出参数传递整数，失败不修改输出。

## 为什么不直接 realloc

回绕后物理数组可能是 `[60,70,30,40,50]`，逻辑顺序却是 30、40、50、60、70。改变容量会改变回绕边界，单纯 realloc 无法保证这个逻辑顺序。

实现先检查元素数乘字节数的溢出，然后 malloc 新数组，按旧 front 和旧容量依次复制有效元素。复制完成后释放旧数组、更新容量，front 设为 0；rear 为 size，恰好满时 rear 设为 0。失败发生在修改旧数组之前，原状态完整保留。

非同容量且非零的 resize 成本为 O(n)，n 为有效元素数量；入队、出队、查看队首 O(1)。扩缩容期间需要同时容纳旧、新数组，额外空间与新容量成正比。模块拥有并释放数组，调用方不能访问内部地址；不提供多线程或中断并发保证。

## 编译与运行

在本题目录执行，必须显式编译两个源文件：

```powershell
& 'C:/mingw64/bin/gcc.exe' -std=c11 -g -Wall -Wextra -Wpedantic -Wshadow -Wconversion main.c queue.c -o main.exe
if ($LASTEXITCODE -eq 0) { .\main.exe }

& 'C:/mingw64/bin/gcc.exe' -std=c11 -g -Wall -Wextra -Wpedantic -Wshadow -Wconversion -DQUEUE_TEST main.c queue.c -o queue_test.exe
if ($LASTEXITCODE -eq 0) { .\queue_test.exe }
```

测试构建以可控 malloc 接口让指定一次请求返回 NULL，其余请求调用真实 malloc，避免人为耗尽系统内存。

## 实际验证

普通构建：62 项检查、0 失败。测试构建：68 项检查、0 失败。上述警告选项编译无警告。

覆盖零容量、空/满队列、无效输出、失败输出保留、查看不出队、同容量、回绕满队列扩容及全序列 FIFO、回绕缩至恰好满、缩容后继续入队、拒绝过小容量与非空缩零、大小乘法溢出、空队列缩容、释放再使用、容量一回绕及重复销毁；模拟初次分配、扩容和缩容失败后验证状态及后续完整数据顺序。

本题实现及测试完成，未使用内存检查器，不代表整章学习完成。
