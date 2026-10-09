# 第十章：结构和联合 — 知识点总结

> 本章学习已完成（用户确认）。本记录涵盖 10.1～10.6 的概念讲解，以及 Q_1、Q_2 的练习总结和 `assert` 补充说明；后续复习问题可继续追加。
> 示例以 C 语言为准；各代码块用于独立说明，不应直接拼接为一个程序。

## 10.1 结构基础知识

结构体把一组相关、类型可以不同的数据组合成一个整体。数组强调“同类元素的集合”，结构体强调“一个对象的多个属性”。

```c
struct Motor {
    int id;
    float speed;
    float temperature;
};  // 类型定义末尾要有分号

struct Motor motor = {1, 1500.0f, 36.5f};
```

- `struct Motor` 是类型，`motor` 是该类型的对象；只定义类型不会创建电机对象。
- 顺序初始化按成员声明顺序对应；C99 起可使用指定成员初始化。
- 函数内未初始化的普通局部结构体，其成员不会自动清零，应先赋值再读取。

```c
struct Motor motor = {.id = 1, .speed = 1500.0f, .temperature = 36.5f};
struct Motor stopped = {0};  // 这里的成员都初始化为数值零

motor.speed = 1800.0f;
stopped = motor;             // 同类型结构体可以整体赋值
```

结构体赋值复制成员的值，两个结构体对象仍独立。数组成员也随结构体整体复制，但指针成员只复制地址，不复制所指对象。

- C 不支持用 `==`、`!=` 直接比较两个结构体，应按需求比较成员。
- 字符数组成员可以在定义时用字符串初始化，但之后不能用 `=` 单独给数组赋值。

## 10.2 结构、指针和成员

### 访问表达式：每一步看左侧类型

```c
struct Date {
    int year;
    int month;
    int day;
};

struct Student {
    char name[20];
    int age;
    struct Date birthday;
    int *score;
};

int score = 90;
struct Student student = {"Li", 18, {2008, 5, 10}, &score};
struct Student *p = &student;
```

| 表达式 | 含义 | 类型 |
|------|------|------|
| `p` | 指向结构体的地址值 | `struct Student *` |
| `&p` | 指针变量自身的地址 | `struct Student **` |
| `*p` | 指针指向的整个结构体 | `struct Student` |
| `p->age` | 普通成员 | `int` |
| `&p->age` | 普通成员的地址 | `int *` |
| `p->birthday` | 嵌套的结构体 | `struct Date` |
| `p->birthday.year` | 嵌套结构体的成员 | `int` |
| `p->score` | 指针成员保存的地址 | `int *` |
| `*p->score` | 指针成员所指的整数 | `int` |
| `&p->score` | 指针成员自身的地址 | `int **` |

**左边是结构体对象用 `.`，左边是结构体指针用 `->`。**如果嵌套成员本身是结构体指针，就继续使用 `->`。

```c
p->age = 19;         // 等价于 (*p).age = 19
p->birthday.year = 2009;
*p->score = 100;     // 等价于 *(p->score) = 100
```

`.`、`->` 和后置 `++` 的优先级高于一元 `*`：

```c
(*p).age;           // 括号不能省，*p.age 会按 *(p.age) 解析
(*p->score)++;      // 增加所指整数
// *p->score++;     // 按 *(p->score++) 解析，增加的是指针成员
```

区分“改变指向”和“改变所指对象”：

```c
int another_score = 80;
p->score = &another_score;  // 改变指针成员保存的地址
*p->score = 95;             // 修改 another_score
```

`p->score` 要求 `p` 指向有效的结构体；`*p->score` 还要求 `p->score` 指向有效整数。打印地址使用 `printf("%p\n", (void *)p)`。

### 自引用与不完整类型

```c
struct Node {
    int value;
    struct Node *next;
};
```

自引用是“成员指向同一种类型”，不要求指向当前对象自己。结构体不能直接包含自身类型的对象，否则大小无法确定；可以包含自身类型的指针。

