# 第十六章：标准函数库 — 知识点总结

> 根据本次可用对话中 16.1～16.8 的讲解整理，记录日期：2026-10-06。用户已确认第十六章学习完成；已补充 Q_1、Q_2 的实现、修正和测试记录。进度以 [学习计划](../STUDY_PLAN.md) 为准。
> 本文代码是教学片段或独立示例，未单独编译运行；不把讲解、文件存在或编译通过等同于掌握。本文以常见的 C11/C17 用法为主，区分标准保证与平台实现。

## 16.1 整型函数

### 绝对值：abs、labs、llabs

头文件为 `<stdlib.h>`。`abs(int x)` 返回 `int`，`labs(long x)` 返回 `long`，`llabs(long long x)` 返回 `long long`；结果是参数的绝对值，适用于整数偏差大小等计算。

```c
/* error 是有方向的偏差，magnitude 只表示偏差大小。 */
int error = -25;
int magnitude = abs(error); /* 25。 */
```

没有通用失败返回值。若绝对值无法由返回类型表示，行为未定义；在常见平台上，`abs(INT_MIN)` 就有此问题。不能认为改用同类型的函数即可消除溢出。`<limits.h>` 提供 `INT_MIN`、`INT_MAX`、`LONG_MIN`、`LONG_MAX`、`LLONG_MIN`、`LLONG_MAX` 等范围宏；具体位宽不应写死。浮点绝对值用 `fabs`，不要用 `abs`，否则会先发生整数转换。

### 商和余数：div、ldiv、lldiv

头文件为 `<stdlib.h>`。

| 函数 | 参数 | 返回类型 | 成员类型 |
| --- | --- | --- | --- |
| `div(numerator, denominator)` | 两个 int | div_t | int |
| `ldiv(numerator, denominator)` | 两个 long | ldiv_t | long |
| `lldiv(numerator, denominator)` | 两个 long long | lldiv_t | long long |

返回结构体的 `quot` 是商，`rem` 是余数。C99 起商向零截断，满足“被除数 = 商 × 除数 + 余数”；非零余数与被除数同号。

```c
/* 同时获得完整组数和剩余数量；不假定一定比 / 与 % 更快。 */
div_t result = div(17, 5); /* quot 为 3，rem 为 2。 */
div_t negative = div(-17, 5); /* quot 为 -3，rem 为 -2。 */
```

调用前保证除数不为零，且结果可表示；常见平台的最小负数除以 -1 会溢出。这组函数没有可依赖的错误返回值，不能等待它们替你报错。

### 字符串转换：strtol、strtoll、strtoul、strtoull

头文件为 `<stdlib.h>`。原型形式是 `返回类型 function(const char *str, char **endptr, int base)`。

| 函数 | 返回类型 |
| --- | --- |
| strtol | long |
| strtoll | long long |
| strtoul | unsigned long |
| strtoull | unsigned long long |

- `str`：待转换字符串，不会被修改。
- `endptr`：若非 NULL，接收第一个未转换字符的地址；通过 `&end` 修改调用者的指针，因此参数是二级指针。指向原字符串，不需要释放。
- `base`：2～36 或 0；10 表示十进制，16 表示十六进制。0 自动判断常见前缀：普通数字十进制、前导 0 八进制、0x/0X 十六进制。要求十进制输入时显式使用 10。
- 跳过前导空白，并接受正负号；无数字时返回 0，且停止位置等于起点。
- 范围错误设置 `errno = ERANGE`：有符号转换返回相应最小值或最大值，无符号转换返回相应最大值。不能只凭返回 0 或边界值判断失败。
- 无符号转换也接受负号，不能用 strtoul 自动拒绝负数；非负输入规则需在跳过前导空白后检查负号。

```c
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    const char *text = "123"; /* 输入字符串。 */
    char *end = NULL;         /* 接收停止位置，不拥有字符串内存。 */
    errno = 0;               /* 清除旧错误值，供本次范围检查使用。 */
    long value = strtol(text, &end, 10);

    if (end == text) /* 完全没有转换出数字。 */
    {
        fprintf(stderr, "No integer found.\n");
        return EXIT_FAILURE;
    }
    if (errno == ERANGE) /* 首先检查 long 的表示范围。 */
    {
        fprintf(stderr, "Long range error.\n");
        return EXIT_FAILURE;
    }
    if (*end != '\0') /* 本例拒绝尾随字符，包括末尾空白。 */
    {
        fprintf(stderr, "Unexpected trailing characters.\n");
        return EXIT_FAILURE;
    }
    if (value < INT_MIN || value > INT_MAX) /* 再检查目标 int 范围。 */
    {
        fprintf(stderr, "Int range error.\n");
        return EXIT_FAILURE;
    }
    int number = (int)value; /* 检查后才转换，避免先转换再判断。 */
    printf("Number: %d\n", number);
    return EXIT_SUCCESS;
}
```

