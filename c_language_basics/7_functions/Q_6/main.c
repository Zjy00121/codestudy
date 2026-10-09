#include <stdio.h>

/*
 * 题目: 数字转英文单词 (支票打印)
 *
 * 函数原型:
 *   void written_amount(unsigned int amount, char *buffer);
 *
 * 规则:
 *   - 将金额转换为英文大写单词，存于 buffer 中
 *   - 调用方保证 buffer 足够大
 *   - 0 输出 "ZERO"
 *   - 采用标准美式英语（不加 AND，单词间用空格分隔）
 *   - 1200 统一为 "ONE THOUSAND TWO HUNDRED" 形式
 *
 * 示例:
 *   16312 → "SIXTEEN THOUSAND THREE HUNDRED TWELVE"
 *   1200  → "ONE THOUSAND TWO HUNDRED"
 *   0     → "ZERO"
 */

#define BUF_SIZE 512

/* ===== 单词表 — 用 static 隐藏，只在文件内可见 ===== */

/* 个位数 0-9 */
static char *ones[] = {
    "ZERO", "ONE", "TWO", "THREE", "FOUR",
    "FIVE", "SIX", "SEVEN", "EIGHT", "NINE"
};

/* 10-19 有专有名字 */
static char *teens[] = {
    "TEN", "ELEVEN", "TWELVE", "THIRTEEN", "FOURTEEN",
    "FIFTEEN", "SIXTEEN", "SEVENTEEN", "EIGHTEEN", "NINETEEN"
};

/* 十位数 20-90 */
static char *tens[] = {
    "", "", "TWENTY", "THIRTY", "FORTY",
    "FIFTY", "SIXTY", "SEVENTY", "EIGHTY", "NINETY"
};

/* 千/百万/十亿 对应的组名，下标 1=千, 2=百万, 3=十亿 */
static char *groups[] = {
    "", "THOUSAND", "MILLION", "BILLION"
};

/* ===== 内部辅助函数 ===== */

/* 将 0-999 转为英文单词写入 buf，返回写入后 '\0' 的位置 */
static char *write_hundreds(char *buf, unsigned int n);

/* 向 buf 追加一个单词并自动处理前导空格，返回新 '\0' 位置 */
static char *write_word(char *buf, char *word, int is_first);

/* ===== 函数声明 ===== */
void written_amount(unsigned int amount, char *buffer);

/* ===== 测试主函数 ===== */
int main(void)
{
    char buf[BUF_SIZE];

    written_amount(0, buf);
    printf("Test  1: %-10u -> \"%s\"\n", 0u, buf);

    written_amount(5, buf);
    printf("Test  2: %-10u -> \"%s\"\n", 5u, buf);

    written_amount(12, buf);
    printf("Test  3: %-10u -> \"%s\"\n", 12u, buf);

    written_amount(19, buf);
    printf("Test  4: %-10u -> \"%s\"\n", 19u, buf);

    written_amount(42, buf);
    printf("Test  5: %-10u -> \"%s\"\n", 42u, buf);

    written_amount(100, buf);
    printf("Test  6: %-10u -> \"%s\"\n", 100u, buf);

    written_amount(312, buf);
    printf("Test  7: %-10u -> \"%s\"\n", 312u, buf);

    written_amount(1200, buf);
    printf("Test  8: %-10u -> \"%s\"\n", 1200u, buf);

    written_amount(16000, buf);
    printf("Test  9: %-10u -> \"%s\"\n", 16000u, buf);

    written_amount(16312, buf);
    printf("Test 10: %-10u -> \"%s\"\n", 16312u, buf);

    written_amount(123456, buf);
    printf("Test 11: %-10u -> \"%s\"\n", 123456u, buf);

    written_amount(1000000, buf);
    printf("Test 12: %-10u -> \"%s\"\n", 1000000u, buf);

    written_amount(2000001, buf);
    printf("Test 13: %-10u -> \"%s\"\n", 2000001u, buf);

    written_amount(1234567890, buf);
    printf("Test 14: %-10u -> \"%s\"\n", 1234567890u, buf);

    written_amount(4294967295u, buf);
    printf("Test 15: %-10u -> \"%s\"\n", 4294967295u, buf);

    return 0;
}

