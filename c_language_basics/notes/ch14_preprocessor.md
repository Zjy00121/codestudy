# 第十四章：预处理器 — 知识点总结

> 根据本次可用对话中预定义符号、宏、条件编译、文件包含和其他指令的讲解整理，重复的文件包含讲解合并记录。
> 学习进度统一见 [学习计划](../STUDY_PLAN.md)。正文代码片段分别说明概念，不应直接拼接，未单独编译运行，展示结果为预期结果；Q_1 独立练习的实际测试记录见文末。

## 14.1 预定义符号

预定义宏由实现预先提供，无需自行 #define。宏在预处理时展开，不是运行时查询函数。

| 宏 | 展开结果与用途 |
|------|------|
| `__FILE__` | 表示当前逻辑源文件名的字符串字面量，具体路径形式取决于实现和构建方式 |
| `__LINE__` | 当前逻辑源代码行号的整数常量，不是执行次数 |
| `__DATE__` | 翻译日期字符串，形式为 Mmm dd yyyy，个位日期前补空格 |
| `__TIME__` | 翻译时间字符串，形式为 hh:mm:ss，不是程序运行时的时间 |
| `__STDC__` | 符合标准的实现中为 1，用于标识 ISO C 一致性 |
| `__STDC_VERSION__` | C 标准版本的 long 整数常量；早期标准不要求提供 |
| `__STDC_HOSTED__` | 1 表示宿主环境，0 表示独立环境 |

版本值：C95 为 199409L，C99 为 199901L，C11 为 201112L，C17 为 201710L，C23 为 202311L；它们不是编译器版本或当天日期。

```c
/* 需要 <stdio.h>；先检查版本宏是否存在，再使用。 */
#ifdef __STDC_VERSION__
    printf("C standard version: %ld\n", __STDC_VERSION__);
#else
    printf("C standard version macro is unavailable.\n");
#endif
```

嵌入式不一定是独立环境，独立环境也不是完全没有标准库；取决于工具链、配置与运行环境。不要重定义标准预定义宏，也不要随意给项目标识符使用双下划线等实现保留名称。

### 文件、行号与函数名称

```c
/*
 * message：错误消息字符串，需要 <stdio.h>。
 * 文件名和行号对应宏使用的位置，不是 #define 所在位置。
 */
#define REPORT_ERROR(message) \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, (message))
```

__FILE__ 和 __LINE__ 可以受 #line 影响，不一定等于编辑器中的物理位置。构建时间信息进入程序后不会因次日运行自动更新。

C99 起的 `__func__` 提供当前函数名，但不是预处理宏，而是行为相当于函数内静态 const char 数组的预定义标识符。不能用 #ifdef __func__ 检测支持情况。

## 14.2 #define

### 对象式宏与函数式宏

```c
#define SENSOR_COUNT 3        // 对象式宏，不创建名为 SENSOR_COUNT 的变量
#define SQUARE(x) ((x) * (x))  // 函数式宏，参数参与记号替换，不是函数传值
```

- 宏名本身没有 C 类型，展开后的表达式按 C 规则具有类型，例如 3 为 int，3U 为 unsigned int。
- #define 通常由换行结束，不加分号；分号若写入会成为替换内容。
- 不展开字符串字面量和注释里的宏名，不替换长标识符中的一部分。
- 定义函数式宏时，宏名与左括号不能有空格；正常定义后，调用位置可以在名字与括号间留空格。
- 宏不遵守花括号的块作用域；从定义位置起影响当前预处理翻译单元的后续内容，直到取消定义或处理结束，不自动传播到另一个独立编译的 .c。

### 括号只能解决优先级，不能解决重复求值

| 定义或调用 | 问题 |
|------|------|
| `#define SQUARE(x) x * x` | SQUARE(2 + 3) 展开为 2 + 3 * 2 + 3，得到 11 |
| `#define SQUARE(x) (x) * (x)` | 100 / SQUARE(5) 展开后变成先除再乘，得到 100 |
| `#define SQUARE(x) ((x) * (x))` | 参数与整体都有括号，修正上述优先级问题 |
| `SQUARE(i++)` | 仍展开为 ((i++) * (i++))，对同一对象未排序地多次修改，行为未定义 |

