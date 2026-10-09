#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    /*
     * 缓冲区每次最多容纳 80 个输入字符，另外 1 个位置留给 '\0'。
     * 它只保存当前行的一段，因此整行长度不受这个数组容量限制。
     */
    enum { CHUNK_CHARS = 80, BUFFER_SIZE = CHUNK_CHARS + 1 };
    char buffer[BUFFER_SIZE];

    /* input_done 初始为假，读到标准输入末尾时改为真。 */
    int input_done = 0;
    while (!input_done)
    {
        /* line_done 表示本行的换行符已经读到并写出。 */
        int line_done = 0;
        while (!line_done)
        {
            /* fgets 最多读 80 个字符，成功后自动补 '\0'。
               长行会留在输入流中，下一次继续读取同一行。 */
            if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)
            {
                /* NULL 可能表示读取错误，也可能表示正常到达输入末尾。 */
                if (ferror(stdin))
                {
                    fprintf(stderr, "Error: cannot read from standard input.\n");
                    return EXIT_FAILURE;
                }

                input_done = 1;
                break;
            }

            /* fputs 不另加换行；每段读到什么文本，就写出什么文本。 */
            if (fputs(buffer, stdout) == EOF)
            {
                fprintf(stderr, "Error: cannot write to standard output.\n");
                return EXIT_FAILURE;
            }

            /* strchr 找到换行符说明本行已处理完，可以开始下一行。
               若本段没有换行，则继续读取同一行的下一段。 */
            if (strchr(buffer, '\n') != NULL)
            {
                line_done = 1;
            }
        }
    }

    /* 标准输出可能有待写出的缓冲数据，结束前检查刷新结果。 */
    if (fflush(stdout) == EOF)
    {
        fprintf(stderr, "Error: cannot flush standard output.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