`atoi`、`atol`、`atoll` 同在 `<stdlib.h>`，参数是字符串，分别返回 int、long、long long。无法可靠区分合法 0 和无数字输入，不能检查停止位置，结果不可表示时行为未定义；需要可靠输入检查时优先用 strto 系列。

### 伪随机整数：rand、srand、RAND_MAX

这两个函数和宏都在 `<stdlib.h>` 中。

| 名称 | 参数、返回值和含义 |
| --- | --- |
| `int rand(void)` | 无参数，返回 0～RAND_MAX 的伪随机整数，两端包含；没有规定的失败返回值 |
| `void srand(unsigned int seed)` | seed 是种子，初始化后续伪随机序列；没有返回值 |
| `RAND_MAX` | rand 的最大返回值，标准保证至少为 32767，具体值依实现 |

伪随机数是算法根据内部状态计算出来的序列，不是真正随机事件。同一实现中，用相同种子重新初始化，会重复相同序列；不同实现不保证序列相同。未调用 srand 就调用 rand，效果相当于先 srand(1)。

固定种子适合调试和生成可复现的测试数据，例如 `srand(123U)`。通常只在开始时初始化一次，不要每生成一个数就重新设置种子，否则可能不断取得同一序列的首项。

#### 时间种子与典型用法

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
    /* 获取当前日历时间，并在转换种子之前检查获取失败。 */
    time_t now = time(NULL);
    if (now == (time_t)-1)
    {
        fprintf(stderr, "Cannot obtain current time.\n");
        return EXIT_FAILURE;
    }

    /*
     * 常见平台用时间值构造 unsigned int 种子，只初始化一次。
     * 转换可能丢失信息，不保证每次运行都产生不同种子。
     */
    srand((unsigned int)now);

    for (int index = 0; index < 5; ++index)
    {
        /* 每次调用推进内部状态，取得下一个整数。 */
        int value = rand();
        printf("%d\n", value);
    }
    return EXIT_SUCCESS;
}
```

在常见按秒表示时间的平台上，同一秒启动可能得到相同种子。time_t 的编码由实现决定，不能把时间种子的惯用写法理解为标准保证的唯一种子。不要在循环中反复 `srand((unsigned int)time(NULL))`。

#### 指定整数范围与溢出边界

```c
/* 取余得到 0～5，再加 1，结果为 1～6；用于简单骰子练习。 */
int dice = rand() % 6 + 1;

/* 结果为 10～20，两端都包含。 */
int value = rand() % 11 + 10;
```

常见形式是 `minimum + rand() % (maximum - minimum + 1)`，但必须先保证 maximum >= minimum，范围长度和最终结果的计算不溢出，取余除数非零，且目标范围适合单次 rand 的输出跨度。不能对任意 int 上下界直接套公式；范围大于输出跨度时，单次取余无法覆盖全部值。

#### 取余偏差与拒绝采样

即使原始输出等概率，原始结果数量不能被目标范围整除时，取余也会产生偏差。假设原值为 0～9，模 6 后 0～3 各对应两个原值，4～5 各对应一个原值。

可丢弃末尾不足一整组的结果，使接受区间的数量能被目标范围整除：

```c
/*
 * rand 共可能返回 RAND_MAX + 1 个整数。
 * 先转 unsigned long 再加 1，避免在 int 中计算 RAND_MAX + 1 溢出。
 */
unsigned long span = (unsigned long)RAND_MAX + 1UL;

/* 保留可均分为六类的最大前缀，丢弃多出来的尾部值。 */
unsigned long limit = span - span % 6UL;
unsigned long sample;

do
{
    sample = (unsigned long)rand(); /* 取得一个候选值。 */
}
while (sample >= limit); /* 位于被丢弃尾部时重新取样。 */

