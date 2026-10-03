#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    /*
     * ch 必须是 int：getchar 成功时返回字符值，
     * 到达输入末尾或读取出错时返回额外的 EOF 值。
     */
    int ch;

    /* 每次先从标准输入读取一个字符，成功后才复制到标准输出。 */
    while ((ch = getchar()) != EOF)
    {
        /* putchar 失败时返回 EOF，不能继续假装复制成功。 */
        if (putchar(ch) == EOF)
        {
            fprintf(stderr, "Error: cannot write to standard output.\n");
            return EXIT_FAILURE;
        }
    }

    /* getchar 返回 EOF 也可能是读取错误，需检查标准输入的状态。 */
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
