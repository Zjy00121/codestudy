# Q_1：动态数组栈调整容量

题目：在动态分配数组的堆栈模块中增加 resize_stack 函数，仅接受堆栈新长度。

## 文件与接口

- stack.h：接口声明。
- stack.c：单个栈的实现，文件内静态状态隐藏数组、数量与容量。
- main.c：操作演示和自动检查；正常构建不替换分配器。

`bool resize_stack(size_t new_length)` 的新长度指容量，单位为整数元素个数，不是字节数，也不改变有效数量。成功返回 true，失败返回 false。缩容低于有效数量会拒绝，避免丢数据；空栈缩到零释放空间；相同容量直接成功。size_t 接口要求传入非负容量，来自字符串或有符号整数的值应在转换前验证。

先检查容量乘法溢出，使用临时指针接收 realloc；非零分配失败保留原状态，成功才更新地址和容量。push 满时失败，不自动扩容。pop/peek 失败不修改输出；调用方不接触或释放底层数组。destroy_stack 可重复调用。

## 编译运行

在本题目录使用 PowerShell，必须同时编译两个源文件：

```powershell
& 'C:/mingw64/bin/gcc.exe' -std=c11 -g -Wall -Wextra -Wpedantic -Wshadow -Wconversion main.c stack.c -o main.exe
if ($LASTEXITCODE -eq 0) { .\main.exe }
```

确定性分配失败测试：

```powershell
& 'C:/mingw64/bin/gcc.exe' -std=c11 -g -Wall -Wextra -Wpedantic -Wshadow -Wconversion -DSTACK_TEST main.c stack.c -o stack_test.exe
if ($LASTEXITCODE -eq 0) { .\stack_test.exe }
```

测试构建只让指定的一次分配返回 NULL，其他请求使用真实 realloc；不会用巨大请求赌系统内存耗尽。

## 实际验证记录

普通构建：29 项检查、0 失败；测试构建：34 项检查、0 失败。上述警告选项编译无警告。

覆盖：初始空栈、零容量、满栈、输出空指针、失败输出保持、同容量、扩容、缩容至有效数量、拒绝过小缩容及非空缩零、LIFO 数据保留、容量乘法溢出、释放后再使用、重复销毁，以及模拟扩/缩容分配失败的状态保持。

本题实现及测试已完成，不等于整章学习完成。未使用内存检查器，也不声称已做并发测试；模块供单线程使用。
