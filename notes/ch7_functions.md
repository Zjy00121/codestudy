# 第七章：函数 — 知识点总结

## 7.1 函数定义

### 函数四要素

```c
int add(int a, int b)    // ①返回类型  ②函数名  ③形参列表
{                        // ④函数体
    return a + b;
}
```

| 要素 | 说明 |
|------|------|
| 返回类型 | 计算结果类型，`void` 表示不返回 |
| 函数名 | 调用时使用的名字 |
| 形参列表 | 需要调用方传入的参数，`(void)` 表示无参数 |
| 函数体 | `{}` 内的实现代码 |

## 7.2 函数声明

### 声明 vs 定义

```c
// 声明（原型）— 告诉编译器"有没有"、"什么格式"
int add(int a, int b);

// 定义 — 真正的实现，产生机器码
int add(int a, int b) { return a + b; }
```

| | 声明 | 定义 |
|------|------|------|
| 有没有 `{}` | ❌ 分号结尾 | ✅ 有函数体 |
| 产生代码 | ❌ | ✅ |
| 可出现次数 | 任意次 | 仅一次 |
| 放哪里 | 头文件 `.h` | 源文件 `.c` |

### 如果没有声明

编译器**猜**函数返回 `int`、参数随意（C89/90 行为，C99+ 已禁止）。猜错 → 未定义行为。
声明的作用就是让编译器**在调用处帮你检查类型匹配**。

### 返回类型不匹配

- 声明和定义返回类型不同 → 编译报错
- 声明一致但 `return` 表达式类型更宽 → 自动窄化转换，可能截断

## 7.3 函数的参数

### 核心结论：C 一切传参都是值传递

```c
void change(int x) { x = 100; }   // 改的是副本
int a = 10;
change(a);                          // a 仍是 10
```

**想改外部变量 → 传地址**：
```c
void change(int *p) { *p = 100; }
change(&a);   // a 变成 100
```

### 数组作为参数 — 退化为指针

```c
void f(int arr[], int n);    // 等价于
void f(int *arr, int n);     // 数组退化成指针

// 函数内部 sizeof(arr) = 指针大小（不是数组大小）
// 所以必须另传长度参数 n
```

### `const` 修饰形参

```c
int find_char(char const *source, char const *chars);
// 承诺"我不会改你的数据"，调用方可放心传入
// const 在 * 左边 → 约束所指向的内容
// const 在 * 右边 → 约束指针本身
```

## 7.4 ADT 和黑盒

### 黑盒 — 接口与实现分离

调用方只关心"能做什么"（接口），不关心"怎么做"（实现）。

```c
// stack.h — 接口
typedef struct Stack Stack;          // 不完全类型：不暴露 struct 内容
Stack *stack_create(void);
void   stack_push(Stack *s, int val);

// stack.c — 实现
struct Stack { int data[100]; int top; };  // 定义藏在这里
```

### C 语言封装手段

- **`static` 函数**: 只在当前 `.c` 文件可见，成为"私有函数"
- **不完全类型**: 头文件只声明 `typedef struct X X;`，不暴露成员

### ADT 好处

| 好处 | 说明 |
|------|------|
| 隔离变化 | 内部改用链表、改数据结构，调用方代码不动 |
| 降低复杂度 | 调用方只记接口函数，不用管内部 |
| 防误操作 | 调用方不能绕过接口直接访问内部成员 |
| 嵌入式应用 | HAL（硬件抽象层）：上层用 `gpio_write()`，底层换芯片只改实现 |

## 7.5 递归

### 递归 = 基线条件 + 递归条件

```c
int factorial(int n) {
    if (n <= 1) return 1;               // 基线：什么时候停
    return n * factorial(n - 1);        // 递归：怎么缩小问题
}
```

### 运行时栈模型

每层递归在**栈**上创建独立的栈帧（局部变量 + 返回地址）。
递归太深 → 栈帧太多 → 栈溢出（Stack Overflow）。

### 哪些问题适合递归

- 问题本身是递归结构的：树遍历、目录遍历、汉诺塔
- 可分解为同结构子问题的：阶乘、斐波那契、快排/归并

### 递归 vs 迭代

| | 迭代 | 递归 |
|------|------|------|
| 空间 | O(1) | O(n) 每层一个栈帧 |
| 速度 | 通常更快 | 有函数调用开销 |
| 可读性 | 线性问题更直观 | 树形/分治问题更直观 |