/* 已接受的区间能均分为六类，加 1 后形成骰子点数。 */
int dice = (int)(sample % 6UL) + 1;
```

这种方法叫拒绝采样，只消除这里的取余映射偏差，不能改善 rand 本身的随机质量；标准不保证其理想均匀性。示例只针对六种结果，推广到其他范围时还要保证接受区间非空以及范围计算安全。

#### 场景与限制

- 适合简单游戏、生成练习数据、可复现测试和模拟传感器输入。
- 不适合密码、密钥或安全令牌，因为序列可能可预测。
- 不默认多线程共享调用安全，平台线程行为需另外核对。
- 本节代码为讲解示例，本次补充未编译运行；不能据此声称随机分布已经验证。

## 16.2 浮点型函数

### 类型、精度与错误报告

大部分数学接口在 `<math.h>`：无后缀通常处理 double，后缀 f 处理 float，后缀 l 处理 long double，例如 sqrt、sqrtf、sqrtl。long double 的实际精度由实现决定，不保证总比 double 高。

浮点数通常是近似表示，例如常见二进制格式不能精确表示 0.1。有限值比较可根据业务容差使用 `fabs(actual - expected) <= tolerance`；固定绝对容差不适合所有量级，NaN、无穷等需单独处理。

数学函数的错误机制由 `math_errhandling` 指示：`MATH_ERRNO` 表示使用 errno，`MATH_ERREXCEPT` 表示使用浮点异常，可能两者都有。不要假定所有平台都用 errno 报告所有数学错误。先检查定义域；若依赖 errno，按实现声明的机制在调用前清零并检查。定义域、极点、溢出、下溢的结果与报告方式存在实现差异。

### 常用计算接口

下表均为 double 版本，参数和返回值为 double；三角函数涉及的角度单位是弧度。

| 函数 | 参数与结果含义 | 边界或场景 |
| --- | --- | --- |
| fabs(x) | 返回绝对值 | 浮点偏差大小 |
| sqrt(x) | 返回平方根 | 实数输入要求 x >= 0 |
| cbrt(x) | 返回立方根 | 可以处理负数 |
| pow(x, y) | 返回 x 的 y 次幂 | 负底数与非整数指数等产生定义域问题；可能溢出 |
| hypot(x, y) | 返回 sqrt(x²+y²) | 求距离，避免不必要的中间溢出或下溢；最终结果仍可能溢出 |
| exp(x)、exp2(x) | 返回 e 的 x 次幂、2 的 x 次幂 | 注意溢出与下溢 |
| log(x)、log10(x)、log2(x) | 返回自然、十进制、二进制对数 | 正数有效；零是极点，负数是定义域错误 |
| sin(x)、cos(x)、tan(x) | 正弦、余弦、正切 | 输入弧度，注意正切奇点附近的数值问题 |
| asin(x)、acos(x) | 反正弦、反余弦 | 输入范围 [-1,1]，输出弧度 |
| atan(x) | 反正切 | 输出弧度，单个比值无法保留全部象限信息 |
| atan2(y, x) | 按坐标分量求方向角 | 参数顺序 y、x，可区分象限；零向量由业务另行处理 |

```c
const double pi = 3.14159265358979323846; /* 不依赖非通用的 M_PI。 */
double degrees = 30.0;
double radians = degrees * pi / 180.0; /* 角度先转换成弧度。 */
double sine = sin(radians);           /* 约为 0.5。 */
double distance = hypot(3.0, 4.0);    /* 5.0，适合坐标距离。 */
double direction = atan2(1.0, -1.0); /* 约为 135 度对应的弧度。 */
```

平方通常直接写 `x * x`，比 pow(x, 2.0) 更直观。

### 取整与余数

| 函数 | 规则 | 2.7 | -2.7 |
| --- | --- | --- | --- |
| floor | 向负无穷 | 2.0 | -3.0 |
| ceil | 向正无穷 | 3.0 | -2.0 |
| trunc | 向零 | 2.0 | -2.0 |
| round | 就近，正好一半时远离零 | 3.0 | -3.0 |

这四个函数返回 double，不是整数类型。round(2.5) 为 3.0，round(-2.5) 为 -3.0。`lround(double)` 返回 long，`llround(double)` 返回 long long，采用相同舍入规则；舍入结果超出返回类型范围时不能依赖正常结果，也不能把它们当自动溢出处理工具。浮点转整数前要考虑目标范围和非有限值。

`fmod(x, y)` 返回浮点余数，规则相当于 `x - trunc(x/y) * y`，非零结果与 x 同号；例如 fmod(5.5,2.0)=1.5，fmod(-5.5,2.0)=-1.5。不能对浮点数使用 `%`。除数应非零，非有限参数也需按接口规则处理。

`remainder(x,y)` 使用最近整数商，恰好一半时选择偶数商，结果不等同于 fmod；例如 remainder(5.5,2.0) 为 -0.5。

### 特殊数值分类

`<math.h>` 的 `isfinite(x)`、`isinf(x)`、`isnan(x)` 分别判断有限值、无穷值、NaN，成立返回非零，不保证是 1。不要用 `value == NAN` 判断 NaN。支持哪些特殊浮点值取决于实现。

```c
/* 非有限值不能作为本例正常的传感器读数。 */
if (!isfinite(value))
{
    fprintf(stderr, "A finite value is required.\n");
}
```

### strtod、strtof、strtold 与 atof

这组转换在 `<stdlib.h>`，原型形式为 `返回类型 function(const char *str, char **endptr)`；分别返回 double、float、long double。没有 base 参数，支持十进制、小数、科学计数法等规定形式，也可识别无穷和 NaN 文本。受 LC_NUMERIC 的小数点规则影响。

检查顺序：清零 errno → 转换 → end==text 判断无数字 → errno==ERANGE 判断范围错误 → 检查尾随字符 → isfinite 判断业务是否接受结果。溢出会报告 ERANGE，下溢也可能报告 ERANGE；返回 0 不能独自说明成功或失败。

```c
/* text 是输入，end 指向原字符串内部，无释放责任。 */
const char *text = "1.25e3";
char *end = NULL;
errno = 0;
double value = strtod(text, &end); /* 正常结果为 1250.0。 */
if (end == text || errno == ERANGE || *end != '\0' || !isfinite(value))
{
    /* 这里只合并示意；实际程序可分步输出具体失败原因。 */
    fprintf(stderr, "Invalid floating-point input.\n");
}
```

`atof(const char *)` 返回 double，缺乏可靠的停止位置和错误检查接口，不适合严谨解析。数学库链接按项目配置处理，需要时把 `-lm` 放在源文件或目标文件之后。

## 16.3 日期和时间函数

### 类型与成员

头文件为 `<time.h>`。`time_t` 保存日历时间编码，`clock_t` 保存处理器时间计数，两者表示由实现决定；标准不要求 time_t 必须是从 1970 年起的整数秒。`struct tm` 保存拆开的日期时间。

| 成员 | 含义与范围 |
| --- | --- |
| tm_year | 从 1900 年起算的年数，显示年份需加 1900 |
| tm_mon | 0～11，显示月份需加 1 |
| tm_mday | 当月日期 1～31 |
| tm_hour、tm_min | 小时 0～23，分钟 0～59 |
| tm_sec | 秒 0～60，包含闰秒空间 |
| tm_wday | 星期 0～6，星期日为 0 |
| tm_yday | 当年第几日 0～365，1 月 1 日为 0 |
| tm_isdst | 正数：夏令时；0：非夏令时；负数：未知 |

### 接口、返回值与生命周期

| 接口 | 参数与作用 | 返回值、失败与边界 |
| --- | --- | --- |
| time(timer) | timer 为 time_t 指针或 NULL，取得当前日历时间 | 返回时间；指针非空则另存；失败 (time_t)-1 |
| localtime(timer) | time_t 地址，转本地 struct tm | 返回库管理的对象指针，失败 NULL |
| gmtime(timer) | time_t 地址，转 UTC struct tm | 返回库管理的对象指针，失败 NULL |
| mktime(calendar) | struct tm 地址，按本地时间反向转换 | 返回 time_t，不能表示时 (time_t)-1；成功会规范化并修改成员 |
| difftime(end,beginning) | 两个日历时间 | 返回 double，单位秒，计算 end-beginning |
| clock() | 获取程序处理器时间计数 | 返回 clock_t，不能取得时 (clock_t)-1 |
| strftime(buffer,capacity,format,calendar) | 格式化到字符数组，容量包含结束符 | 返回字符数，不含结束符；放不下返回 0，不能使用该结果缓冲区 |
| asctime(calendar) | struct tm 地址，生成固定格式文本 | 库管理的字符串，含末尾换行；严格保证不如 strftime 适合可控输出 |
| ctime(timer) | time_t 地址，生成本地固定格式文本 | 相当于 asctime(localtime(timer))，不宜代替可检查的分步转换 |

localtime、gmtime 以及固定格式接口的返回存储可能被后续相关调用覆盖，不释放；需保存时立即复制结构体或字符串。不要默认线程安全。UTC 和本地时间描述同一时刻，区别是表示规则；本地时区由运行环境决定。

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
    time_t now = time(NULL); /* 先取得日历时间。 */
    if (now == (time_t)-1)
    {
        fprintf(stderr, "Cannot obtain time.\n");
        return EXIT_FAILURE;
    }
    struct tm *ptr = localtime(&now); /* 库管理对象，不 free。 */
    if (ptr == NULL)
    {
        fprintf(stderr, "Cannot convert time.\n");
        return EXIT_FAILURE;
    }
    struct tm calendar = *ptr; /* 独立副本，不依赖后续覆盖行为。 */
    char buffer[64];           /* 容量单位为字节，包含结束符空间。 */
    size_t length = strftime(buffer, sizeof buffer,
                             "%Y-%m-%d %H:%M:%S", &calendar);
    if (length == 0)
    {
        fprintf(stderr, "Cannot format time.\n");
        return EXIT_FAILURE;
    }
    printf("Local time: %s\n", buffer);
    return EXIT_SUCCESS;
}
```