```c
struct Node second = {20, NULL};  // NULL 可由 <stddef.h> 提供
struct Node first = {10, &second};
// first.next->value 为 20；second.next 为 NULL，表示没有后续节点
```

```c
struct Node;  // 前向声明：知道类型存在，还不知道成员和大小
```

| 类型尚未完整时的操作 | 是否允许 |
|------|------|
| 定义 `struct Node *p` | 允许，指针本身的大小已知 |
| 定义 `struct Node node` 对象 | 不允许，需要知道对象大小 |
| `sizeof(struct Node)` | 不允许 |
| `p->value` | 不允许，尚不知道成员 |

结构体定义内部，标签名已可用，但类型尚未完整，因此可以声明 `struct Node *next`。前向声明还可用于两个类型互相持有指针，以及在头文件中隐藏具体成员。

使用 `typedef` 时，别名必须先声明才能使用：

```c
typedef struct Node {
    int value;
    struct Node *next;  // 此处使用已可见的标签，不使用末尾才声明的别名
} Node;
```

也可以先写 `typedef struct Node Node;`，再在完整定义里使用 `Node *next`。

## 10.3 结构的存储分配

普通成员按声明顺序排列，开头没有填充，成员之间和末尾可能有填充。`sizeof` 包含这些填充。

对齐要求为 4 字节，表示起始地址应是 4 的整数倍。具体大小、对齐和布局由目标平台及实现决定。

```c
struct A { char a; int b; char c; };
struct B { int b; char a; char c; };
```

**假设** `char` 大小和对齐均为 1，`int` 大小和对齐均为 4，结构体采用常见的自然对齐布局：

```text
A：[a:1][填充:3][b:4][c:1][尾部填充:3] → 12 字节
B：[b:4][a:1][c:1][尾部填充:2]         →  8 字节
```

- 按各成员的对齐要求安排位置，再按整个结构体的对齐要求补齐末尾。
- 常见普通布局中，结构体对齐要求等于成员最严格的对齐要求；不能将其作为所有编译选项和平台下的绝对规则。
- 尾部填充使结构体数组中的每个元素都能满足对齐要求，相邻元素距离为 `sizeof(结构体类型)`。
- 嵌套结构体以自身完整大小和对齐要求参与外层布局，不会把内部成员重新摊开排列。
- 指针成员只计入指针本身的大小，不包含所指数据；数组成员的存储就在结构体内部。

观察实际布局：

```c
#include <stdio.h>
#include <stddef.h>

struct Example { char a; int b; char c; };

int main(void)
{
    printf("Size: %zu\n", sizeof(struct Example));
    printf("Alignment: %zu\n", _Alignof(struct Example));  // C11
    printf("b offset: %zu\n", offsetof(struct Example, b));
    return 0;
}
```

填充字节不属于成员，不应假定其值固定。不能用 `memcmp` 比较整个结构体来代替成员值比较，也不要直接把本机结构体布局当作跨平台通信格式。

## 10.4 作为函数参数的结构

**C 一律按值传递参数：传结构体时复制结构体值，传指针时复制地址值。**

| 参数形式 | 含义 | 常见用途 |
|------|------|------|
| `struct Motor motor` | 结构体副本 | 小对象、独立计算 |
| `struct Motor *motor` | 指向原对象 | 修改原对象 |
| `const struct Motor *motor` | 通过此指针只读原对象 | 查询、打印 |

```c
void stop_copy(struct Motor motor)
{
    motor.speed = 0.0f;  // 只改副本
}

void stop_motor(struct Motor *motor)
{
    motor->speed = 0.0f;  // 调用方须提供有效地址
}

void print_motor(const struct Motor *motor)
{
    printf("Speed: %.1f\n", motor->speed);
}
```

调用分别是 `stop_copy(motor)`、`stop_motor(&motor)`、`print_motor(&motor)`。

- 函数中改变指针形参的指向，不改变调用者的指针；通过指针写成员可以修改原对象。
- `const struct Motor *p` 限制通过 `p` 修改结构体；`struct Motor *const p` 限制指针本身改指向。
- 大结构体传指针通常可以减少复制开销，小结构体传值也可能高效，实际性能取决于编译器和调用约定。

