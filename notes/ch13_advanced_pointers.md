# 第十三章：高级指针话题 — 知识点总结

> 根据本次可用对话中 13.1～13.5 的讲解整理，学习进度统一见 [学习计划](../STUDY_PLAN.md)。
> 代码片段分别说明概念，不应直接拼接成一个程序。正文示例未单独编译运行，示例结果为预期结果；Q_1～Q_3 独立练习的实际验证记录见文末。

## 13.1 进一步探讨指向指针的指针

### 指针变量也是对象

```c
int value = 10;   // 整数对象
int *p = &value;  // 指针对象，保存 value 的地址
int **pp = &p;    // 二级指针对象，保存 p 的地址
```

```text
pp ──▶ p ──▶ value
int ** int *  int
```

| 表达式 | 类型 | 访问的含义 |
|------|------|------|
| pp | int ** | 保存 p 地址的指针变量 |
| *pp | int * | 指针变量 p |
| **pp | int | p 最终指向的整数 |
| &p | int ** | p 自身的地址 |

```c
/* 延续上面的对象关系。 */
**pp = 20;       // 修改 value，不改变 p 的指向
int other = 99;
*pp = &other;    // 修改 p，让它指向 other；value 仍为 20
**pp = 100;      // 此时修改的是 other
```

### 通过函数修改调用方的指针

C 的参数传递都是值传递。接收 int *p 后执行 p = NULL，只改变局部参数副本；要修改调用方的指针变量，需要传入它的地址。

```c
/*
 * pp：调用方指针变量的地址，允许传 NULL。
 * 无返回值；只清空指针，不释放原来指向的对象。
 * 若它是动态内存的唯一入口，清空前必须先处理释放责任，避免泄漏。
 */
void clear_pointer(int **pp)
{
    if (pp == NULL)
    {
        return;
    }
    *pp = NULL; // 修改的是调用方的指针变量
}
```

调用形式为 clear_pointer(&p)。与第十二章对应：

- 单链表删除函数接收 struct NODE **rootp，通过 *rootp 更新独立头指针。
- 根节点结构体已经保存了入口时，只需传入其地址，通过 rootp->next 修改成员。
- 还可以返回新头指针，由调用方使用 head = sll_reverse(head) 接收。

不是“修改链表必须使用二级指针”，而是根据要修改的对象决定传入哪个地址。

### 指针数组与二维数组不能混淆

```c
int a = 10;
int b = 20;
int *items[] = {&a, &b}; // 数组元素的类型是 int *
int **pp = items;       // 指向首个指针元素

pp++;                   // 移到下一个指针元素，不是让 a 的地址向后移动
**pp = 30;              // 此时修改 b

int matrix[2][3] = {{1, 2, 3}, {4, 5, 6}};
int (*row)[3] = matrix; // 指向一行，每行包含三个 int
```

- pp + 1 按一个 int * 元素移动，对应大小为 sizeof(int *)，不能固定认定为 4 或 8 字节。
- matrix 转换为首元素指针后，类型是 int (*)[3]，不是 int **。
- 二维数组直接存整数；指针数组存地址。强制转换不能修复这两种存储结构的差异。
- 指针本身不携带数组长度，函数通常还需要额外的数量参数。

### 逐层有效性与 const

pp 有效只表示能够访问中间指针；要访问 **pp，中间指针也必须指向有效整数。

```c
/* 前提：非空地址均指向仍然有效且允许相应访问的对象。 */
if (pp != NULL && *pp != NULL)
{
    **pp = 100; // && 短路，pp 为空时不会读取 *pp
}
```

如果只是给 *pp 写入新地址，不要求 *pp 原来非空。非空检查无法识别所有野指针或悬空指针。

| 类型 | 限制 |
|------|------|
| int ** | 可通过它修改中间指针及最终整数 |
| int **const | 最外层指针变量不能重新赋值 |
| int *const * | 不能通过它修改中间指针，可修改最终整数 |
| const int ** | 不能通过它修改最终整数，可修改中间指针 |

int ** 不能安全地隐式转换为 const int **。否则可以把只读整数的地址写进原本的 int *，再通过这个可写指针尝试修改 const 对象。不要用强制转换绕过类型检查。

## 13.2 高级声明

### 阅读方法

从标识符出发向外分析：尊重分组括号，[] 和函数参数列表 () 比前面的 * 结合得更紧，最后结合基础类型。区分分组括号 (*p) 与函数参数列表 (int, int)。

| 声明 | 含义 |
|------|------|
| int *a[3]; | 包含三个 int * 元素的数组 |
| int (*p)[3]; | 指向 int[3] 数组的指针 |
| int *f(void); | 无参数、返回 int * 的函数 |
| int (*fp)(void); | 指向无参数、返回 int 的函数的指针 |
| int (*ops[3])(int, int); | 包含三个函数指针的数组，函数接收两个 int 并返回 int |
| int (*get_row(void))[3]; | 无参数、返回数组指针的函数，目标数组为 int[3] |
| int (*select_operation(int choice))(int, int); | 接收 int、返回函数指针的函数，被指向函数接收两个 int 并返回 int |

