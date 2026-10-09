# 第十五章：输入输出函数 — 知识点总结

> 根据本次可用对话中 15.1～15.16 的讲解及 `atexit` 的追问整理。用户已确认第十五章学习完成；学习进度以 [学习计划](../STUDY_PLAN.md) 为准。正文片段是概念示例，未单独编译运行，Q_1～Q_4 的实际验证记录见文末。

## 15.1 错误报告

先检查接口的返回值，再按该接口的约定查询错误原因。不能仅凭 `errno != 0` 判断刚才的操作失败：成功调用不一定清零 `errno`，也不是每个接口失败都会设置它。需要在后续调用前保留错误编号时，立即用 `int saved_errno = errno;` 保存。

- `<errno.h>` 提供 `errno`；`<string.h>` 的 `strerror(errnum)` 将错误编号转换为描述字符串。
- `<stdio.h>` 的 `perror(prefix)` 根据当前 `errno` 向 `stderr` 输出前缀和错误描述。只有在相关接口约定设置 `errno` 时，描述才可用于解释该次失败。
- 正常结果输出到 `stdout`，错误诊断输出到 `stderr`，便于分别重定向。
- `fopen` 失败返回 `NULL`。常见运行库会设置 `errno`，但严格的 ISO C 不保证每次 `fopen` 失败都提供对应错误编号。

## 15.2 终止执行

从 `main` 返回和调用 `exit(status)` 都是正常终止；`EXIT_SUCCESS`、`EXIT_FAILURE` 表示向执行环境报告成功或失败，不能把“正常终止流程”误解为“执行成功”。正常终止时会调用 `atexit` 注册的函数，并处理输出流的刷新与关闭。`abort()` 是异常终止，不调用这些退出函数；不要依赖它完成正常清理。

### 本次疑问：为什么没有显式调用，却输出 `Program finished.`？

```c
#include <stdio.h>
#include <stdlib.h>

/* 退出处理函数没有参数，也不返回值。 */
static void finish_message(void)
{
    puts("Program finished.");
}

int main(void)
{
    /* 传入函数地址并注册，不在此刻执行函数体。 */
    if (atexit(finish_message) != 0)
    {
        fprintf(stderr, "Error: cannot register exit handler.\n");
        return EXIT_FAILURE;
    }

    puts("Program running.");

    /* main 正常返回时，C 运行库调用已注册的 finish_message。 */
    return EXIT_SUCCESS;
}
```

执行顺序是“注册函数 → 输出 `Program running.` → `main` 返回 → 运行库调用 `finish_message` → 输出 `Program finished.`”。`finish_message` 是传入的函数地址；`finish_message()` 才是在当前位置直接调用。若注册多个退出函数，它们按注册顺序的逆序执行。断电、崩溃或被强制结束时不能依赖退出处理函数运行。

## 15.3～15.5 标准 I/O 库、ANSI I/O 与流总览

`<stdio.h>` 提供标准 I/O 接口。`FILE *` 用来操作流，指向库维护的流控制对象，并非指向整个文件内容；不要依赖 `FILE` 的内部布局，也不要用 `free` 代替 `fclose`。

| 标准流 | 用途 | 常见函数 |
| --- | --- | --- |
| `stdin` | 标准输入 | `getchar()`、`scanf()` |
| `stdout` | 标准输出 | `putchar()`、`printf()` |
| `stderr` | 错误诊断 | `fprintf(stderr, ...)` |

标准流可能被重定向，不能假定它们始终连接键盘或屏幕。旧称“ANSI I/O”指标准 C 的流 I/O。文本流可能进行换行等外部表示转换，程序中的一个 `\n` 不一定对应文件中的一个字节；二进制流适合需要保留原始字节的数据。具体文本转换依平台而异。

