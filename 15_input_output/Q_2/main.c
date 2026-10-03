#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    /*
     * 每行最多有 80 个内容字符，另需 1 个位置保存可能出现的换行符，
     * 再留 1 个位置给字符串结束符 '\0'，所以数组容量是 82 个 char。
     */
    enum { MAX_LINE_CHARS = 80, LINE_BUFFER_SIZE = MAX_LINE_CHARS + 2 };
    char line[LINE_BUFFER_SIZE];

    /* fgets 每次最多读 81 个字符，读到换行时也将其存入数组。 */
    while (fgets(line, LINE_BUFFER_SIZE, stdin) != NULL)
    {
        /* fputs 原样写出本次读到的字符串，不额外添加换行符。
           因此最后一行即使没有换行，复制结果也与输入一致。 */
        if (fputs(line, stdout) == EOF)
        {
            fprintf(stderr, "Error: cannot write to standard output.\n");
            return EXIT_FAILURE;
        }
    }

    /* fgets 返回 NULL 可能表示文件结束，也可能表示读取错误。 */
    if (ferror(stdin))
    {
        fprintf(stderr, "Error: cannot read from standard input.\n");
        return EXIT_FAILURE;
    }

    /* 输出可能还在缓冲区中；显式刷新并检查最终写出结果。 */
    if (fflush(stdout) == EOF)
    {
        fprintf(stderr, "Error: cannot flush standard output.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
