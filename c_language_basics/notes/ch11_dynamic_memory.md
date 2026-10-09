# 第十一章：动态内存分配 — 知识点总结

> 本章学习已完成（用户确认）。已记录 11.1～11.6 的讲解、相关提问及 Q_1～Q_3 的练习总结。
> 代码片段用于分别说明概念；完整扩容实例可独立编译。本文示例本次未进行编译运行，展示的结果是预期结果。

## 11.1 为什么使用动态内存分配

动态内存分配是在运行时根据需求申请空间，并由程序决定释放时机。

| 需求 | 固定局部数组的局限 | 动态分配的作用 |
|------|------|------|
| 数据量运行时才知道 | 固定容量可能浪费或不够 | 根据实际元素数量申请 |
| 函数返回后仍需保存数据 | 普通局部数组的生命周期结束 | 动态对象继续存在，直到被释放 |
| 数据不断增加 | 数组对象自身不能直接变大 | 调整动态内存容量或新增节点 |

保存地址的指针变量和所指对象有各自的生命周期：函数内的局部指针消失，不会自动释放它曾指向的动态内存。返回普通局部数组的地址不能延长数组的生命周期。

动态分配存在分配失败、管理开销和碎片等问题。固定三相电流可用 `float currents[3]`；规模可变的采样记录可以考虑动态分配。嵌入式实时任务也可能采用固定缓冲区、内存池或仅在初始化阶段分配。

## 11.2 malloc 和 free

头文件：`<stdlib.h>`。

```c
void *malloc(size_t size);  // 参数为总字节数，成功返回起始地址，失败返回 NULL
void free(void *ptr);      // 释放动态内存，不返回结果
```

### 按对象大小计算空间

```c
// 申请 5 个 int 的空间，不是仅申请 5 字节。
// sizeof *p 等于 sizeof(int)，此处不会实际解引用尚未赋值的 p。
int *p = malloc(5 * sizeof *p);

if (p == NULL) {
    // 分配失败属于运行中需要处理的情况，不应仅依赖 assert。
    fprintf(stderr, "Allocation failed.\n");
    return EXIT_FAILURE;
}

// malloc 不初始化内存，先写入，再读取。
for (size_t i = 0; i < 5; ++i) {
    p[i] = 0;
}

free(p);   // 释放的是所指内存，不是局部指针变量 p
p = NULL;  // 单独修改 p，避免继续保存失效地址
```

- 在 C 中，`void *` 可以转换为其他对象指针，不需要强制转换 `malloc` 的返回值。
- `sizeof p` 是指针大小；`sizeof *p` 才是所指对象大小。
- 对普通非变长类型，`sizeof` 不求值其操作数，不会真的通过 `p` 访问内存。
- `free` 不要求传入大小；传入 `NULL` 合法，不执行释放操作。
- 只能释放仍有效的动态分配块的起始地址；不能释放局部变量、普通数组、块中间地址，不能重复释放。
- `free` 不自动将指针清空，不保证擦除内容或立即把物理内存还给操作系统。
- 将 `p` 置空不会修复其他指向原块的别名指针。

### 防止计算大小时溢出

```c
// SIZE_MAX 由 <stdint.h> 提供；count 已经是经过业务检查的 size_t 数量。
int *p = NULL;

// 本例拒绝零数量；也可以在业务中专门处理空数据。
// 必须先除法检查，再乘法，不能等乘积溢出后再判断。
if (count == 0 || count > SIZE_MAX / sizeof *p) {
    return EXIT_FAILURE;
}

p = malloc(count * sizeof *p);
// 接下来仍然必须检查 p 是否为 NULL。
```

## 11.3 calloc 和 realloc

| 函数 | 大小参数 | 内容处理 |
|------|------|------|
| `malloc(bytes)` | 总字节数 | 未初始化 |
| `calloc(count, size)` | 元素个数、每个元素字节数 | 所有字节置零 |
| `realloc(ptr, bytes)` | 新的总字节数，不是新增字节数 | 保留新旧大小中较小范围内的原内容，新增部分不初始化 |

### calloc：申请并按位清零

```c
// 成功后，5 个 int 元素的值都是 0；使用前仍须检查返回值。
int *p = calloc(5, sizeof *p);
```