按数据形式选择接口：字符 I/O（`fgetc`、`fputc`）、未格式化文本 I/O（`fgets`、`fputs`）、格式化 I/O（`fprintf`、`fscanf`）、二进制 I/O（`fread`、`fwrite`）。一个文件流通常经历“打开 → 读写并检查结果 → 区分结束与错误 → 关闭并检查结果”。成功打开的一方通常负责关闭；接受调用者传入 `FILE *` 的函数不应擅自关闭，除非接口约定移交所有权。

## 15.6～15.7 打开和关闭流

`fopen(path, mode)` 成功返回 `FILE *`，失败返回 `NULL`。`freopen` 可让现有流重新关联文件，使用时也要检查结果。

| 模式 | 含义 |
| --- | --- |
| `r` | 只读，要求文件存在 |
| `w` | 写入，创建文件或截断已有文件 |
| `a` | 追加写入，文件不存在则创建 |
| `r+` | 读写，要求文件存在，保留原内容 |
| `w+` | 读写，创建或截断文件 |
| `a+` | 读取并追加写入 |

加 `b` 指定二进制模式，例如 `rb`、`wb`。选择 `w`、`w+` 前确认允许丢弃旧内容。更新流（含 `+`）从写转读通常先刷新或定位，从读转写通常先定位；读到文件末尾时有标准规定的例外，不能无条件直接交替读写。

`fclose(fp)` 成功返回 `0`，失败返回 `EOF`。写入函数成功后，缓冲数据仍可能在关闭时写出失败，因此要检查 `fclose`。调用 `fclose` 后不再使用该流指针，也不能再次关闭它；若打开失败，没有流需要关闭。

## 15.8 字符 I/O

`fgetc(fp)`、`getc(fp)` 和 `getchar()` 读取字符；对应的输出函数是 `fputc(ch, fp)`、`putc(ch, fp)` 和 `putchar(ch)`。输入返回值必须先存入 `int`，以便同时表示所有可能的字符值和额外的 `EOF`。读取返回 `EOF` 后用 `ferror`、`feof` 区分错误与文件结束；`EOF` 不是文件内的普通字符。`ungetc` 可把字符退回输入流供后续读取。

```c
/* ch 用 int 保存读取结果；stream 是已打开、由调用方负责关闭的输入流。 */
int ch;
while ((ch = fgetc(stream)) != EOF)
{
    /* 只有成功取得字符后才使用它；写入失败时停止。 */
    if (fputc(ch, stdout) == EOF)
    {
        fprintf(stderr, "Error: output failed.\n");
        break;
    }
}
```

## 15.9 未格式化的行 I/O

`fgets(buffer, capacity, stream)` 最多读取 `capacity - 1` 个字符，成功时补 `\0`；若读到换行，会保留换行。返回 `NULL` 时，本次没有得到可返回的字符串，应结合流状态判断。长行可能分多次读完，末行也可能没有换行。`fputs` 向指定流写字符串，不自动添加换行；`puts` 向 `stdout` 写字符串并添加换行。两者写出的文本不包含字符串结尾的 `\0`。`gets` 不检查容量，已从 C 标准移除。

## 15.10 格式化的行 I/O

`printf`、`fprintf`、`snprintf` 把数据转换为文本；`scanf`、`fscanf`、`sscanf` 把文本按格式转换为对象值。格式与实参类型必须匹配。`fprintf` 成功返回写出的字符数，失败返回负值；`snprintf` 还应检查返回的所需字符数是否达到目标容量，以发现截断。

`fscanf` 等输入函数返回成功赋值的项目数，期望两个值就检查是否等于 `2`；未成功赋值的对象不能按有效结果使用。用 `%s` 写入数组时指定宽度，并为结尾 `\0` 留空间。格式化输入不会自动以一行为边界；处理用户输入时可先用 `fgets` 取得整行，再进行解析。

## 15.11 二进制 I/O

`fread(ptr, size, count, stream)` 和 `fwrite(ptr, size, count, stream)` 中，`size` 的单位是每个元素的字节数，`count` 是元素数，返回值也是已完成的元素数。短读要用 `feof`、`ferror` 区分文件结束和错误；短写要作为写入未完成处理。不要把未完整读入的元素当作有效对象。

