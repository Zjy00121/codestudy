# 第四章：语句 — 知识点总结

## 4.1 空语句

只有一个分号 `;`，什么都不做。

### 用途

| 场景 | 示例 |
|------|------|
| 循环工作全在条件里 | `while ((ch = getchar()) != EOF);` |
| 占位符 | `void f() { ; /* TODO */ }` |
| 跳过循环体 | `for (i = 0; arr[i] != x; i++);` |
| 空分支 | `if (cond) ; else do_something();` |

### 常见坑

多写一个分号导致循环体或 if 体变空：

```c
while (i < 10);    // ← 多余分号，死循环！
    sum += i;      // 永远不执行
```

---

## 4.4 if 语句

- `else` 总是与最近的未匹配 `if` 结合
- C 没有 `elseif` 关键字，`else if` 是 `else { if (...) }` 的简写
- 用 `{}` 避免歧义

---

## 4.5 while 语句 / 4.6 for 语句 / 4.7 do 语句

### for 中声明的变量

```c
for (int i = 0; i < 10; i++) { }   // i 只活在 for 内，循环结束就死
```

C99 起支持。循环变量不会泄漏到后续代码。

### do-while(0) 在宏中的妙用

```c
#define SAFE_MACRO(x)  do { foo(x); bar(x); } while(0)
// 保证宏作为单条语句出现，不受周围 if/else 影响
```

---

## 4.8 switch 语句

### fall-through 机制

```c
switch (n) {
    case 0: printf("A");   // 没有 break → 继续往下执行！
    case 1: printf("B");   // 也会被执行
}
```

`break` 是跳出 switch 的门。不写就穿透。

### 何时故意用 fall-through

```c
case 'a': case 'e': case 'i': case 'o': case 'u':
    printf("元音\n"); break;
```

多个 case 共享同一段代码。

---

## 4.9 goto 语句

正当用途：跳出深层嵌套、错误处理跳转到统一清理点。

---

## 相关函数用法

### fgets — 读一行

```c
char *fgets(char *buf, int n, FILE *stream);
```

| 参数 | 含义 |
|------|------|
| `buf` | 存放读到的字符 |
| `n` | 最多读 n-1 个字符（留 1 个给 `\0`） |
| `stream` | `stdin` = 键盘输入 |

- **换行符 `\n` 会被读进去**，末尾自动补 `\0`
- 返回值：读到内容 → 返回 `buf`；EOF/出错 → 返回 `NULL`
- 返回值用 `NULL` 判断，不用 `EOF`（`fgets` 返回指针，不是 int）

### fabs — 浮点绝对值

```c
#include <math.h>
double fabs(double x);
```

编译时加 `-lm` 链接数学库。整数用 `abs()`（`<stdlib.h>`）。

### strcmp — 字符串比较

```c
#include <string.h>
int strcmp(const char *s1, const char *s2);
// 返回 0 → 相同；非 0 → 不同
```

### strcpy — 字符串复制

```c
#include <string.h>
char *strcpy(char *dst, const char *src);
// 把 src 复制到 dst（含 \0）
```

### 字符串长度（手动实现）

```c
int len = 0;
while (str[len] != '\0')
    len++;
```

### 空格检测

```c
if (ch == ' ') { }              // 直接比较
if (isspace(ch)) { }            // 含空格、\t、\n 等（需 <ctype.h>）
```

---

## scanf 注意事项

- **格式串里不要写 `\n`**：`scanf("%d\n", &n)` 会跳过所有空白直到碰到非空白才停
- 连续输入用 `while (scanf("%d", &n) == 1)` 循环

### scanf 返回值陷阱

`while (scanf("%d", &n) == 1)` 和 `while (scanf("%d", &n))` **不一样**：

| 写法 | 读到整数 | 读到字母 | Ctrl+Z (EOF) |
|------|----------|----------|-------------|
| `== 1` | 继续 | 退出 | 退出 ✅ |
| 不写`==1` | 继续 | 退出 | **死循环** ❌ |

`scanf` 返回三种值：成功次数（1）、匹配失败（0）、EOF（-1）。C 里非 0 即真，`-1` 也为真，所以不写 `== 1` 碰到 EOF 退不出去。

> 💡 `scanf` 返回值必须和成功次数比，不能直接当 bool 用。`getchar` 才可以直接写 `while ((ch = getchar()) != EOF)`。

---

## printf 格式符

| 格式 | 类型 | 用途 |
|------|------|------|
| `%d` | `int` | 整数 |
| `%f` | `double` | 浮点 |
| `%c` | `char` | 字符 |
| `%zu` | `size_t` | `sizeof` 返回值 |
| `%s` | `char *` | 字符串 |

---

## 数组与 NUL

- `char a[N]` 声明有 N 个元素，下标 0 到 N-1，`a[N]` 越界
- 普通数组声明**不自带 `\0`**，只有字符串初始化时编译器自动加
- 函数内声明的数组是垃圾值，函数外（全局/static）自动清零

---

## 字符串去多余空格（deblank 逻辑）

核心思路：用一个标志位跟踪是否"正在一个空格区间内"：

```c
at_beg = 1                    // 初始：未进入空格区
遇到第一个空格 → at_beg = 0   // 保留，标记进入
遇到后续空格 → 删除（左移所有后续字符），i-- 回头检查
遇到非空格   → at_beg = 1    // 离开空格区
```

删除后 `i--` 是关键——左移后当前位置补上来的是新字符，必须回头检查。