strftime 常用符号：`%Y` 年、`%m` 月、`%d` 日、`%H` 24 小时制小时、`%M` 分钟、`%S` 秒、`%A` 完整星期名称、`%B` 完整月份名称、`%j` 年内日序 001～366、`%%` 百分号；月份和分钟大小写不同。`%x`、`%X`、`%c` 与文字名称受 LC_TIME 影响。返回 0 也可能是合法格式生成空字符串，所以需结合所用格式判断。

### mktime 与计时边界

构造本地日期时先 `struct tm calendar = {0};`，设置年减 1900、月减 1，并设置 `tm_isdst=-1` 让库尝试判断夏令时。mktime 忽略输入的 tm_wday/tm_yday，成功会计算它们。它可能将 1 月 32 日规范化为 2 月 1 日；成功不等于原日期合法，严格日期验证还需比较规范化后的成员。不能把 UTC 的 struct tm 直接交给 mktime 当成 UTC 转换。

计算日历差用 difftime，不依赖 time_t 的底层编码。日历时钟可能被校时调整，不适合可靠测量短时耗时；单调时钟属于平台接口。

clock 的处理器秒数通常按 `(double)(finish - start) / CLOCKS_PER_SEC` 换算，先检查两个返回值，并确保差值可表示、计数未回绕。CLOCKS_PER_SEC 是比例，不是实际分辨率保证。按标准语义它测 CPU 消耗，等待输入或休眠通常消耗很少 CPU；具体实现有差异。