可用合法的使用表达式核对类型：(*p)[0] 得到 int，ops[0](10, 20) 也得到 int。实际访问仍需保证地址、下标及函数指针有效。

函数不能直接返回数组或函数，但可以返回数组指针或函数指针。返回地址不会延长普通局部对象的生命周期。

### typedef 拆开声明

```c
/* 类型别名，表示接收两个 int、返回 int 的函数指针。 */
typedef int (*BinaryOperation)(int, int);
BinaryOperation operations[3]; // 三个函数指针组成的数组，使用前需要初始化
BinaryOperation select_operation(int choice); // 返回函数指针的函数

typedef int Row[3]; // Row 是数组类型，不是指针类型
Row *get_row(void); // 返回指向一行数组的指针
```

typedef 是类型别名，不是文本替换：

```c
typedef int *IntPointer;
const IntPointer p = NULL; // 等价于 int *const p，不是 const int *p
```

int *p, value 声明的是一个指针和一个整数，* 不会作用于后面的所有变量。初学阶段建议分别声明。

## 13.3 函数指针

### 声明、赋值与调用

```c
/* 返回 left、right 中的较大值，不修改外部对象。 */
int max_value(int left, int right)
{
    if (left > right)
    {
        return left;
    }
    return right;
}

/* 以下语句放在调用函数中。 */
int (*operation)(int, int) = max_value; // 保存函数地址，不复制函数
int result = operation(10, 20);       // 间接调用，预期为 20
```

- operation = max_value 与 operation = &max_value 含义相同。
- operation(a, b) 与 (*operation)(a, b) 都能调用所指函数。
- max_value(10, 20) 是调用得到的整数，不是函数地址，不能赋给 operation。
- 可以改变 operation 的指向以选择不同函数，但目标函数类型必须兼容。
- 不能调用空指针、未初始化指针或通过不兼容的函数指针类型调用函数；不能用强制转换掩盖接口不兼容。

### 回调函数

回调是把普通函数的地址交给另一个函数，由后者调用；不要求异步执行。

```c
/*
 * left、right：待处理整数；operation：调用方指定的处理函数。
 * 无返回值，输出操作结果；空函数指针时报告错误并返回。
 * 需要 <stdio.h> 提供输出函数及 NULL。
 */
void print_result(int left, int right, int (*operation)(int, int))
{
    if (operation == NULL)
    {
        fprintf(stderr, "No operation selected.\n");
        return;
    }
    int result = operation(left, right);
    printf("Result: %d\n", result);
}
```

调用 print_result(10, 20, max_value)，由 print_result 决定调用时机，由调用方选择处理行为。嵌入式中可用于传感器数据接收后的用户处理函数。

### 转移表

函数指针数组可以建立“编号 → 处理函数”的映射，例如命令编号对应停止、启动、查询操作。

```c
typedef int (*BinaryOperation)(int, int);
/* 假设 min_value 已定义，接口与 max_value 相同，返回较小值。 */
BinaryOperation operations[] = {max_value, min_value};

/* 以下语句放在函数内部。 */
size_t choice = 1; // 本例选择第二个操作，size_t 可由 <stddef.h> 提供
size_t count = sizeof operations / sizeof operations[0];
if (choice < count)
{
    BinaryOperation selected = operations[choice];
    if (selected != NULL)
    {
        int result = selected(10, 20);
        printf("Result: %d\n", result);
    }
}
```

先检查下标，再取出指针并检查有效性。适用于接口统一的操作集合，不必把所有 switch 都改成转移表。

### 与对象指针的区别

标准 C 不允许函数指针加减整数，不允许对函数类型使用 sizeof，但允许 sizeof 函数指针变量。不能假设函数指针与对象指针同样大小，也不能照搬对象指针与 void * 之间的转换保证。

## 13.4 命令行参数

### main 的参数与传入方式

int main(int argc, char *argv[]) 与 int main(int argc, char **argv) 的参数类型等价；数组形式的函数参数会调整为指针。

```powershell
.\main.exe lidar 10
```

通常得到 argc 为 3，argv[0] 是程序名字符串，argv[1] 是 "lidar"，argv[2] 是 "10"，argv[3] 是 NULL。

| 项目 | 含义与边界 |
|------|------|
| argc | 非负参数数量，通常包含程序名 |
| argv[0] | argc > 0 时表示程序名；具体形式由环境决定，名字不可用时可以是空字符串 |
| argv[1] | 第一项用户参数，访问前需要 argc > 1 |
| argv[argc] | 空指针，不能作为字符串传给 %s |
| argv[i][j] | 第 i 个参数的第 j 个字符，需保证两层访问有效 |