直接将结构体内存映像写入文件，可能受填充、字节序、类型宽度和表示方式影响；传感器协议或长期保存的数据应明确规定每个字段的编码。

## 15.12 刷新和定位函数

- `fflush` 对输出流要求写出标准库缓冲的数据，返回 `EOF` 表示失败；它不保证数据已永久写入物理介质。不要用 `fflush(stdin)` 作为标准 C 的清输入方法。
- `fseek(stream, offset, origin)` 成功返回 `0`；`origin` 可以是 `SEEK_SET`、`SEEK_CUR`、`SEEK_END`。`ftell` 返回当前位置，失败返回 `-1L`。
- 文本流的位置值不一定是字节编号；可保存 `ftell` 的结果，之后交给 `fseek` 恢复。`rewind` 回到开头并清除结束、错误指示器，但没有可直接检查的返回值。
- `fgetpos` 将位置存入 `fpos_t` 对象，`fsetpos` 用保存的位置恢复；不要自行解释 `fpos_t` 的内部表示。

## 15.13 改变缓冲方式

`setvbuf` 可以选择全缓冲 `_IOFBF`、行缓冲 `_IOLBF` 或无缓冲 `_IONBF`；`setbuf` 是较简单的接口。应在打开流后、对该流执行其他操作前配置。若由调用方提供缓冲区，必须保证其存活到流不再使用缓冲区为止。缓冲影响数据传出的时机，仍需检查写入、刷新和关闭结果。

## 15.14 流错误函数

`feof` 查询文件结束指示器，`ferror` 查询错误指示器，`clearerr` 清除两者。结束指示器是在读取操作尝试越过末尾后才设置的，因此用读取函数结果驱动循环，不用 `while (!feof(stream))` 预测下一次读取。`clearerr` 只清除记录的状态，不修复引发错误的原因。

## 15.15 临时文件

`tmpfile()` 创建可读写的二进制临时流，成功返回 `FILE *`，使用后应 `fclose`。临时文件在关闭时被自动删除。`tmpnam()` 只生成名称，不创建文件；从获得名称到创建文件之间可能有名称冲突或竞争。只需临时存储时优先考虑 `tmpfile`，并注意运行环境可能限制临时文件的创建。

## 15.16 文件操纵函数

`remove(path)` 删除指定文件，`rename(old_path, new_path)` 重命名文件；两者成功返回 `0`，失败返回非零。它们按路径操作，与用 `FILE *` 读写流不同。调用前确认目标路径，检查返回值；不要假定目标已存在时的覆盖行为，或对仍打开文件的操作行为在各平台一致。

## 函数、类型与符号使用手册

下面按实际编程时遇到的顺序，把上文涉及的名字展开。函数原型中的参数名只是说明用途，参数类型和顺序才是接口的一部分。`size_t` 表示对象大小或元素数量，`FILE *` 表示库管理的流，`EOF` 是输入失败或结束时使用的负整数宏，不是文件里存储的一个字符。

#### 诊断与终止：15.1～15.2

