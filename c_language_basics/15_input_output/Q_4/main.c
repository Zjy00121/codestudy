#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum
{
    NAME_CAPACITY = 256, /* 文件名数组的容量，包含末尾的 '\0'。 */
    CHUNK_CHARS = 80,    /* 每次最多读取当前行中的 80 个字符。 */
    BUFFER_SIZE = CHUNK_CHARS + 1 /* 另留 1 个位置给 '\0'。 */
};

/*
 * prompt：显示给用户的英文提示；name：调用方提供的文件名数组。
 * 成功时返回 1，并在 name 中留下不含换行的字符串；失败返回 0。
 * 数组由 main 持有，本函数不分配内存，也不负责释放。
 */
static int read_file_name(const char *prompt, char name[NAME_CAPACITY])
{
    /* 提示必须先刷新，用户才能在等待输入前看到它。 */
    if (fputs(prompt, stdout) == EOF || fflush(stdout) == EOF)
    {
        fprintf(stderr, "Error: cannot show file prompt.\n");
        return 0;
    }

    /* fgets 最多读取 NAME_CAPACITY - 1 个字符，并补 '\0'。 */
    if (fgets(name, NAME_CAPACITY, stdin) == NULL)
    {
        fprintf(stderr, "Error: cannot read file name.\n");
        return 0;
    }

    char *newline = strchr(name, '\n');
    if (newline != NULL)
    {
        /* 文件名不包含用户输入时按下 Enter 产生的换行。 */
        *newline = '\0';
    }
    else if (strlen(name) == NAME_CAPACITY - 1)
    {
        /* 缓冲区已满，不能把被截断的路径误当成完整文件名。 */
        fprintf(stderr, "Error: file name is too long.\n");
        return 0;
    }

    if (name[0] == '\0')
    {
        fprintf(stderr, "Error: file name is empty.\n");
        return 0;
    }

    return 1;
}

int main(void)
{
    /* 两个数组分别保存输入、输出路径；路径可以包含空格。 */
    char input_name[NAME_CAPACITY];
    char output_name[NAME_CAPACITY];

    if (!read_file_name("Input file: ", input_name) ||
        !read_file_name("Output file: ", output_name))
    {
        return EXIT_FAILURE;
    }

    /* "w" 会截断原文件；相同的路径字符串不能用作源和目标。 */
    if (strcmp(input_name, output_name) == 0)
    {
        fprintf(stderr, "Error: input and output file names are the same.\n");
        return EXIT_FAILURE;
    }

    FILE *input = fopen(input_name, "r");
    if (input == NULL)
    {
        fprintf(stderr, "Error: cannot open input file.\n");
        return EXIT_FAILURE;
    }

    FILE *output = fopen(output_name, "w");
    if (output == NULL)
    {
        fprintf(stderr, "Error: cannot open output file.\n");
        /* 输入流已成功打开，因此即使第二次打开失败也要关闭它。 */
        if (fclose(input) == EOF)
        {
            fprintf(stderr, "Error: cannot close input file.\n");
        }
        return EXIT_FAILURE;
    }

    /* buffer 只保存当前行的一段，内存占用不随行长增长。 */
    char buffer[BUFFER_SIZE];
    int status = EXIT_SUCCESS;

    int input_done = 0;
    while (!input_done && status == EXIT_SUCCESS)
    {
        int line_done = 0;
        while (!line_done)
        {
            /* fgets 最多读取当前行的 80 个字符，再补 '\0'。 */
            if (fgets(buffer, BUFFER_SIZE, input) == NULL)
            {
                if (ferror(input))
                {
                    fprintf(stderr, "Error: cannot read input file.\n");
                    status = EXIT_FAILURE;
                }

                input_done = 1;
                break;
            }

            /* fputs 不额外加换行，长行逐段写出，末行无换行也保留。 */
            if (fputs(buffer, output) == EOF)
            {
                fprintf(stderr, "Error: cannot write output file.\n");
                status = EXIT_FAILURE;
                break;
            }

            /* 读到换行后才处理下一行；否则继续读取当前行的下一段。 */
            if (strchr(buffer, '\n') != NULL)
            {
                line_done = 1;
            }
        }
    }

    /* 输出流关闭时可能才写出缓冲数据，因此检查关闭结果。
       两个成功打开的流都由 main 负责关闭，即使复制失败也一样。 */
    if (fclose(output) == EOF)
    {
        fprintf(stderr, "Error: cannot close output file.\n");
        status = EXIT_FAILURE;
    }

    if (fclose(input) == EOF)
    {
        fprintf(stderr, "Error: cannot close input file.\n");
        status = EXIT_FAILURE;
    }

    return status;
}