`calloc` 按字节清零，不能泛化为所有类型的语义初始化：C 不保证全零位模式代表空指针或浮点零。两个大小参数分开传递，避免调用者先做乘法而溢出；仍应限制业务数量、检查申请失败。

### realloc：临时指针接收结果

下面假设 `new_count` 非零，且已经检查过乘法溢出：

```c
// p 是原分配块的起始指针，也可以是 NULL。
int *tmp = realloc(p, new_count * sizeof *p);

if (tmp == NULL) {
    // 非零大小请求失败：原块仍有效，p 未丢失。
    // 调用者可以继续使用旧块，或释放后退出。
} else {
    p = tmp;              // 成功后，只通过返回的新指针访问
    capacity = new_count; // 成功后才能更新容量
}
```

- 不直接写 `p = realloc(p, ...)`，避免失败时覆盖唯一的旧地址，导致泄漏。
- 成功可能原地调整，也可能搬迁；旧指针及指向旧元素的别名都不能继续使用，即使数值地址看似相同。
- 可保留元素下标，成功后通过新指针重新计算元素地址，且需确认下标仍有效。
- 扩容后的新增空间不初始化，即使原块来自 `calloc` 也是如此。
- 缩容后，不能访问被缩掉的范围。
- `realloc(NULL, 非零大小)` 相当于 `malloc`。
- 不依赖零大小请求的标准版本或实现差异；需要释放时直接用 `free`。

## 11.4 使用动态分配的内存

### 常见访问方式

| 用途 | 分配示意 | 访问方式 |
|------|------|------|
| 单个整数 | `malloc(sizeof *p)` | `*p` |
| 整数数组 | `malloc(count * sizeof *p)` | `p[i]`，等价于 `*(p + i)` |
| 字符串副本 | `malloc(strlen(source) + 1)` | 复制字符及 `\0` 后按字符串使用 |
| 单个结构体 | `malloc(sizeof *motor)` | `motor->id`、`motor->speed` |

表中省略了失败检查和大小检查，实际代码不能省略。

动态数组的元素数量必须单独保存，`sizeof(p)` 不能得到分配块大小。数组容量和有效长度也不同：

```text
data → [10][20][空位][空位]
length = 2：已有两个有效元素
capacity = 4：总共能容纳四个元素
```

遍历有效数据看 `length`，判断能否追加看 `capacity`，保持 `length <= capacity`。

### 结构体指针成员需要单独管理

```c
struct Buffer {
    size_t count;  // 元素数量
    int *data;    // 只保存地址，不自带数组空间
};
```

申请 `sizeof(struct Buffer)` 只得到结构体空间，不会自动申请 `data` 所指数组。若外层结构体和内部数组分别分配，则通常先 `free(buffer->data)`，再 `free(buffer)`。内部申请失败，也要清理已申请的外层对象。

### 函数之间传递内存

```c
// values 借用调用者的数组；count 指明有效元素数量。
// 函数只读、不释放，调用者在全部使用结束后负责释放。
void print_values(const int *values, size_t count);
```

设计接口时明确申请者、使用者、可否修改、释放者和有效期。结构体复制中的指针成员只复制地址，不会自动复制所指的动态内存。

## 11.5 常见的动态内存错误

| 错误 | 典型原因 | 修正思路 |
|------|------|------|
| 分配失败后解引用 | 未检查 `NULL` | 每次分配后检查结果 |
| 分配空间过小 | 把元素数量当字节数、误用 `sizeof p` | 用 `count * sizeof *p` |
| 字符串越界 | 未给 `\0` 留空间 | 容量包含终止符 |
| 大小计算溢出 | 未检查数量乘法 | 先用 `SIZE_MAX / sizeof *p` 判断 |
| 读取未初始化数据 | 以为 malloc 或扩容会清零 | 先初始化，遍历有效长度 |
| 下标越界 | 把 `< count` 写成 `<= count` | 有效下标为 `0` 到 `count - 1` |
| 内存泄漏 | 覆盖唯一地址、退出时遗漏释放 | 保留管理指针，清理所有退出路径 |
| 释放后使用 | 原指针或别名仍访问旧块 | 结束所有借用后释放，不再访问 |
| 重复释放 | 多处都认为自己负责释放 | 明确单一释放责任 |
| 非法释放 | 释放局部对象或 `p + 1` | 释放有效分配块的起始地址 |
| realloc 丢失原块 | 直接覆盖原指针 | 临时指针接结果，成功才更新状态 |
| 嵌套资源泄漏或失效访问 | 只释放外层，或先释放外层再读内部地址 | 先释放内部资源，再释放外层 |