| 名称及头文件 | 参数、含义与用法 | 返回结果和注意事项 |
| --- | --- | --- |
| `errno`，`<errno.h>` | 一个可修改的 `int` 左值，用于读取接口约定设置的错误编号。若后续还要调用其他函数，先保存 `int saved_errno = errno;`。 | 不是函数；非零不自动证明最近一次调用失败，成功调用也不保证清零。 |
| `strerror(int errnum)`，`<string.h>` | 将已保存的错误编号转换成描述字符串，可交给 `fprintf(stderr, "%s", ...)`。 | 返回指向错误描述字符串的指针；不要修改或释放它，不依赖它在后续调用后保持不变。 |
| `perror(const char *prefix)`，`<stdio.h>` | 依据当前 `errno` 向 `stderr` 输出提示和错误描述；应在确认失败后及时调用。 | 返回 `void`；`prefix` 用于说明失败的是哪一步。只有相关接口保证设置 `errno` 时，描述才能准确解释该次失败。 |
| `return status`（在 `main` 中） | 结束 `main`，把退出状态交给执行环境；`status` 可取 `EXIT_SUCCESS` 或 `EXIT_FAILURE`。 | 从 `main` 返回会触发正常终止流程；普通函数中的 `return` 只返回调用者。 |
| `exit(int status)`，`<stdlib.h>` | 从当前调用位置开始正常终止整个程序；用状态参数报告结果。 | 不返回；调用已注册的退出函数，然后处理流的正常终止。不会替你执行尚未到达的普通清理语句。 |
| `atexit(void (*func)(void))`，`<stdlib.h>` | `func` 是指向“无参数、无返回值”函数的指针，例如 `atexit(finish_message)` 注册函数地址。 | 注册成功返回 `0`，失败返回非零；正常终止时才由运行库调用函数。多次注册时逆序执行。 |
| `abort(void)`，`<stdlib.h>` | 程序遇到严重异常时异常终止。 | 不返回；不会执行 `atexit` 注册的函数，不应依赖它刷新输出或完成清理。 |

`EXIT_SUCCESS` 和 `EXIT_FAILURE` 是 `<stdlib.h>` 中的状态宏，代表成功与失败，不要假设失败宏的数值必须等于某个固定整数。`NULL` 表示空指针，可与 `fopen` 的失败结果比较；不能解引用。`stderr` 是标准错误流，用它输出诊断不会自动让程序返回失败状态，仍需正确设置退出状态。

#### 流和文件生命周期：15.3～15.7

`<stdio.h>` 中的 `FILE` 是流控制类型，应用程序只通过接口使用它。`stdin`、`stdout`、`stderr` 分别是标准输入、标准输出和标准错误流，通常由运行环境在程序开始前建立。`FILE *fp` 是指向流控制对象的指针；它不是文件内容数组，也不能用 `sizeof *fp` 推出文件大小。调用者成功 `fopen` 后通常负责 `fclose`，传给辅助函数使用时应说明所有权是否转移。

| 函数 | 参数与使用方法 | 返回结果、失败处理 |
| --- | --- | --- |
| `fopen(const char *path, const char *mode)` | `path` 是以 `\0` 结尾的文件名；`mode` 是如 `"r"`、`"wb"` 的模式字符串。 | 成功返回新的 `FILE *`；失败返回 `NULL`。写入模式中的 `w` 会截断已有内容。 |
| `freopen(const char *path, const char *mode, FILE *stream)` | 关闭并重新关联已有流，例如把 `stdout` 重定向到文件；成功后使用返回的流指针。 | 成功返回流指针，失败返回 `NULL`；原流也可能已被关闭，不能假设它仍可用。 |
| `fclose(FILE *stream)` | 对成功打开的流调用一次；调用后旧指针不可再用于读写或再次关闭。 | 成功返回 `0`，失败返回 `EOF`。输出缓冲可能在关闭时才写出，所以写入成功后仍需检查关闭结果。 |

模式符号含义：`r` 读取且文件须存在；`w` 创建或截断；`a` 追加；`+` 使流可读可写；`b` 请求二进制模式。`"r+"` 不截断且要求文件存在，`"w+"` 会截断，`"a+"` 的写入保持追加语义。文本模式可能有换行转换；二进制模式适合字节必须保持原样的数据。

更新流（带 `+`）要遵守读写切换规则：输出后接输入，先成功调用 `fflush` 或定位函数；输入后接输出，先调用定位函数，除非输入已遇到文件末尾。即使调用 `fflush`，若随后要从开头读取，仍须定位到开头。

#### 字符与未格式化行 I/O：15.8～15.9