命令行参数在启动时传入；scanf 在程序运行中读取输入。参数由环境提供，不需要调用方 free。

```powershell
.\main.exe "front lidar"
.\main.exe "H:\sensor data\scan.txt"
```

引号可将包含空格的内容组合成一个参数；具体分词和转义由终端、运行环境处理，不是 C 程序自己按空格拆分。

### 数量、格式与范围分别检查

参数都是字符串，"100" 不是整数 100。需要转换时用 strtol 检查错误，不只依赖无法清楚区分错误的 atoi。

```c
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    /* 一个程序名加一个采样数量参数。 */
    if (argc != 2)
    {
        fprintf(stderr, "Usage: main.exe <sample_count>\n");
        return EXIT_FAILURE;
    }

    /* end 接收首个未转换字符的地址，指向原字符串内部，无需释放。 */
    char *end = NULL;
    errno = 0; // 排除之前操作留下的错误状态
    long count = strtol(argv[1], &end, 10); // 十进制；通过 &end 修改调用方指针

    if (end == argv[1] || *end != '\0' || errno == ERANGE)
    {
        /* 无数字、存在剩余字符或超出 long 范围，均拒绝。 */
        fprintf(stderr, "Invalid integer.\n");
        return EXIT_FAILURE;
    }
    if (count < 1 || count > 10000)
    {
        /* 类型可表示，不代表满足业务范围。 */
        fprintf(stderr, "Sample count must be between 1 and 10000.\n");
        return EXIT_FAILURE;
    }
    printf("Sample count: %ld\n", count);
    return EXIT_SUCCESS;
}
```

strtol 解析 "12abc" 时，end 指向 'a'；完全无数字时 end 等于输入起点。它接受前导空白及可选正负号，若要求纯数字还需额外格式检查。

### 选项处理

- 位置参数依靠顺序，如 lidar 100；选项参数依靠名字，如 --sensor lidar --count 100。
- 使用 strcmp(argv[i], "--count") == 0 判断内容，需要 <string.h>；== 比较的是地址。
- 读取选项值前先确认后面还有参数，再移动下标，避免越界。
- 按业务约定处理未知选项、重复选项、缺少选项值及缺少必需参数。
- 区分三个概念：argc 是数量，argv[argc] 的 NULL 结束指针序列，字符串中的 '\0' 结束字符序列。

## 13.5 字符串常量

### 字面量是数组，不是指针变量

普通字符串字面量 "hello" 对应 char[6]，包括末尾 '\0'。在很多表达式中转换为首元素指针，但 sizeof 字面量得到完整数组大小。

| 表达式 | 含义 |
|------|------|
| sizeof "hello" | 6，包含结束字符 |
| strlen("hello") | 5，不包含结束字符 |
| sizeof "" | 1，空字符串也有结束字符 |
| 'A' | C 中普通字符常量，类型为 int |
| "A" | 对应 char[2] 的字符串字面量 |

普通字面量的元素类型虽为 char，但尝试修改它是未定义行为。建议用 const char * 访问，让类型检查帮助发现修改错误。标准不规定字面量必须放在某个硬件只读内存区域。

### 指针与独立字符数组

```c
const char *p = "hello"; // 指向字面量，不复制为可写数组
char text[] = "hello";  // 用字符初始化独立数组，自动包含结束符

p = "world";           // 改变指针，不改变原字面量
text[0] = 'H';          // 修改独立数组，合法
```

- sizeof p 是指针大小，sizeof text 在这里为 6。
- 不能执行 text = "world"，数组不能通过赋值整体替换。
- const char * 限制通过指针修改字符；char *const 固定指针；const char *const 同时限制两者。
- char *const p = "hello" 也不能修改字面量；指针类型允许写入不等于目标对象允许修改。

### 存储期、内容比较与字符串表

字面量具有静态存储期，程序运行期间一直存在；函数可以返回字面量首地址，通常返回类型使用 const char *。普通局部 char 数组在函数返回后失效，不能返回其地址供后续访问。字面量不能 free。

相同内容的字面量可以共享或不共享存储，不依赖地址相同与否判断内容；使用 strcmp 比较。

```c
/* 三个指针组成的固定名称表：不能改指针元素，也不能通过它们改字符。 */
const char *const names[] = {"lidar", "imu", "motor"};
/* names[1] 是 "imu" 的首地址，names[1][1] 是字符 'm'。 */
```

若声明为 const char *names[]，仍可给 names[1] 赋另一个字符串地址。

### 相邻拼接与结束符陷阱