## 16.4 非本地跳转

头文件 `<setjmp.h>`：`jmp_buf` 保存恢复执行所需的环境，`setjmp` 是宏，`longjmp(env,value)` 跳回恢复点且不会正常返回到调用处。它们不复制全部程序内存，也不撤销此前写入的数据。

| 情况 | setjmp 的表现 |
| --- | --- |
| 首次直接调用 | 返回 0 |
| longjmp(env,5) | 原 setjmp 位置返回 5 |
| longjmp(env,0) | 原 setjmp 位置返回 1 |

return 返回一层调用，goto 只能跳同一函数标签；longjmp 可跨过多层调用，回到仍有效的 setjmp 环境。

```c
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

static jmp_buf recovery; /* 本例只使用一个恢复点。 */

static void read_data(void)
{
    /* 模拟无法继续的读取错误，1 是自定义错误编号。 */
    longjmp(recovery, 1);
}

int main(void)
{
    /* 首次进入正常分支；跳回后进入错误分支。 */
    if (setjmp(recovery) == 0)
    {
        read_data();
    }
    else
    {
        fprintf(stderr, "Read error.\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
```

setjmp 的调用上下文有严格限制。常用合法形式是控制条件中的整个表达式或规定的与整数常量比较，例如 `if (setjmp(env)==0)`；也可作为独立表达式语句。不要使用 `int code = setjmp(env);`，也不要把它随意嵌入复合表达式。

恢复点所属函数必须仍在执行；保存环境的函数已经返回后再跳回是未定义行为，即使 jmp_buf 仍存在。不能跨线程使用环境；涉及变长数组退出作用域的情形也有额外限制。

在调用 setjmp 的函数中，具有自动存储期、非 volatile、在 setjmp 与 longjmp 之间被修改的局部对象，在恢复后值不确定，不能依赖。确有需要时可用 `volatile int count`，而不是给所有变量加 volatile。全局/静态对象以及堆内存不会自动回滚。

longjmp 会跳过中间函数的 free、fclose 等代码，不能自动释放资源。恢复入口需有明确的资源记录和清理责任。普通练习优先采用错误返回值或同一函数内的 goto cleanup。不要把它当成异步信号的通用恢复方案。

## 16.5 信号

### 接口与信号宏

头文件 `<signal.h>`。信号是一种事件通知，不等同于嵌入式硬件中断。外部信号可能异步打断正常执行。

`signal(int sig, void (*handler)(int))` 设置处理方式，返回之前的处理方式；失败返回 SIG_ERR。处理函数形式为 `void handler(int signal_number)`。SIG_DFL 请求默认处理，SIG_IGN 请求忽略，SIG_ERR 是失败标志，不应作为处理方式注册。

`raise(int sig)` 主动产生信号，成功返回 0，失败非零；若处理函数正常返回，raise 在其结束后返回。

| 宏 | 含义 |
| --- | --- |
| SIGINT | 交互式中断，常见终端中 Ctrl+C 产生 |
| SIGTERM | 终止请求 |
| SIGABRT | 异常终止，如 abort |
| SIGFPE | 算术异常，不仅限浮点 |
| SIGILL | 非法指令 |
| SIGSEGV | 无效内存访问 |

编号、默认动作和终端行为依实现而异。不能假定未定义行为必然触发信号；真实算术或内存异常也不能靠处理函数返回后继续执行来通用恢复。

### 处理函数只设置标志

`sig_atomic_t` 是适合在异步中断存在时作为原子实体访问的整数类型。常用静态存储期对象 `static volatile sig_atomic_t stop_requested=0;`。volatile 表示值可能被正常流程外改变，sig_atomic_t 提供相应单次访问保证；不是通用线程同步，也不保证 `++` 这样的读改写整体原子。

异步信号处理函数受严格限制，不应随意调用 printf、malloc、free、fopen 等；通用教学模式是只向 volatile sig_atomic_t 标志赋值，在主流程中读取并清理。