| 函数 | 参数与使用方法 | 返回结果、边界 |
| --- | --- | --- |
| `fgetc(FILE *stream)` / `getc(FILE *stream)` | 从指定流取得下一个字符，并推进位置；用 `int` 保存结果。 | 成功返回作为 `unsigned char` 转成 `int` 的字符值；到末尾或出错返回 `EOF`，随后检查 `feof` / `ferror`。`getc` 可以实现为宏，不要给流实参写带副作用的表达式。 |
| `getchar(void)` | 从 `stdin` 读取，通常相当于 `getc(stdin)`。 | 返回规则同 `fgetc`。 |
| `fputc(int ch, FILE *stream)` / `putc(int ch, FILE *stream)` | 向指定流写一个字符；只写 `ch` 转为 `unsigned char` 后的值。 | 成功返回写入字符值，失败返回 `EOF`；`putc` 可能是宏。 |
| `putchar(int ch)` | 向 `stdout` 写一个字符。 | 返回规则同 `fputc`。 |
| `ungetc(int ch, FILE *stream)` | 将一个非 `EOF` 字符退回输入流，供下次读取；标准至少保证连续退回一个字符的能力。 | 成功返回退回的字符值，失败返回 `EOF`；它不是修改外部文件内容。 |
| `fgets(char *buffer, int n, FILE *stream)` | `buffer` 指向至少有 `n` 个 `char` 的数组；最多读 `n-1` 个字符，成功时补 `\0`，读到的换行会保留。示例用 `fgets(line, (int)sizeof line, stream)`，前提是容量可用 `int` 表示。 | 成功返回 `buffer`；在未取得字符前遇到结束或错误返回 `NULL`。长行会分段，数组内没有换行不一定表示出错。`n` 应大于 1。 |
| `fputs(const char *text, FILE *stream)` | 写入以 `\0` 结尾的字符串，不写结尾符，也不自动加换行。 | 成功返回非负值，失败返回 `EOF`。 |
| `puts(const char *text)` | 向 `stdout` 写字符串，并另写换行。 | 成功返回非负值，失败返回 `EOF`。 |

处理 `fgets` 的结果时，可用 `strchr(line, '\n')` 判断本次是否读到换行；没有换行可能是长行的第一段，也可能是文件最后一行。不能据此单独断定文件结束。`gets` 无容量参数，已从 C 标准移除。

#### 格式化 I/O：15.10

| 函数 | 数据来源或去向 | 返回结果与重点 |
| --- | --- | --- |
| `printf(const char *format, ...)` | 将值按格式写入 `stdout`。 | 成功返回输出字符数，失败返回负值。 |
| `fprintf(FILE *stream, const char *format, ...)` | 将值按格式写入指定流，常用 `fprintf(stderr, ...)` 报错。 | 成功返回输出字符数，失败返回负值。 |
| `snprintf(char *buffer, size_t size, const char *format, ...)` | 向容量为 `size` 字节的数组格式化写入；当 `size > 0` 时留位置写 `\0`。 | 成功返回若空间足够本应写出的字符数，不含 `\0`；负值表示编码错误。返回值非负且达到 `size` 表示截断。 |
| `scanf(const char *format, ...)` | 从 `stdin` 按格式转换并写入接收对象。 | 返回成功赋值的项目数；输入失败发生在首次成功赋值前时返回 `EOF`。 |
| `fscanf(FILE *stream, const char *format, ...)` | 从指定流按格式转换。 | 返回规则同 `scanf`；返回 `0` 表示没有成功赋值，不能当成文件结束。 |
| `sscanf(const char *text, const char *format, ...)` | 从已有字符串解析，常与 `fgets` 配合。 | 返回规则同 `scanf`。 |

格式符号示例：`%d` 在 `printf` 中接收 `int` 值，在 `scanf` 中对应 `int *`；`%f` 在 `printf` 中接收经默认提升后的 `double`，在 `scanf` 中对应 `float *`；`%lf` 在 `scanf` 中对应 `double *`。`%s` 在输入时需设置最大字段宽度，例如 `char name[16]` 配 `"%15s"`，留 1 个字符给 `\0`。输入函数不会自动以整行为界；期望两项时必须检查返回值等于 `2` 后再使用两个输出对象。

