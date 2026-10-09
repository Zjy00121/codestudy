#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*
 * 题目: 统计单词 "the" 的出现次数
 *
 * 从标准输入扫描文本，统计单词 "the"（小写、完整单词）出现的次数。
 *
 * 规则:
 *   - 区分大小写: "The" / "THE" 不计数，只有 "the" 计数
 *   - "the" 必须是完整单词: "there" / "thee" / "then" 不计数
 *   - 单词由一个或多个空白字符分隔
 *   - 输入行长度不超过 100 字符
 */

int count_the(char const *string);

int main(void)
{
    char line[128];      /* 足够容纳 100 字符的行 + 换行 + '\0' */
    int  total = 0;

    /* 逐行读取标准输入，直到 EOF */
    while (fgets(line, sizeof(line), stdin) != NULL) {
        total += count_the(line);
    }

    /* 输出总计数 */
    printf("%d\n", total);

    return 0;
}

/*
 * count_the: 统计一行字符串中单词 "the" 的出现次数
 *
 * 参数:
 *   string — 待扫描的字符串（只读）
 *
 * 返回:
 *   单词 "the"（小写、完整单词）出现的次数
 *
 * 算法:
 *   循环定位每个单词:
 *     1. 跳过空白，定位到单词开头
 *     2. 判断该单词是否恰好是 "the"（前3字符 + 后面是空白或结尾）
 *     3. 跳过整个单词，继续下一个
 */
int count_the(char const *string)
{
    int count = 0;
    char const *p = string;

    while (*p != '\0') {

        /* 跳过空白，定位到下一个单词开头 */
        while (isspace((unsigned char)*p))
            p++;

        if (*p == '\0')
            break;   /* 字符串剩余全是空白，结束 */

        /* 判断当前单词是否为 "the":
         *   前 3 个字符是 t-h-e，且第 4 个字符是空白或 '\0'（完整单词） */
        if (p[0] == 't' && p[1] == 'h' && p[2] == 'e' &&
            (p[3] == '\0' || isspace((unsigned char)p[3]))) {
            count++;
        }

        /* 跳过整个单词，直到遇到空白或结尾 */
        while (*p != '\0' && !isspace((unsigned char)*p))
            p++;
    }

    return count;
}
