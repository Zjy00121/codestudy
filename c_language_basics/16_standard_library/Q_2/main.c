#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * 功能：模拟掷骰子，成功返回 1～6；获取时间失败时返回 0。
 * 首次成功调用时用当前时间设置种子，后续调用继续使用该序列。
 * 拒绝采样消除取余偏差；各点等概率仍以 rand 原始输出均匀为前提。
 * 本函数使用共享的 rand 状态，按单线程练习使用。
 */
int roll_dice(void)
{
    /* 静态局部变量在函数返回后仍保留值，程序启动时初始化为 0。 */
    static int initialized = 0;

    if (!initialized)
    {
        /* 先检查获取时间成功，再转换为 srand 所需的种子类型。 */
        time_t now = time(NULL);
        if (now == (time_t)-1)
        {
            /* 不标记初始化成功，调用者处理错误；后续调用可重试。 */
            return 0;
        }

        /* 时间转换不保证种子唯一，同一秒启动可能产生相同序列。 */
        srand((unsigned int)now);
        initialized = 1;
    }

    /*
     * 原始结果范围是 0～RAND_MAX，共 RAND_MAX + 1 个值。
     * 先转换为 unsigned long 再加 1，避免 int 加法溢出。
     */
    unsigned long span = (unsigned long)RAND_MAX + 1UL;

    /*
     * 接受 [0, limit)，其大小可被 6 整除。
     * 丢弃尾部不足六个一组的值，使每种余数对应相同数量的原值。
     */
    unsigned long limit = span - span % 6UL;
    unsigned long sample;

    do
    {
        sample = (unsigned long)rand();
    }
    while (sample >= limit);

    /* 接受的原值取余得到 0～5，加 1 后返回骰子点数 1～6。 */
    return (int)(sample % 6UL) + 1;
}

int main(void)
{
    int dice = roll_dice();
    if (dice == 0)
    {
        fprintf(stderr, "Cannot obtain current time.\n");
        return EXIT_FAILURE;
    }

    printf("%d\n", dice);
    return EXIT_SUCCESS;
}