```c
#include <stdio.h>

/* line 保存整行输入，count 和 voltage 是解析后的目标对象。 */
char line[64];
int count;
double voltage;

/* 先获取一段文本，再检查两个字段是否都成功赋值。 */
if (fgets(line, (int)sizeof line, stdin) != NULL)
{
    int converted = sscanf(line, "%d %lf", &count, &voltage);
    if (converted == 2)
    {
        printf("Count: %d, voltage: %.2f\n", count, voltage);
    }
    else
    {
        fprintf(stderr, "Error: invalid input.\n");
    }
}
```

#### 二进制 I/O 与数量单位：15.11

`fread(void *ptr, size_t size, size_t count, FILE *stream)` 从流读取最多 `count` 个元素，每个 `size` 字节，并写入 `ptr` 指向的缓冲区。`fwrite(const void *ptr, size_t size, size_t count, FILE *stream)` 将同样数量的元素写入流。两者返回值都是完整处理的**元素数**；要比较 `returned == count`，不能拿总字节数比较。`ptr` 的容量至少应为 `size * count` 字节，计算时须防止乘法溢出；通常用数组自身的元素大小和容量传参。

```c
/* samples 是 unsigned char 数组，每个元素占 sizeof samples[0] 字节。
   stream 是已用 "wb" 打开、由调用方负责 fclose 的输出流。 */
unsigned char samples[] = {0xA5, 0x01, 0x0A, 0xFF};
size_t count = sizeof samples / sizeof samples[0];
size_t written = fwrite(samples, sizeof samples[0], count, stream);

/* written 的单位是元素；未写满时记录失败，仍要检查后续 fclose。 */
if (written != count)
{
    fprintf(stderr, "Error: incomplete write.\n");
}
```

`fread` 少于要求数量时，查看 `ferror` 和 `feof`。已读满的元素可以处理，未完整读入的尾部对象不可当作有效值。原样写入结构体适合同一环境内受控的临时用途，不应直接当作跨平台通信格式。

#### 刷新、定位与缓冲：15.12～15.13

| 函数或符号 | 参数与使用方法 | 返回结果、限制 |
| --- | --- | --- |
| `fflush(FILE *stream)` | 对输出流写出库缓冲中尚待输出的数据；交互式提示后可用 `fflush(stdout)`。 | 成功返回 `0`，失败返回 `EOF`；不保证物理持久化。标准 C 不用 `fflush(stdin)` 清输入。 |
| `fseek(FILE *stream, long offset, int origin)` | 以 `origin` 为基准移动位置。`SEEK_SET` 为开头，`SEEK_CUR` 为当前位置，`SEEK_END` 为末尾。 | 成功返回 `0`，失败返回非零。文本流只可用标准允许的位置用法，不要用读过的字符数计算偏移。 |
| `ftell(FILE *stream)` | 取得可用于后续定位的当前位置值。 | 失败返回 `-1L`。文本流返回值应作为位置凭证，不一定是字节编号。 |
| `rewind(FILE *stream)` | 回到开头，同时清除结束与错误指示器。 | 返回 `void`，不能直接通过返回值检查是否定位成功。 |
| `fgetpos(FILE *stream, fpos_t *pos)` | 将当前位置保存到 `*pos`；`pos` 指向调用方提供的 `fpos_t` 对象。 | 成功返回 `0`，失败返回非零。不要解析 `fpos_t` 的内部内容。 |
| `fsetpos(FILE *stream, const fpos_t *pos)` | 使用此前保存的位置恢复流的位置。 | 成功返回 `0`，失败返回非零。 |
| `setvbuf(FILE *stream, char *buffer, int mode, size_t size)` | 在打开流后、其他操作前选择缓冲方式。`buffer == NULL` 时由库管理缓冲；自供缓冲区须保持有效。 | 成功返回 `0`，失败返回非零。`mode` 取 `_IOFBF`、`_IOLBF`、`_IONBF`。 |
| `setbuf(FILE *stream, char *buffer)` | 简化设置：空指针请求无缓冲，非空指针提供大小至少为 `BUFSIZ` 的数组。 | 返回 `void`，无法直接报告失败；自供数组必须存活到流结束。 |