```c
/* a、b 可能被求值多次，不传带副作用的表达式。 */
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

/* 先读取一次传感器，避免 MAX(read_sensor(), 100) 导致重复读取。 */
int sample = read_sensor(); // 假设接口已声明且返回 int
int result = MAX(sample, 100);
```

普通函数具有类型接口，实参各求值一次，但多个实参之间的求值顺序不能随意假定。不要笼统认为宏一定比函数快。

### 多语句宏

反斜杠紧接换行可续行，行末反斜杠后不要添加注释或其他字符。

```c
/*
 * v：适合 %d 的整数表达式，本宏求值一次，需要 <stdio.h>。
 * do-while(0) 使多条语句作为一个语句使用，调用处正常写分号。
 */
#define REPORT_VALUE(v)          \
    do                          \
    {                           \
        printf("Value: ");      \
        printf("%d\n", (v));    \
    } while (0)
```

这样可用于 if/else，不会把第二条输出语句意外放到 if 控制之外；循环体只执行一次。

### # 字符串化与 ## 记号拼接

```c
/* 直接字符串化不先展开参数；外层宏用于先展开参数。 */
#define STRINGIFY_RAW(x) #x
#define STRINGIFY(x) STRINGIFY_RAW(x)
#define SENSOR_COUNT 3
/* STRINGIFY_RAW(SENSOR_COUNT) 得到 "SENSOR_COUNT"，STRINGIFY 得到 "3"。 */

/* 拼接结果必须是合法的预处理记号；两层写法可先展开宏参数。 */
#define JOIN_RAW(a, b) a##b
#define JOIN(a, b) JOIN_RAW(a, b)
#define INDEX 2
/* JOIN(sensor_, INDEX) 生成标识符 sensor_2，不是在运行时拼接字符串。 */
```

直接参与 # 或 ## 的参数不按普通参数的方式先行展开，因此需要两层宏的场景要明确。

### 与其他语言工具的区别

| 工具 | 适合用途 |
|------|------|
| #define | 配置、预处理选择、记号操作和必要的宏替换 |
| const | 带类型、作用域和存储期的对象，限制相应修改 |
| typedef | 类型别名，不是文本替换 |
| 函数 | 带类型接口的操作，便于检查、调试，可能被优化或内联 |

## 14.3 条件编译

条件编译在预处理阶段选择保留的代码，普通 if 是 C 语句，可依赖运行时数据。普通 if 的各分支通常仍需通过编译检查；预处理排除的部分不作为 C 代码编译。

```c
#ifndef ENABLE_DEBUG
#define ENABLE_DEBUG 0 // 外部未配置时采用默认值
#endif

#if ENABLE_DEBUG
    /* 保留调试实现。 */
#else
    /* 保留普通实现。 */
#endif
```

### 是否定义不等于值是否非零

| 配置 | #ifdef FEATURE | #if FEATURE |
|------|------|------|
| 未定义 | 假 | 剩余未定义标识符按 0 处理，为假 |
| 定义为 0 | 真 | 假 |
| 定义为 1 | 真 | 真 |
| 定义为空宏 | 真 | 展开后缺少有效条件表达式，不能这样使用 |

- #ifdef FEATURE 等价于 #if defined(FEATURE)。
- #ifndef FEATURE 等价于 #if !defined(FEATURE)。
- defined 用在预处理条件中，不是运行时函数。
- 可用 &&、||、! 组合条件；#elif 提供其他条件，只有第一个满足的分支被选择。
- 每个条件开始指令必须有对应的 #endif，嵌套时分别配对。

```c
/* 要求版本宏存在，并且对应 C11 或更高版本。 */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    /* 使用相应标准功能。 */
#else
    /* 使用兼容实现。 */
#endif
```

### 条件表达式与配置来源

#if 不读取 C 变量的值，即使变量是 const；也不能用 sizeof 查询 C 类型大小。宏展开后剩余的普通未定义标识符按 0 处理，拼错宏名可能因此悄悄关闭功能。GCC 的 -Wundef 有助于发现此类错误。