越界等错误不一定立即崩溃，可能稍后才表现出来。编译通过或运行一次正常，不代表内存管理正确。

## 11.6 内存分配实例

### 实例一：按输入规模保存读数

流程：读取数量并检查 → 计算空间并分配 → 逐项输入 → 计算平均值 → 释放。

- 先检查有符号输入为正且在业务范围内，再转成 `size_t`。
- `scanf("%lf", &readings[i])` 将数据写入第 `i` 个 `double` 元素；中途读取失败要释放数组再退出。
- `sum / (double)count` 计算平均值，数量必须非零。
- 此前的 `scanf` 示例假设输入数值可由目标类型表示；不能当作处理任意超大或恶意输入的完整解析器。

### 实例二：创建带动态名字的传感器

```c
struct Sensor {
    int id;
    char *name;  // 指向另外分配的字符串副本
};
```

`create_sensor` 的步骤：

1. 检查源字符串指针，计算长度并保证加一不溢出。
2. 申请结构体，失败返回 `NULL`。
3. 申请 `length + 1` 字节保存名字，失败先释放结构体。
4. 写入编号，通过 `memcpy(..., length + 1)` 复制字符和终止符。
5. 返回对象，由调用者最终调用配套销毁函数。

```text
sensor → [id][name 指针] → 独立的字符串副本
```

修改原字符串不影响副本。销毁时先释放名字，再释放结构体。源指针有效还不够，`strlen` 要求它指向可读取且以 `\0` 结尾的字符串。

### 实例三：容量不够时自动扩容（详细注释）

以下是独立程序；`append` 维护地址、有效长度和容量，调用者负责最终释放。

```c
#include <stdio.h>   // printf、fprintf、stderr
#include <stdlib.h>  // realloc、free、退出状态
#include <stdint.h>  // SIZE_MAX

struct IntArray {
    int *data;        // 动态数组的起始地址
    size_t length;    // 已写入的有效元素数量
    size_t capacity;  // 当前最多可以容纳的元素数量
};

/*
 * 追加一个整数：成功返回 1，失败返回 0。
 * array 必须指向正确初始化的管理结构，满足 length <= capacity。
 * 传入结构体指针，是为了修改调用者的地址、长度和容量。
 * 失败时保留原数据和状态，最终释放由调用者负责。
 */
static int append(struct IntArray *array, int value)
{
    // 第一步：只有空间用满才需要申请或扩容。
    if (array->length == array->capacity) {
        // 元素数量的计算上限，不代表实际可用内存有这么大。
        // sizeof *array->data 等于 sizeof(int)。
        size_t max_count = SIZE_MAX / sizeof *array->data;

        if (array->capacity >= max_count) {
            return 0;  // 已无法容纳更多元素，不修改原状态
        }

        // 第二步：选择新容量。首次申请从 2 开始，之后通常翻倍。
        size_t new_capacity;

        if (array->capacity == 0) {
            new_capacity = 2;
            if (new_capacity > max_count) {
                new_capacity = max_count;
            }
        } else if (array->capacity > max_count / 2) {
            // 先除法判断，避免翻倍时溢出；接近上限时只增至上限。
            new_capacity = max_count;
        } else {
            new_capacity = array->capacity * 2;
        }

        // 第三步：数量转成总字节数，前面的上限检查保证乘法安全。
        size_t new_bytes = new_capacity * sizeof *array->data;

        // 首次 data 为 NULL，相当于 malloc；后续调整并保留旧数据。
        // 临时指针接收结果，避免失败时覆盖唯一的旧块地址。
        int *tmp = realloc(array->data, new_bytes);
        if (tmp == NULL) {
            return 0;  // new_bytes 非零，旧块仍有效，可继续使用或释放
        }

        // 第四步：成功后才更新地址和容量，不能再使用旧块指针。
        array->data = tmp;
        array->capacity = new_capacity;
    }

    // 第五步：length 就是第一个空位置的下标，先写入再增加长度。
    array->data[array->length] = value;
    ++array->length;
    return 1;
}

int main(void)
{
    // 初始化指针为空，长度与容量为零；此时还没有整数数组空间。
    // 这是 C 的初始化语义，不是对 calloc 全零位模式的推断。
    struct IntArray array = {0};

    for (int value = 10; value <= 50; value += 10) {
        // 传入地址，让 append 能修改 main 中的管理结构。
        int success = append(&array, value);
        if (success == 0) {
            fprintf(stderr, "Append failed.\n");
            // 之前可能已经成功申请过内存；若为 NULL，free 也合法。
            free(array.data);
            return EXIT_FAILURE;
        }

        printf("Added %d: length = %zu, capacity = %zu\n",
               value, array.length, array.capacity);
    }

    printf("Values: ");
    // 只读取有效元素，不读取尚未初始化的剩余容量。
    for (size_t i = 0; i < array.length; ++i) {
        printf("%d ", array.data[i]);
    }
    putchar('\n');

    // 释放的是动态数组；array 是局部结构体，不能 free(&array)。
    free(array.data);
    array.data = NULL;
    array.length = 0;
    array.capacity = 0;
    return EXIT_SUCCESS;
}
```