`_IOFBF` 表示全缓冲，`_IOLBF` 表示行缓冲，`_IONBF` 表示无缓冲；它们是传给 `setvbuf` 的模式宏。`BUFSIZ` 是 `<stdio.h>` 给出的缓冲区大小宏，不代表所有流必然采用该大小。`fpos_t` 是保存文件位置的类型，不要当成普通整数操作。

#### 结束、错误、临时文件与路径操作：15.14～15.16

| 函数 | 参数与使用方法 | 返回结果、后续责任 |
| --- | --- | --- |
| `feof(FILE *stream)` | 查询该流的文件结束指示器；读取尝试遇到末尾后才可能置位。 | 非零表示已置位，`0` 表示未置位；不能预测下一次读取。 |
| `ferror(FILE *stream)` | 查询该流的错误指示器，读取返回 `EOF` 或发生短读后尤其要检查。 | 非零表示已置位，`0` 表示未置位。 |
| `clearerr(FILE *stream)` | 清除文件结束与错误指示器，准备在原因处理后重试。 | 返回 `void`；不修复引发错误的设备或文件问题。 |
| `tmpfile(void)` | 创建可读写的二进制临时流，适合保存中间数据。 | 成功返回 `FILE *`，失败返回 `NULL`；成功后负责 `fclose`，文件关闭时自动删除。 |
| `tmpnam(char *buffer)` | 生成临时文件名；`buffer == NULL` 时返回内部静态数组，非空时目标数组至少容纳 `L_tmpnam` 个字符。 | 成功返回名称指针，失败返回 `NULL`；**不创建文件**，之后创建存在名称竞争。 |
| `remove(const char *path)` | 删除 `path` 指定的文件。 | 成功返回 `0`，失败返回非零；调用前核对目标路径。 |
| `rename(const char *old_path, const char *new_path)` | 将旧路径重命名为新路径。 | 成功返回 `0`，失败返回非零；目标已存在或文件仍打开时的处理有平台差异。 |

`L_tmpnam` 是 `<stdio.h>` 给出的名字数组最小容量宏，只与 `tmpnam` 接口有关。`tmpnam` 生成名字与随后真正创建文件是两步，不能把返回名称当成已经获得了文件。处理普通中间结果时优先使用 `tmpfile`。

## 练习记录

### Q_1：逐字符复制标准输入到标准输出

- 文件：[Q_1/main.c](../15_input_output/Q_1/main.c)。题目要求将标准输入中的字符逐个复制到标准输出。
- 原实现用 `int ch` 接收 `getchar()`，以 `EOF` 控制循环，再用 `putchar(ch)` 输出；核心思路正确。`int` 能同时表示字符值和额外的 `EOF`。
- 补充了 `putchar` 的失败检查；循环结束后用 `ferror(stdin)` 区分正常输入结束和读取错误；最后用 `fflush(stdout)` 检查缓冲输出是否写出。出错向 `stderr` 报告并返回 `EXIT_FAILURE`，成功返回 `EXIT_SUCCESS`。
- 用 MinGW64 GCC 的 `-Wall -Wextra -Wpedantic -Wshadow -Wconversion` 编译，无警告。实际运行测试空输入、单字符 `A`、两行文本及 4096 个字符，逐项比较输出和退出状态，均通过。多行测试在本机 Windows 文本流中观察到换行输出为 CRLF，这是文本模式的平台行为。

### Q_2：逐行复制标准输入到标准输出