```c
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

static volatile sig_atomic_t stop_requested = 0;

static void handle_interrupt(int signal_number)
{
    (void)signal_number; /* 本例不区分编号。 */
    stop_requested = 1;  /* 不在这里输出或释放资源。 */
}

int main(void)
{
    if (signal(SIGINT, handle_interrupt) == SIG_ERR)
    {
        fprintf(stderr, "Cannot install handler.\n");
        return EXIT_FAILURE;
    }
    if (raise(SIGINT) != 0) /* 主动产生一次信号用于示意。 */
    {
        fprintf(stderr, "Cannot raise signal.\n");
        return EXIT_FAILURE;
    }
    if (stop_requested)
    {
        /* 已回到主流程，在这里进行实际资源清理。 */
        printf("Stop requested.\n");
    }
    return EXIT_SUCCESS;
}
```

标准允许不同的处理器保持/重置行为，不能默认一次注册永久有效。设置停止标志也不保证主流程立刻退出，阻塞调用何时返回取决于平台。非 raise 等规定来源的 SIGFPE、SIGILL、SIGSEGV 处理函数返回可能导致未定义行为。POSIX 的 sigaction、Windows 控制台事件等属于平台扩展。

## 16.6 打印可变参数列表

### 参数状态与默认提升

`...` 表示数量可变的参数，本身不包含数量和类型信息，需要数量参数、格式字符串等协议。`<stdarg.h>` 提供以下类型与宏：

| 名称 | 含义与责任 |
| --- | --- |
| va_list | 记录访问参数的状态，不假定是指针或数组 |
| va_start(args,last_named) | C11/C17 常见写法，使用最后一个固定参数初始化状态 |
| va_arg(args,type) | 按指定类型取得下一个参数并推进状态，无自动越界/类型检查 |
| va_copy(copy,args) | 复制当前状态，供独立遍历；不依赖普通赋值 |
| va_end(args) | 结束访问，每次 start/copy 都必须对应 end |

传经省略号的 float 提升为 double；char、short 等执行整数提升，通常是 int，具体也可能为 unsigned int。因此读取 float 实参用 `va_arg(args,double)`。参数不足或不兼容类型读取会产生未定义行为（标准规定的少数兼容例外不应当作日常接口设计依据）。

自定义整数求和可用 count 指定数量，循环 `int value=va_arg(args,int);`，调用方保证实参数量和类型匹配，且累计结果不溢出。不要认为 va_list 可以自行得知列表长度。

### vprintf、vfprintf、vsnprintf

头文件 `<stdio.h>`。

| 接口 | 参数与输出目标 | 返回值 |
| --- | --- | --- |
| vprintf(format,args) | 格式字符串、va_list，输出 stdout | 成功字符数，错误为负数 |
| vfprintf(stream,format,args) | FILE 指针、格式、va_list，输出指定流 | 成功字符数，错误为负数 |
| vsnprintf(buffer,capacity,format,args) | 字符数组、字节容量、格式、va_list | 空间充分本应生成的字符数，不含结束符；编码错误负数 |

不能用 printf(format,args) 展开列表；它把 args 当作普通的一个实参。格式与实际提升后的类型必须匹配。消费列表的函数可能推进状态，调用后不能随意再次使用原状态；需要重复格式化，事先 va_copy，并对每份状态 va_end。

```c
#include <stdarg.h>
#include <stdio.h>

/* 返回 0 表示各次输出成功，-1 表示至少一次输出失败。 */
static int print_log(const char *format, ...)
{
    va_list args;
    va_start(args, format); /* 从最后一个固定参数之后开始访问。 */
    int prefix = fputs("[LOG] ", stderr);
    int body = vfprintf(stderr, format, args); /* 由格式解释各参数。 */
    va_end(args); /* 失败路径也必须结束本次访问。 */
    int newline = fputc('\n', stderr);
    if (prefix == EOF || body < 0 || newline == EOF)
    {
        return -1;
    }
    return 0;
}
/* 调用示例：print_log("Sensor %d: %.2f", 3, 12.75); */
```

vsnprintf 容量包含结束符。容量大于零时正常格式化会终止字符串；返回负数先处理错误，再用 `(size_t)result >= capacity` 判断截断。完整容量需要 result+1 字节，但计算或分配时仍需检查大小可表示。容量为 0 时不写入，可用 NULL 缓冲区查询长度；要进行第二次格式化需使用独立的参数状态。不能把返回值误认为实际保存字符数。

## 16.7 执行环境

宿主环境通常有操作系统和完整标准库；独立环境常见于裸机或底层系统，启动方式与部分库支持由实现规定。不能默认单片机具备环境变量、命令解释器、文件系统。下列接口在 `<stdlib.h>`。

### getenv 与 system

`getenv(const char *name)` 查询环境变量，返回字符串地址，未找到返回 NULL。名称和内容依执行环境，字符串由库管理，不修改、不释放，后续调用可能覆盖，长期保存应复制。值是文字，数字需另行 strtol 等检查转换。