预期容量变化（每次分配均成功且平台可容纳这些元素）：

| 追加值 | 有效长度 | 容量 |
|------:|------:|------:|
| 10 | 1 | 2 |
| 20 | 2 | 2 |
| 30 | 3 | 4 |
| 40 | 4 | 4 |
| 50 | 5 | 8 |

主线：**满了就扩容 → 成功后更新地址和容量 → 写入末尾 → 增加有效数量**。翻倍减少反复申请的次数，溢出检查保护容量计算。

## 补充提问

### 为什么使用 size_t？

- `size_t` 是表示对象大小的无符号整数类型，`sizeof` 和 `strlen` 的结果类型都是它，分配函数也使用它。
- 可通过 `<stddef.h>` 或相关标准头文件获得；具体宽度依赖平台，不固定为 32 位或 64 位。
- 内存字节数、数组容量、长度和下标常用它；可能为负的数量应考虑有符号类型。
- 打印使用 `%zu`，不能随意用 `%d`。
- 无符号减法可能回绕；`size_t i` 的 `i >= 0` 永远成立，不能这样控制倒序循环。

```c
// count 为零也能安全跳过循环，不会先计算 count - 1。
for (size_t i = count; i > 0; --i) {
    printf("%d\n", values[i - 1]);
}
```

### stderr 是什么？

`<stdio.h>` 提供标准流：`stdin` 用于输入，`stdout` 用于正常输出，`stderr` 用于错误输出。默认正常输出和错误输出常显示在同一终端，但可以分别重定向。

```c
printf("Result: %d\n", result);  // 默认写 stdout
fprintf(stderr, "Allocation failed.\n");  // 错误写 stderr
```

```powershell
.\main.exe > result.txt 2> error.txt
```

`fprintf` 只输出信息，不终止程序。`return EXIT_FAILURE` 从 `main` 返回，才结束程序并报告失败。

### for 中为什么用 ++i 而不是 i++？

`++i` 产生增加后的值，`i++` 产生增加前的值，两者都会增加变量。

```c
int i = 3;
int a = ++i;  // i 为 4，a 为 4
// 独立示例：若 i 原为 3，int a = i++; 则 a 为 3，i 为 4。
```

`for` 的第三部分不使用这个表达式的结果，因此对这里的整数变量，`++i` 和 `i++` 效果一样，不能简单声称前者更快。两者都在循环体之后执行。

## 练习题总结

### Q_1 — 用 malloc 实现 calloc

代码：[Q_1/main.c](../11_dynamic_memory/Q_1/main.c)。

**题意**：内部使用 `malloc`，分配指定数量和大小的元素，并将内存清零。

原实现的问题：

- `if (p = NULL)` 是赋值，会覆盖已分配的地址，条件结果为假；应使用 `p == NULL`。
- 只调用 `malloc` 并返回，没有完成 `calloc` 要求的清零。
- 直接计算 `num * size`，没有防止乘法溢出。

实现顺序：处理零参数 → 检查 `num > SIZE_MAX / size` → 计算总字节数 → `malloc` → 检查结果 → 清零全部字节 → 返回。

```c
// p 指向成功申请的内存，total 是经过溢出检查的总字节数。
// unsigned char 每次访问一个字节，适合清零任意对象的字节表示。
for (size_t i = 0; i < total; ++i) {
    p[i] = 0;
}
```