```powershell
& 'C:/mingw64/bin/gcc.exe' -Wall -Wextra -Wpedantic -DENABLE_DEBUG=1 main.c -o main.exe
```

- -D 是构建时定义宏；main.exe --debug 是运行时参数，需要程序解析，两者不同。
- 被排除区域仍要让预处理器正确识别注释、记号和条件指令，不可随意放未结束的注释或不匹配的 #endif。
- 减少过深嵌套；平台差异较多时可拆分实现文件。

## 14.4 文件包含

### 含义与搜索

#include 引入指定文件内容，并继续处理其中的宏、条件编译和嵌套包含；不是调用该文件或执行另一个程序。.h 是命名约定，不是强制扩展名。

- `<stdio.h>` 通常用于标准或工具链头文件；`"sensor.h"` 通常用于项目头文件。
- GCC 常见配置中双引号形式先搜索当前包含文件所在目录，再搜索其他路径；实际顺序依实现和编译选项而定。
- -Iinclude 增加头文件搜索目录，不是库文件搜索目录。

### 声明、实现与链接

```text
compare.h：声明 max_int 接口
    ↑            ↑
 main.c       compare.c
 调用接口      定义函数
```

```c
/* compare.h：共享接口及包含保护。 */
#ifndef CODESTUDY_COMPARE_H
#define CODESTUDY_COMPARE_H

/* left、right 为待比较整数，返回较大值。 */
int max_int(int left, int right);

#endif
```

main.c 和 compare.c 都包含 compare.h；实现方包含自己的头文件有助于检查声明、定义一致性。

```powershell
& 'C:/mingw64/bin/gcc.exe' -g -Wall -Wextra -Wpedantic -Wshadow -Wconversion main.c compare.c -o main.exe
```

只包含声明而没有让实现参与链接，可能出现未定义引用。不要通过包含 .c 来替代常规编译链接，否则与单独编译同一 .c 混用时可能重复定义。

| 错误阶段 | 检查重点 |
|------|------|
| 找不到头文件 | 名称、路径、包含目录 |
| 参数或返回类型不匹配 | 声明、定义和调用 |
| 链接时缺少实现 | 实现文件或库是否参与链接 |

### 包含保护与翻译单元

直接包含与间接包含可能让同一头文件反复出现，导致结构体等重复定义。#ifndef/#define/#endif 在首次处理正文时定义保护宏，之后跳过正文；保护宏应独特，避免不同头文件互相屏蔽。

一个 .c 连同其包含内容经过预处理形成一个翻译单元。包含保护只作用于各自的预处理过程，不能解决多个翻译单元各自产生外部定义的问题。

```c
/* counter.h：声明共享对象，不在各个包含者中创建定义。 */
extern int count;

/* counter.c：在唯一一个实现文件中定义。此行不放进公共头文件。 */
int count = 0;
```

文件作用域的普通 int count; 也不是这里需要的纯 extern 声明，它构成暂定定义。头文件中 static 变量通常让每个翻译单元各有一份对象，不是共享一份。

头文件通常放类型定义、typedef、宏、函数声明、必要的 extern 声明；普通函数定义和共享变量正式定义放在 .c 中。static inline 等有特定规则的函数定义是可放头文件的情形，不能泛化。

### 自包含与依赖

头文件若使用 size_t，应自己包含 <stddef.h>，不要要求调用方猜测前置包含顺序。尽量减少不必要的依赖与循环包含。

#pragma once 是常见实现扩展，不是标准 C 的头文件保护方式；可移植写法优先使用保护宏。

## 14.5 其他指令

### #error：配置诊断

```c
#define BUFFER_SIZE 8

/* 在构建时诊断不合法的容量配置，不是运行时的输入检查。 */
#if BUFFER_SIZE < 16
#error BUFFER_SIZE must be at least 16
#endif
```

常用工具链遇到有效分支中的 #error 会使构建失败并报告信息。后面无需引号或分号；不要假设诊断文字中的宏会按 printf 参数一样展开。可用于传感器配置缺失、冲突或不支持的平台选择。

### #line：逻辑源位置