### 指针成员与浅拷贝

结构体副本中的指针和原指针指向同一对象，所以“按值传入结构体”不保证外部数据不会被修改。

```c
struct Sensor { int id; int *value; };

void update(struct Sensor sensor)
{
    sensor.id = 2;        // 只改副本
    *sensor.value = 100;  // 修改共享的外部整数
}
```

`const struct Sensor *p` 也不会让指针成员所指的数据自动只读：不能写 `p->value = NULL`，但在目标有效且可写时可以写 `*p->value = 100`。

### 返回结构体

```c
struct Motor stopped_motor(struct Motor motor)
{
    motor.speed = 0.0f;
    return motor;
}
```

返回局部结构体的值是合法的；不能返回指向普通局部结构体的指针供调用者访问，因为函数结束后该对象已失效。返回值中的指针成员也需要单独保证所指对象的生命周期。

## 10.5 位段

位段（位域）为整数成员指定二进制位宽，适合表示标志或小范围状态值。

```c
struct MotorStatus {
    unsigned int enabled : 1;   // 0～1
    unsigned int direction : 1; // 0～1
    unsigned int mode : 2;      // 0～3
    unsigned int error : 4;     // 0～15
};
```

- `n` 位无符号位段范围是 `0～2^n-1`；使用 `.`、`->` 访问。
- 明确使用 `unsigned int` 或 `signed int`；普通 `int` 位段的符号性由实现决定。
- 超出范围的值不能原样保存，应在赋值前检查。例如给上述 `mode` 赋常量 `7` 会转换为 `3`，编译器可能警告。
- 位宽是整数常量表达式，不能由运行时变量决定，且不能超过所声明类型允许的宽度。
- 不能对位段取地址，也不能单独对位段使用 `sizeof`；可以对整个结构体使用 `sizeof`。

```c
struct Flags {
    unsigned int ready : 1;
    unsigned int : 3;      // 无名位段：留出不访问的位
    unsigned int error : 1;
    unsigned int : 0;      // 后面的位段不再放进当前分配单元
    unsigned int mode : 2;
};
```

零宽位段必须无名。位段的排列方向、跨分配单元行为、对齐等依赖实现，不能只把位宽相加就断定结构体大小。

对于位位置固定的协议或寄存器，显式掩码与移位更清晰：

```c
unsigned int flags = 0;
flags |= 1u;                              // 设置 bit 0
flags &= ~(1u << 1);                       // 清除 bit 1
flags = (flags & ~(3u << 2)) | (2u << 2);  // bit 2～3 写入模式 2
unsigned int mode = (flags >> 2) & 3u;     // 读取模式
```

硬件读写还必须遵守芯片手册。修改位段或执行掩码写入可能涉及“读—改—写”；`volatile` 不保证原子性，也不会消除寄存器本身的特殊读写语义。

## 10.6 联合

联合的成员共用同一块存储空间，所有成员从同一个位置开始。通常一次使用其中一个成员保存值。

```c
union Data {
    int integer;
    float real;
    char text[8];
};

union Data data = {.real = 3.5f};  // C99 指定成员初始化
data.integer = 42;                // 覆盖共享存储，不再期待 real 保持原值
```

- 不指定成员名时，初始化第一个成员，例如 `union Data data = {42};`。
- 访问方式与结构体相似：对象用 `.`，指针用 `->`。
- 大小至少足够容纳最大成员，并满足对齐要求，可能大于最大成员大小。
- 例如 `union { int i; char bytes[5]; }` 在 `int` 大小和对齐均为 4 的常见布局下通常占 8 字节。
- 写较小成员不代表整个联合的所有字节都被覆盖或清零。
- 写入一个成员后读取另一个成员不是数值转换；按另一种类型解释对象表示依赖平台，还可能涉及陷阱表示。普通业务代码应读取当前保存值的成员。

### 类型标记与联合配合

联合不会自己记录当前使用哪个成员，常用枚举标记管理：