- 相邻字面量 "hello " "world" 在翻译过程中拼接，等价于 "hello world"；不是运行时 strcat，所需空格要写在引号内。
- "ab\0cd" 对应 6 个字符的数组，但 strlen 只计到第一个空字符，结果为 2，%s 也只输出 ab。
- char text[5] = "hello" 在 C 中允许，但没有空间保存 '\0'，不能直接作为字符串交给 strlen 或 %s。
- char text[] = "hello" 让编译器确定大小，得到包含结束符的 6 元素数组。

## 练习记录

### Q_1：用函数指针表统计字符类别

- 文件：[Q_1/main.c](../13_advanced_pointers/Q_1/main.c)。
- 题目要求：从标准输入读取字符，分别统计控制字符、空白、数字、小写、大写、标点和不可打印字符的百分比，不用一系列 if 判断类别。
- 使用结构体数组保存分类名称与函数指针：iscntrl、isspace、isdigit、islower、isupper、ispunct；不可打印由包装函数返回 !isprint(ch)。
- 用户框架中的 char ch 改为 int ch，完整保留 getchar 的字符值与 EOF；成功读取的值可直接传给 ctype 函数。
- 分类结果只保证零或非零，先用 != 0 转成 0/1 再累加。类别独立计数，换行可同时属于控制、空白、不可打印，百分比之和可能超过 100%。
- 空输入约定输出 0.00%，避免除零；检查总计数溢出及输入错误。采用启动时的 C locale，按字节分类，不处理 Unicode 字符分类。
- 实际通过 11 组测试：空输入、混合输入、数字、小写、大写、标点、空格、空白控制字符、NUL/DEL、无末尾换行、高位字节不被误当作 EOF。验证了各类数量和百分比。
- 本地测试验证的是标准输入实际读到的字符；Windows 文本流可能转换换行或处理文本 EOF 标记，不等于任意二进制文件的原始字节统计。

### Q_2：使用回调遍历单链表

- 文件：[Q_2/main.c](../13_advanced_pointers/Q_2/main.c)。
- 用户的遍历逻辑正确，但回调原来接收 int value，与题目要求不符；改为 void (*function)(struct NODE *node)，调用 function(current) 传节点地址。
- 遍历函数沿 next 访问每个节点一次；具体处理由回调决定。空回调直接返回。
- 回调可以修改 value，但不得修改链表链接、释放节点或使节点失效。遍历函数不拥有节点，不分配或释放内存。
- 实际通过 7 组检查：空链表、单节点、四节点、从中间开始、重复遍历、只读遍历与空回调、修改数据并保持链接。通过节点地址序列验证调用次数和顺序，不仅比较节点值。
- 测试回调用文件内静态状态记录地址，这是满足单参数回调约束的测试手段；遍历函数本身不使用该状态。
- 回调为 O(1) 时总体 O(n)，遍历函数额外空间 O(1)。

### Q_3：任意元素类型的通用排序

- 文件：[Q_3/main.c](../13_advanced_pointers/Q_3/main.c)。
- 保留用户的冒泡排序思路，接口改为 void sort(void *nums, size_t num, size_t size, int (*compare)(const void *, const void *))。
- num 为元素个数，size 为单元素字节数，不用 char 存储数量和下标；调用时用 sizeof array[0]，调用方负责确保实际容量足够。
- 转为 unsigned char * 后计算 bytes + j * size，比较回调接收元素地址，而不是元素的首字节值。
- 比较结果大于零时交换相邻元素，逐字节交换完整的 size 字节；原代码内层未随 j 移动地址、仅交换首字节的问题已修正。
- 只检查回调结果的符号，不要求恰好为 -1 或 1。整数比较用关系判断，不通过相减比较，避免极值引起有符号溢出。
- 字符串指针数组的回调接收指针元素的地址，需按 const char *const * 读取后再 strcmp；字符数组元素则可通过通用签名包装函数直接比较内容，不强制转换不兼容的函数指针。
- 检查空参数、零元素大小、少于两个元素和数量乘大小溢出。本接口无返回值，对上述无需排序或无效输入直接返回；检查不替代调用方的容量保证。
- 实际通过 14 组测试：空数组、单元素、已有序、逆序、重复值、全相等、整数极值、降序回调、double、字符串指针、结构体与稳定性、300 元素、300 字节元素及边界、无效参数。double 回调示例不定义 NaN 排序策略。
- 相等时不交换，所以是稳定排序；固定元素大小且比较为 O(1) 时最坏 O(n²)，已有序可提前结束为 O(n)，额外空间 O(1)。若元素大小为 s、比较开销为 C，最坏开销可记为 O(n²(C+s))。

### 验证说明

以上三题均已在各题目录用 MinGW64 GCC 和 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 编译，无编译警告，所列测试均实际通过。Q_1 通过外部脚本输入确定字节并核对输出；Q_2、Q_3 的测试保留在各自 main 函数中。正文概念片段的预期结果不视为独立测试结果。