```c
const char *path = getenv("PATH"); /* 不取得内存所有权。 */
if (path != NULL)
{
    printf("PATH: %s\n", path);
}
```

`system(const char *command)` 交给命令解释器执行。command 为 NULL 时查询是否有命令解释器，非零表示可用，零表示不可用；非空命令的返回值解释由实现决定，不能套用统一状态码。`system("dir")` 是 Windows 场景，命令并不跨平台。不能直接拼接未经处理的用户输入，否则特殊字符可能形成额外命令；普通文件操作优先使用库接口。

### exit、atexit、abort 与退出状态

| 接口或宏 | 含义、参数、结果 |
| --- | --- |
| exit(status) | 正常终止整个程序，不返回；status 传给执行环境 |
| EXIT_SUCCESS | 成功状态，0 也表示成功 |
| EXIT_FAILURE | 失败状态，具体数值由实现决定 |
| atexit(function) | 注册 void function(void)，成功 0，失败非零 |
| abort() | 引发 SIGABRT，异常终止，不返回；处理函数不返回的情况另论 |

普通函数 return 只返回调用者；初次调用的 main 返回相当于 exit。正常终止会按注册逆序调用 atexit 函数、刷新关闭标准 I/O 流、移除 tmpfile 创建的文件。不会逐层执行函数末尾代码，也不会自动执行用户的 free；资源应按约定主动清理。

```c
/* 两个退出函数都没有参数和返回值。 */
static void cleanup_first(void) { puts("Cleanup first."); }
static void cleanup_second(void) { puts("Cleanup second."); }
/* 在 main 中依次注册，检查失败；正常退出时 second 先执行。 */
```

退出处理函数所需状态必须届时仍有效，不能保留已经结束的普通局部对象地址。abort 不调用 atexit，流是否刷新/关闭等清理由实现决定。正常终止流程不等于业务成功：exit(EXIT_FAILURE) 仍是正常终止。崩溃、断电和强制结束不能依赖退出回调。

## 16.8 locale

### 类别与 setlocale

头文件 `<locale.h>`。locale 是地区和语言规则，不等同于文件编码，也不是自动解决所有中文乱码的开关。程序启动等价于 `setlocale(LC_ALL,"C")`，不自动采用系统语言。

`setlocale(int category,const char *locale)` 成功返回描述所选设置的库管理字符串，失败 NULL，失败不改变原设置。不修改、不释放返回字符串，后续调用可能覆盖。

| 参数 | 含义 |
| --- | --- |
| locale 为 "C" | 标准基本规则，必须支持，数值小数点为 . |
| locale 为 "" | 请求环境提供的本地规则 |
| locale 为 NULL | 查询，不修改 |
| 其他名称 | 由实现决定，不保证跨平台支持 |

| 类别 | 影响 |
| --- | --- |
| LC_ALL | 所有类别 |
| LC_NUMERIC | 非货币数值格式、打印/解析的小数点 |
| LC_MONETARY | 货币表示规则 |
| LC_TIME | 日期时间格式、月份星期名称 |
| LC_CTYPE | 字符分类、大小写和多字节转换 |
| LC_COLLATE | 地区字符串排序规则 |

strtod、scanf 等受 LC_NUMERIC 影响：小数点为逗号时，输入 12.5 可能只读到 12；固定使用点号的通信协议需要保持明确解析规则。可在初始化时采用 LC_ALL 的环境设置，再单独设 LC_NUMERIC 为 C，并检查每次返回值。setlocale 是共享状态，不宜在各函数或多线程里随意切换。

LC_TIME 影响 strftime 的文字名称和 %x/%X/%c。LC_CTYPE 影响 isalpha、toupper 等，但 ctype 参数仍须为 EOF 或 unsigned char 可表示值：char 应先转换为 unsigned char。逐字节调用 isalpha 不等于处理完整 UTF-8 汉字。

`<string.h>` 的 strcmp 按字节值比较，不使用地区排序；strcoll 按 LC_COLLATE 比较，返回负、零、正分别表示排序前、相等、后；不保证中文 locale 一定按拼音排序。

### localeconv 与 struct lconv

`struct lconv *localeconv(void)` 返回库管理的当前数值/货币格式信息，不修改、不释放；后续 localeconv 或相关 setlocale 可能覆盖内容。需要长期保存时不只复制结构体，还要考虑其字符串成员的存储。

| 成员 | 含义 |
| --- | --- |
| decimal_point | 非货币小数点字符串 |
| thousands_sep、grouping | 非货币分组分隔符、分组规则 |
| currency_symbol、int_curr_symbol | 本地、国际货币符号字符串 |
| mon_decimal_point | 货币小数点 |
| mon_thousands_sep、mon_grouping | 货币分组分隔符、规则 |
| positive_sign、negative_sign | 货币正负号字符串 |
| frac_digits、int_frac_digits | 本地、国际货币小数位数 |