```c
enum ValueType { VALUE_INT, VALUE_FLOAT };

struct Value {
    enum ValueType type;
    union {
        int integer;
        float real;
    } data;
};

struct Value value = {
    .type = VALUE_FLOAT,
    .data = {.real = 36.5f}
};
```

读取前检查 `type`，根据标记选择 `value.data.integer` 或 `value.data.real`。切换存储类型时同步更新标记。

| 对比 | 结构体 `struct` | 联合 `union` |
|------|------|------|
| 存储 | 各成员各占空间 | 所有成员共用空间 |
| 大小 | 所有成员加上可能的填充 | 容纳最大成员并满足对齐 |
| 用途 | 同时保存多个属性 | 保存多种可能形式中的一种 |

电机的编号、转速和温度适合结构体；一条传感器数据在整数计数和浮点温度之间二选一，适合带类型标记的联合。

## 问题与练习总结

### Q_1 — 长途电话记账信息

代码：[10_structures/Q_1/main.c](../10_structures/Q_1/main.c)。

**题目要求**：记录通话日期、时间，以及主叫、被叫、付账三个电话号码；每个电话号码都由区号、交换台号码和站号码组成。

**原解法检查**：结构层次和字段齐全，符合题意。`PHOEN_NUMBER` 是命名拼写问题，统一改为 `PHONE_NUMBER`；声明放在 `main` 内合法，但类型仅在该代码块的作用域内可用，需要由多个函数使用时应移到文件作用域。

```c
struct PHONE_NUMBER {
    int area;
    int exchange;
    int station;
};

struct LONG_DISTANCE_BILL {
    int date;  // YYYYMMDD
    int time;  // HHMMSS
    struct PHONE_NUMBER called;
    struct PHONE_NUMBER calling;
    struct PHONE_NUMBER billed;
};
```

**设计要点**：

- 三个号码结构相同，复用一个类型；三个号码要同时保存，所以用三个结构体成员，不能用联合互相覆盖。
- `called` 是被叫，`calling` 是主叫，`billed` 是付账电话；付账电话可以与主叫相同，也可以不同。
- 用整数保存日期和时间时，需要明确编码约定。`20260925` 表示 2026-09-25，`143005` 表示 14:30:05，午夜用 `0` 表示。
- 拆分时间可用 `time / 10000`、`time / 100 % 100`、`time % 100`；用 `%02d` 输出时补齐两位。
- 这种声明负责存储，不会自动检查日期、时间或号码是否合法。
- 电话号码是标识而非用于计算的数量；本题的整数方案可表达所选示例，若要保留前导零、不同位数等信息，字符数组更合适。

**整体赋值不等于共享对象**：

```c
bill.billed = bill.calling;
bill.billed.station = 8888;
assert(bill.calling.station == 5678);
```

赋值复制了成员值，修改付账号码不会改变主叫号码。本题成员中没有指针，因此复制整个记账结构后，各成员也是独立保存的。

**已验证**：三个号码分别初始化和读取、修改其中一个不影响其他号码、修改记录副本不影响原记录、午夜输出为 `00:00:00`。此前已用 GCC 警告选项编译，无警告，断言全部通过。

### 补充问题 — `assert` 是什么？

`assert` 是 `<assert.h>` 提供的**断言宏**，不是普通函数，用来检查“运行到这里时应该成立的条件”。

```c
#include <assert.h>

assert(bill.date == 20260925);
```

- 条件非零：继续执行，不主动输出成功信息。
- 条件为零：输出包含表达式、文件和行号等内容的诊断信息，并异常终止程序。
- 测试末尾的 `puts("All tests passed.")` 是我们自己添加的提示，不是断言自动打印的。
- `if` 用于正常业务分支和错误处理；`assert` 用于测试和检查程序内部假设，不应代替用户输入校验。

定义 `NDEBUG` 可以关闭断言：在包含 `<assert.h>` 前写 `#define NDEBUG`，或编译时加 `-DNDEBUG`。关闭后连条件表达式都不会求值，因此不要把必要操作放进断言里。