```c
/* 以下放在函数内部，并已包含 <stdio.h>。 */
#line 100 "generated_sensor.c"
printf("%s:%d\n", __FILE__, __LINE__);
```

预期显示 generated_sensor.c:100。指定的是下一行的逻辑行号，之后继续递增；可只指定行号。不会创建文件、修改磁盘文件名或跳转执行。主要用于代码生成工具，使诊断对应原始输入的位置。

### #pragma 与 _Pragma

#pragma 的含义取决于具体指令：有标准规定的形式，也有编译器扩展，使用前确认目标实现的支持。

- once：常见的防重复包含扩展。
- pack：某些实现提供布局控制，可能影响成员对齐、填充及访问性能；使用 push/pop 恢复状态，避免污染后续定义。
- 打包结构体不会固定整数类型宽度，也不解决字节序，不能仅靠它构造可移植的通信协议。
- 警告控制等实现专用 pragma 不应代替修正类型或指针错误。

C99 的 _Pragma 运算符可用于宏展开中表达 pragma 效果：

```c
/* x 是 pragma 内容，先字符串化，再交给 _Pragma 处理。 */
#define DO_PRAGMA(x) _Pragma(#x)
/* 具体 pragma 是否支持仍取决于实现，不因使用包装就变得可移植。 */
```

普通宏替换不能简单生成新的 #pragma 预处理指令，_Pragma 提供了相应机制。

### 空指令与 #undef

只有 # 的一行是合法空指令，不执行操作；它属于预处理阶段，与 C 的空语句分号不同。

```c
#define ENABLE_DEBUG 1
#undef ENABLE_DEBUG // 从此不再定义，取消未定义的宏也允许
```

把宏定义为 0 不等于取消定义。要改成不同替换内容，可以先 #undef 再 #define。

## 练习记录

### Q_1：按编译选项选择报表打印函数

- 文件：[Q_1/main.c](../14_preprocessor/Q_1/main.c)。
- 题目要求：void print_ledger(int x) 根据选项是否定义，调用一个或多个打印函数，并原样传入 x。

| 已定义选项 | 调用结果 |
|------|------|
| 无 | print_ledger_default(x) |
| OPTION_LONG | print_ledger_long(x) |
| OPTION_DETAILED | print_ledger_detailed(x) |
| 两项都有 | 先 long，再 detailed，各一次，不调用 default |

原实现使用两个独立 #ifdef 支持组合选项，这个思路正确。修正了 OPTION_DETALIED 的拼写，以及 print_longer_* 与题目要求的 print_ledger_* 名称不一致的问题。

去掉辅助宏 OK，直接用 !defined(OPTION_LONG) && !defined(OPTION_DETAILED) 选择默认分支。宏不受函数花括号作用域限制，外部同名 OK 可能错误地屏蔽默认调用；直接检查选项更清楚，也避免污染宏环境。

```c
/*
 * x：原样交给打印实现的整数；无返回值。
 * 三个打印函数需提前声明。选项按是否定义判断，不按数值判断。
 */
void print_ledger(int x)
{
#ifdef OPTION_LONG
    print_ledger_long(x);
#endif
#ifdef OPTION_DETAILED
    print_ledger_detailed(x);
#endif
#if !defined(OPTION_LONG) && !defined(OPTION_DETAILED)
    print_ledger_default(x);
#endif
}
```

不能改成互斥的 #if/#elif，否则两项都定义时只会调用一个函数。定义为 0 仍算已定义，-DOPTION_LONG=0 仍启用 long；取消定义才是不选择该选项。

### 测试与验证范围

练习已用 MinGW64 GCC 和 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 实际编译运行，8 种配置均通过且无编译警告：无选项、仅 long、仅 detailed、两项同时、long 为 0、detailed 为 0、两项均为 0、仅定义无关的 OK=1。

每种配置传入 42、0、-7、INT_MIN、INT_MAX，外部脚本逐行核对调用次数、顺序与参数是否原样传递，共 40 组输入/配置组合。main 中保留这五个输入。

三个打印函数是测试替身，只输出函数名称标识和参数；题目没有提供实际报表格式，因此不代表验证了金融交易或报表业务。