还有描述符号位置、空格、正负号位置的成员。空字符串可能表示信息不可用；数值 char 成员可能用 CHAR_MAX 表示不可用。grouping/mon_grouping 是按字节编码的组大小，不是可用 %s 展示的普通文字：0 表示重复上一组大小，CHAR_MAX 表示不再分组。

```c
if (setlocale(LC_ALL, "") == NULL)
{
    fprintf(stderr, "Cannot set locale.\n");
}
else
{
    const struct lconv *rules = localeconv(); /* 不拥有该对象。 */
    printf("Decimal point: %s\n", rules->decimal_point);
    printf("Currency symbol: %s\n", rules->currency_symbol);
}
```

标准 C 的 printf("%f",value) 不会仅因设置 locale 自动增加千位分隔符。面向用户的显示可采用地区规则，协议和文件格式则应保持明确的一致约定。

## 练习与验证记录

用户已确认第十六章学习完成。以下记录依据本次可用对话中的实际检查与测试；正文教学示例未单独编译运行。

### Q_1：年龄表示与最小基数

源码：[Q_1/main.c](../16_standard_library/Q_1/main.c)。题目要求从命令行接收十进制年龄，在 2～36 中寻找使其表示不大于 29 的最小基数，例如 41 在十六进制中为 29。

原实现使用 scanf 而非命令行参数，只尝试 2、4、8、16、32，并以 age*10/base 判断，既不等同进制转换，也可能溢出。修正后使用 argc/argv、strtol 和 ERANGE 检查输入，从 2 到 36 逐个尝试。

对非负年龄，quotient=age/base、remainder=age%base。quotient>=base 意味着至少三位，不符合要求；否则 quotient<2 或 quotient==2 且 remainder<=9 时符合。首次符合即最小基数，不用乘法构造新值，也不使用 strcmp 比较数字串。返回 0 表示搜索无解，main 报告错误并返回 EXIT_FAILURE。

输入策略：接受非空十进制数字串、年龄 0 和前导零；拒绝负号、正号、空白、小数、尾随字符及超出 long 的数值。缺少参数或多余参数也失败。

| 测试输入 | 结果 |
| --- | --- |
| 0、1、3 | 基数 2 |
| 4、8 | 基数 3 |
| 9 | 基数 4 |
| 29 | 基数 10 |
| 30 | 基数 11 |
| 41、00041 | 基数 16 |
| 72 | 基数 32 |
| 81 | 基数 36，表示为 29 |
| 82、100 | 在 2～36 中无解，失败退出 |
| -1、abc、41abc、1.5、+41、前导空白、超长整数 | 拒绝并失败退出 |
| 缺少参数、额外参数 | 用法错误，失败退出 |

已使用项目 GCC 和 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 编译，无警告；以上最终代码用例已通过。空串分支已通过代码检查，本次对话未记录其独立执行测试。

### Q_2：等概率骰子与首次初始化

源码：[Q_2/main.c](../16_standard_library/Q_2/main.c)。题目要求函数返回 1～6，首次调用时用当前时间设置种子。

原实现只在 main 中生成一个值，未封装函数，rand()%6+1 有取余偏差。本机 RAND_MAX=32767，共 32768 个原值，其中点数 1、2 各对应 5462 个原值，点数 3～6 各对应 5461 个原值。对原表达式进行 60 万次采样，结果都在范围内，但统计接近不能证明等概率。

修正后 roll_dice(void) 使用 static initialized 记录初始化状态。首次调用 time(NULL)，失败返回 0 且不标记成功；成功后 srand，再设置标志，后续调用不重复设置种子。main 检查 0 并报告错误。

计算 span=(unsigned long)RAND_MAX+1UL，limit=span-span%6UL；不断取得 sample，拒绝 sample>=limit，最后返回 sample%6+1。本机拒绝 32766、32767，剩余 32766 个原值可均分成六类。先转换再加 1 避免 int 溢出。此方法消除取余映射偏差，等概率仍以 rand 原始输出均匀为前提；标准 C 不保证理想随机质量，不用于安全随机数。

已使用同样警告选项编译，无警告，并运行程序。受控测试替换时间和随机接口后直接测试真实 roll_dice 函数，验证：时间失败返回 0、后续成功可重试、成功只设置一次种子、拒绝两个尾部值、连续返回全部六种点数。测试已通过，临时测试文件已移除。没有把单次实际输出或统计测试当成严格均匀性证明。

### 本章回顾

- 输入转换不仅要获得数值，还要检查结束位置、目标范围和业务要求。
- 浮点计算区分定义域、精度和错误报告机制；时间区分日历时间与处理器时间。
- 非本地跳转不自动清理资源；信号处理优先设置标志，主流程清理。
- 可变参数遵守类型提升与 va_list 生命周期；locale 是共享的表示规则。
- 随机数区分种子、序列复现、范围映射和分布质量。