```c
// 不推荐：assert(++count == 10); 关闭断言后 count 不会增加
++count;
assert(count == 10);
```

测试时应保持断言启用，否则程序末尾的成功提示不能证明检查真正执行过。

### Q_2 — 汽车销售信息记录

代码：[10_structures/Q_2/main.c](../10_structures/Q_2/main.c)。

**题目要求**：每份记录都有顾客名字、地址和车型；交易分现金、租赁、贷款三种，各自保存不同的附加信息。

| 交易类型 | 附加字段 |
|------|------|
| 现金 | 建议零售价、实际售价、营业税、许可费用 |
| 租赁 | 建议零售价、实际售价、预付定金、安全抵押、月付金额、租赁期限 |
| 贷款 | 建议零售价、实际售价、营业税、许可费用、预付定金、贷款期限、贷款利率、月付金额、银行名称 |

按题目要求，金额和利率使用 `float`，期限使用 `int`。代码约定期限单位为月、利率用小数表示（`0.05f` 表示 5%）；这些是示例的存储约定，题目没有规定具体单位。测试只验证存取，不计算税费和还款金额。

**字符串容量必须多留一个位置**：题目明确最大长度不包括结尾的 NUL 字符 `\0`。

| 字段 | 最大字符串长度 | 字符数组容量 |
|------|------:|------:|
| 顾客名字 | 20 | 21 |
| 顾客地址 | 40 | 41 |
| 车型 | 20 | 21 |
| 银行名称 | 20 | 21 |

**设计：公共信息 + 类型标记 + 联合**。

```c
enum SALE_TYPE { CASH_SALE, LEASE_SALE, LOAN_SALE };

// CASH_INFO、LEASE_INFO、LOAN_INFO 在此结构之前分别完整定义。
struct SALES_RECORD {
    char customer_name[21];
    char customer_address[41];
    char model[21];
    enum SALE_TYPE type;
    union {
        struct CASH_INFO cash;
        struct LEASE_INFO lease;
        struct LOAN_INFO loan;
    } details;
};
```

- 公共信息与交易详情需要同时存在，外层用结构体。
- 一份记录只属于一种交易，三类详情用联合共享存储。
- 每类详情中的字段需要同时存在，所以联合内的每个成员又是一个结构体。
- `type` 记录当前使用哪个联合成员；它不会自动随成员赋值而更新，写入和读取时必须保持一致。
- 各类型分别声明完整字段，方便直接对照题目。重复字段定义不代表一份记录会同时分配三份详情空间，联合仍按其大小和对齐要求分配。

**嵌套访问与只读参数**：

```c
static void print_record(const struct SALES_RECORD *record);

// 已知 record->type == LOAN_SALE 时：
// record->details.loan.bank_name
```

`record` 是指针，用 `->`；`details` 是联合对象，用 `.`；`loan` 是结构体对象，再用 `.`。打印函数根据 `type` 分支读取对应成员，`const` 表明不通过参数修改记录。

**已验证**：

- 现金、租赁、贷款各一条记录，检查附加字段并运行全部三种打印分支。
- 顾客名字、地址、车型、银行名称分别填入最大长度字符串，检查长度及最后的 `\0`。
- 复制贷款记录后修改副本中的字符数组，原记录保持不变。数组成员会随结构体复制，不会像指针成员那样仅复制地址。
- 测试使用已知长度的字符串常量，容量足够才调用 `strcpy`；实际接收外部输入时仍需检查长度。
- 浮点断言比较的是直接保存的同一 `float` 常量，不涉及运算累积误差；不能照搬为所有浮点计算的比较方式。

此前已用 GCC 的 `-std=c11 -Wall -Wextra -Wpedantic -Wshadow -Wconversion` 编译，无警告，断言全部通过，三种记录输出符合测试数据。

### 两题的共同思路

**需要同时存在的信息用结构体；互相替代、一次只用一种的信息用带类型标记的联合。**先按题意拆分数据关系，再确定类型、容量、单位和访问方式。