- 文件：[Q_2/main.c](../15_input_output/Q_2/main.c)。题目允许假定每行内容字符不超过 80 个，不含结尾换行符。
- 原实现使用已移除且没有容量参数的 `gets`，并声明 `int buf[81]`；`gets` 无法防止越界，`int` 数组也不适合作为字符字符串。`puts` 总会添加换行，不能保持末行无换行的输入形式。
- 改为 `char line[82]`：80 个内容字符、1 个可能的换行符、1 个字符串结束符。使用 `fgets` 逐行读取，用 `fputs` 原样输出，并检查输出、输入和刷新失败。数组容量以枚举常量表达，便于看清大小计算。
- MinGW64 GCC 使用 `-Wall -Wextra -Wpedantic -Wshadow -Wconversion` 编译，无警告。实测空输入、空行、多行、末行无换行、80 字符且有换行、80 字符且无换行，逐项比较输出与退出状态，均通过。换行在本机 Windows 文本流中输出为 CRLF；测试时按此平台转换比较。

### Q_3：不限行长，逐行分段复制

- 文件：[Q_3/main.c](../15_input_output/Q_3/main.c)。题目延续 Q_2 的逐行复制，但取消每行 80 字符的上限；较长的一行允许分段处理。
- 原文件只有空 `main`，尚未实现。现使用 81 个 `char` 的固定缓冲区，每次 `fgets` 最多取 80 个字符并补 `\0`。外层循环处理各行，内层循环反复读写同一行的各段；`strchr(buffer, '\n')` 找到换行后才进入下一行。读到无换行的末行后，下一次 `fgets` 返回 `NULL`，借助 `ferror(stdin)` 区分文件结束与读取错误。
- `fputs` 不额外添加换行，因此输入末行没有换行时输出也没有；检查写入结果与结束时 `fflush(stdout)` 的结果。固定缓冲区使内存占用不随行长增长。
- MinGW64 GCC 使用 `-Wall -Wextra -Wpedantic -Wshadow -Wconversion` 编译，无警告。实测空输入、空行、79、80、81、160 字符加换行、161 字符无换行，以及混合多行与 240 字符长行，逐项比较输出和退出状态，均通过。本机 Windows 文本流把输出换行表示为 CRLF，测试按此平台行为比较。

### Q_4：读取文件名并逐行分段复制文件

- 文件：[Q_4/main.c](../15_input_output/Q_4/main.c)。从标准输入依次读入输入文件名和输出文件名，分别用 `"r"`、`"w"` 打开，再按 Q_3 的固定缓冲区方式逐行分段复制。目录中原有文件名 `mian.c` 已改成 `main.c`，便于按本仓库约定编译。
- `read_file_name` 用 `fgets` 读取文件名，去掉末尾换行，并拒绝空名称或超出数组容量的名称；提示写到 `stdout` 后先 `fflush`，使等待输入前可以看到提示。文件名中的空格会保留。
- 在打开输出文件前拒绝完全相同的文件名字符串，避免 `"w"` 直接截断输入文件。更复杂的同一文件别名依赖平台文件身份检查，这个标准 C 练习未处理；使用时仍要确认两个路径指向不同文件。
- 成功打开的输入、输出流由 `main` 负责关闭；第二个文件打开失败时关闭第一个。复制时检查 `fgets`、`fputs`，结束时检查两个 `fclose`，以免输出缓冲写入失败被当作成功。
- MinGW64 GCC 使用 `-Wall -Wextra -Wpedantic -Wshadow -Wconversion` 编译，无警告。实测空文件、含空行的多行文件、241 字符长行、161 字符无换行末行，复制内容一致；输入文件不存在、两个文件名相同、输出路径无法打开、空文件名和超长文件名时均返回失败状态，且同名测试保留原内容。测试文件创建在 Q_4 内并已清理。本机文本流输出换行按 CRLF 比较。

用户已确认第十五章学习完成；本笔记记录了 Q_1～Q_4 的检查、修正与测试，其余练习如有需要可后续补充。