**简单规则: 线性问题用迭代，递归结构用递归。**

## 7.6 可变参数列表

### `<stdarg.h>` 四个宏

```c
#include <stdarg.h>

void demo(int count, ...) {
    va_list args;              // ① 声明参数游标
    va_start(args, count);     // ② 定位到第一个可变参数
    int val = va_arg(args, int);  // ③ 取出当前参数，游标前进
    va_end(args);              // ④ 清理
}
```

| 宏 | 作用 |
|----|------|
| `va_list` | 声明游标变量 |
| `va_start(ap, last)` | 让游标指向最后一个固定参数之后 |
| `va_arg(ap, type)` | 取出当前值，游标前进 `sizeof(type)` |
| `va_end(ap)` | 收尾清理 |

### 如何知道参数个数

| 策略 | 例子 |
|------|------|
| 格式字符串 | `printf(fmt, ...)` — 数 `%` 占位符 |
| 计数参数 | `sum(count, ...)` — 第一个参数指明数量 |
| 哨兵值 | `execl(..., NULL)` — 遇到特殊值结束 |

### 默认参数提升（重要！）

```c
va_arg(args, float);   // ❌ float → 实际已提升为 double
va_arg(args, double);  // ✅

va_arg(args, char);    // ❌ char → 实际已提升为 int
va_arg(args, int);     // ✅
```

## 练习总结

### Q_1 — 厄密多项式 (递归)

```c
H_0(x) = 1
H_1(x) = 2x
H_n(x) = 2x·H_{n-1}(x) - 2(n-1)·H_{n-2}(x)
```

**注意**: 用 `if / else if / else` 链而非三个平列 `if`，避免编译器误报"无返回值"。

### Q_2 — 最大公约数 (欧几里德辗转相除，递归)

```c
int gcd(int m, int n) {
    if (n == 0) return m;                   // 除零保护
    if (m % n == 0) return n;               // 基线
    return gcd(n, m % n);                   // 递归
}
```

**注意**: 需要 `n == 0` 保护；负数入参先 `abs()`。

### Q_3 — 字符串转整数 (指针遍历)

```c
while (*string != '\0') {
    if (*string < '0' || *string > '9') break;
    value = value * 10 + (*string - '0');
    string++;    // ← 容易遗忘！缺了就是死循环
}
```

**经典 bug**: 循环内忘记 `string++` → 死循环。

### Q_4 — 可变参数找最大值 (哨兵模式)

以 `-1` 为哨兵标记参数列表结束：
```c
while (num >= 0) {
    num = va_arg(var_arg, int);
    if (num > max) max = num;
}
```

### Q_5 — 简化版 printf

支持的格式码: `%d` / `%f` / `%s` / `%c` / `%%`

架构：
```
my_printf → 扫描格式字符串 → 按格式码分发
  %d → print_integer()  — 数字逐位提取 + 逆序输出
  %f → print_float()    — 整数/小数分离，复用 print_integer
  %s → 逐字符 putchar
  %c → putchar(va_arg(int))  ← char 提升为 int!
```

**设计要点**:
- `print_integer`: `buf[i++] = '0' + (n % 10)`，输出时逆序
- `print_float`: `int_part = (int)f; frac_part = f - int_part`，小数逐位 ×10 取整
- `%c` / `%f` 的变参读取必须注意类型提升

### Q_6 — 数字转英文单词 (查表法 + 分解)

```
16312
  → 千进制分组: [16312/1000=16] THOUSAND + [312]
  → write_hundreds(16) + "THOUSAND" + write_hundreds(312)
  → "SIXTEEN THOUSAND THREE HUNDRED TWELVE"
```

**结构**:
- 四张查表数组: `ones[]`, `teens[]`, `tens[]`, `groups[]`
- `written_amount()` — 千进制分组 + 组名拼接
- `write_hundreds()` — 处理 0-999
- `write_word()` — 追加单词 + 自动控前导空格

## 编程提醒

- **声明先行于调用** — 让编译器帮你检查类型，别让它猜
- **一切传参都是值传递** — 想改外部变量 → 传地址
- **递归必须有基线条件** — 否则无限递归直到栈溢出
- **可变参数注意类型提升** — `float→double, char→int`
- **循环遍历字符串 / 数组** — 别忘了 `p++`！
- **`if/else if/else` 链** — 比平列 `if` 更安全，编译器能推理出所有路径都返回