清零范围是 `num * size` 字节，不是 `num` 字节。用 `int *` 接收 malloc 本身不是错误，但按 int 下标清零不适合通用字节处理。

本实现对零大小请求选择返回 `NULL`；成功返回的内存由调用者 `free`。按位清零不应推广为任意类型的语义零值。

**验证**：整数数组清零并写入、3×7 共 21 字节的逐字节清零、单字节请求、三种零参数组合、三种乘法溢出请求。GCC 警告检查和断言测试通过；未强制模拟真实分配失败。

### Q_2 — 读取整数列表，并在首元素保存数量

代码：[Q_2/main.c](../11_dynamic_memory/Q_2/main.c)。以下测试结果来自此前检查时完成的自动输入测试。

**题意**：从标准输入读整数直到 EOF，返回动态数组，首元素保存输入整数的个数，其后保存数据。

```text
输入：10 20 30，再结束输入
返回：[3][10][20][30]
下标： 0   1   2   3
```

关键数量：`count` 只统计数据；`capacity` 包含计数槽。因此有效占用是 `count + 1` 个槽位。

原实现的问题及修正：

- 两处 `array = NULL` 改为比较，避免丢失地址。
- 初始只分配一个槽，已用于计数；第一个值写入 `array[1]` 前必须扩容。
- 满的条件是 `count == capacity - 1`，不能等写入后才扩容。
- 使用临时指针接收 `realloc`，成功才更新地址和容量；失败释放旧块。
- 溢出检查放在运算前，槽位数不得超过 `SIZE_MAX / sizeof *array`。
- 计数放在 `int` 元素中，还必须保证 `count <= INT_MAX`。
- `scanf` 返回 0 是匹配失败，不是 EOF；返回 EOF 后还检查 `ferror(stdin)`，避免把读取错误当作正常结束。

空输入返回有效的 `[0]`；失败返回 `NULL`，两种情况可区分。函数保留 `scanf("%d")` 方案，要求数字在 `int` 可表示范围内，不负责验证超范围数值转换。

**验证**：12 项自动输入测试通过，包括空 EOF、纯空白、无末尾换行、负数/零/重复值、当前 MinGW 的 int 边界、3/4/7/8/1000 个元素的扩容，以及首项或中途出现非法文本。完整对照数量、顺序和输出。未强制模拟内存耗尽或输入流故障。

### Q_3 — 读取没有固定长度上限的字符串

代码：[Q_3/main.c](../11_dynamic_memory/Q_3/main.c)；自动测试：[Q_3/test.ps1](../11_dynamic_memory/Q_3/test.ps1)。

**题意与本实现约定**：读取一行普通文本，保留空格和制表符，以换行或 EOF 结束，不保存换行；返回动态字符串，由 main 打印并释放。空行或立即 EOF 返回有效空字符串。

- 初始容量 16 字节只是起点，不是长度上限；满时按需翻倍，接近 `SIZE_MAX` 时避免乘法溢出。
- `length` 不包含终止符，`capacity` 包含终止符位置；当 `length == capacity - 1` 时，在存入新字符前扩容。
- 用 `int ch` 接收 `getchar`，确保可以区分普通字符值和 EOF。
- 字符直接复制到动态内存，不需要一个固定长度的中间数组。
- 使用临时指针接收 `realloc`；结束时补 `text[length] = '\0'`。
- 拒绝内嵌 NUL 字节，因为它会使 C 字符串提前结束；本题读取文本，不是任意二进制数据。
- 无人为长度上限仍受可用内存与大小类型限制；`strlen` 统计字节数，多字节中文不等于汉字数量。

**验证**：11 项自动测试通过，覆盖立即 EOF、空行、单字符、空格和制表符、首个换行结束、15/16/31/32 字节的扩容边界、100000 字符长输入及内嵌 NUL 拒绝。GCC 编译无警告；未强制模拟内存耗尽。

### 本章练习的共同检查顺序

1. 明确容量单位，以及计数槽或字符串终止符是否占位置。
2. 在乘法、加法和写入前检查边界，不等越界后再补救。
3. 分配失败时保留旧地址，并清理已经获得的资源。
4. 区分正常结束、空结果和失败，明确调用者的释放责任。
5. 用空输入、刚好装满、跨越容量边界和长输入验证结果，不能只看程序是否崩溃。