/*
 * written_amount: 将无符号整数转为英文大写单词
 *
 * 参数:
 *   amount — 待转换的金额 (unsigned int)
 *   buffer — 调用方提供的输出缓冲区（保证足够大）
 *
 * 算法:
 *   从高到低按千进制分组: 十亿 → 百万 → 千 → 个位
 *   每组 0-999 调用 write_hundreds 转单词，
 *   然后拼接组名 (BILLION / MILLION / THOUSAND)。
 *
 * 分组处理顺序 (从高到低):
 *   group=3: 十亿 (1,000,000,000)
 *   group=2: 百万  (1,000,000)
 *   group=1: 千    (1,000)
 *   group=0: 个位  (1)
 */
void written_amount(unsigned int amount, char *buffer)
{
    unsigned int divisor;    /* 当前分组除数 */
    int          group;      /* 当前组索引: 3=十亿, 2=百万, 1=千, 0=个位 */
    int          first_word; /* 是否第一个单词（控制空格） */
    char        *p;          /* buffer 写入游标 */
    char         chunk_buf[128];  /* 临时存放单组 0-999 的单词 */
    char        *chunk_end;       /* chunk_buf 的 '\0' 位置 */

    /* 零 — 特殊情况直接返回 */
    if (amount == 0) {
        char *s = "ZERO";
        p = buffer;
        while ((*p++ = *s++) != '\0');
        return;
    }

    p          = buffer;
    *p         = '\0';
    first_word = 1;
    divisor    = 1000000000u;     /* 从十亿开始 */
    group      = 3;

    while (divisor >= 1) {
        unsigned int chunk = amount / divisor;
        amount %= divisor;

        if (chunk > 0) {
            /* 将 0-999 的数字转为单词 */
            chunk_end = write_hundreds(chunk_buf, chunk);
            *chunk_end = '\0';

            /* 拼接到主 buffer（处理空格） */
            if (!first_word)
                *p++ = ' ';

            {
                char *s = chunk_buf;
                while ((*p = *s++) != '\0')
                    p++;
            }

            /* 拼接组名 */
            if (groups[group][0] != '\0') {
                *p++ = ' ';
                char *g = groups[group];
                while ((*p = *g++) != '\0')
                    p++;
            }

            first_word = 0;
        }

        divisor /= 1000;
        group--;
    }

    *p = '\0';
}

/*
 * write_hundreds: 将 0-999 转为英文单词
 *
 * 分解:
 *   百位: N HUNDRED
 *   十位: >=20 用 tens 表 + ones 表
 *         10-19 用 teens 专有名字
 *         1-9  用 ones 表
 *         0    不输出
 *
 * 参数: buf — 写入目标, n — 待转换数字 (0-999)
 * 返回: buf 中 '\0' 的位置
 */
static char *write_hundreds(char *buf, unsigned int n)
{
    char *p   = buf;
    int   first = 1;
    int   h;

    h = n / 100;                  /* 百位数 */
    n = n % 100;                  /* 剩余的十位+个位 */

    if (h > 0) {
        p     = write_word(p, ones[h], first);
        p     = write_word(p, "HUNDRED", 0);
        first = 0;
    }

    if (n >= 20) {
        p     = write_word(p, tens[n / 10], first);
        first = 0;
        if (n % 10 > 0)
            p = write_word(p, ones[n % 10], first);
    }
    else if (n >= 10) {
        p     = write_word(p, teens[n - 10], first);
        first = 0;
    }
    else if (n > 0) {
        p     = write_word(p, ones[n], first);
        first = 0;
    }

    *p = '\0';
    return p;
}

/*
 * write_word: 追加单词到缓冲区，自动处理前导空格
 *
 * 参数:
 *   buf    — 写入目标（该位置当前为 '\0' 或即将被覆盖）
 *   word   — 要追加的单词
 *   is_first — 1 表示这是本段第一个单词（不加前导空格）
 *
 * 返回: 写入后 '\0' 的位置（即 buf + 单词长度 + 可能的空格）
 *
 * 示例:
 *   buf="ONE", is_first=1, word="THOUSAND"
 *   → buf="ONE THOUSAND"
 *
 *   buf="ONE THOUSAND", is_first=0, word="TWO"
 *   → buf="ONE THOUSAND TWO"
 */
static char *write_word(char *buf, char *word, int is_first)
{
    char *w;

    if (!is_first)
        *buf++ = ' ';             /* 非首个单词 → 先放空格 */

    w = word;
    while ((*buf++ = *w++) != '\0')
        ;                         /* 拷贝单词 + '\0' */

    return buf - 1;               /* 回退到 '\0' 处 */
}
