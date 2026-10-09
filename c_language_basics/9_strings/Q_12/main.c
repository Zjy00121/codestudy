#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*
 * 题目: 密钥准备 prepare_key（加密程序第 1 部分）
 *
 * 将密钥单词转换为编码字符数组：
 *   1. 全部转为大写（或小写）
 *   2. 去除重复字母，只保留第一次出现的
 *   3. 用字母表中剩余的字母填充，凑满 26 个字符
 *
 * 例如 "TRAILBLAZERS" →
 *   去重后 "TRAILBZES"，
 *   剩余字母 "CDFGHJKMNOPQUVWXY"，
 *   结果 "TRAILBZESCDFGHJKMNOPQUVWXY"。
 *
 * 返回:
 *   真 — 成功
 *   假 — key 为空 或 包含非字母字符
 */

int prepare_key(char *key);
void encrypt (char *data, char const* key);
void decrypt (char *data, char const* key);

int main(void)
{
    char key[27];
    int  ret;

    /* --- 测试1: 题目示例 --- */
    strcpy(key, "TRAILBLAZERS");
    ret = prepare_key(key);
    printf("Test 1: \"TRAILBLAZERS\"\n");
    printf("  ret=%d, key=\"%s\"\n", ret, key);
    printf("  (expect \"TRAILBZESCDFGHJKMNOPQUVWXY\")\n\n");

    /* --- 测试2: 小写输入 --- */
    strcpy(key, "hello");
    ret = prepare_key(key);
    printf("Test 2: \"hello\"\n");
    printf("  ret=%d, key=\"%s\"\n", ret, key);
    printf("  (expect \"HELOABCDFGIJKMNPQRSTUVWXYZ\")\n\n");

    /* --- 测试3: 单字母 --- */
    strcpy(key, "a");
    ret = prepare_key(key);
    printf("Test 3: \"a\"\n");
    printf("  ret=%d, key=\"%s\"\n", ret, key);
    printf("  (expect \"ABCDEFGHIJKLMNOPQRSTUVWXYZ\")\n\n");

    /* --- 测试4: 空字符串 → 假 --- */
    strcpy(key, "");
    ret = prepare_key(key);
    printf("Test 4: \"\"\n");
    printf("  ret=%d (expect 0)\n\n", ret);

    /* --- 测试5: 含数字 → 假 --- */
    strcpy(key, "abc123");
    ret = prepare_key(key);
    printf("Test 5: \"abc123\"\n");
    printf("  ret=%d (expect 0)\n\n", ret);

    /* --- 测试6: 含空格 → 假 --- */
    strcpy(key, "abc def");
    ret = prepare_key(key);
    printf("Test 6: \"abc def\"\n");
    printf("  ret=%d (expect 0)\n\n", ret);

    /* --- 测试7: 覆盖全部 26 个字母 --- */
    strcpy(key, "abcdefghijklmnopqrstuvwxyz");
    ret = prepare_key(key);
    printf("Test 7: 全字母表\n");
    printf("  ret=%d, key=\"%s\"\n", ret, key);
    printf("  (expect \"ABCDEFGHIJKLMNOPQRSTUVWXYZ\")\n\n");

    /* --- 测试8: 混合大小写 --- */
    strcpy(key, "ZeBrA");
    ret = prepare_key(key);
    printf("Test 8: \"ZeBrA\"\n");
    printf("  ret=%d, key=\"%s\"\n", ret, key);
    printf("  (expect \"ZEBRA\" + 剩余字母)\n\n");

    /* ================================================================
     * 第二部分: encrypt 加密测试
     * 密钥 "TRAILBLAZERS" → "TRAILBZESCDFGHJKMNOPQUVWXY"
     * ================================================================ */
    {
        char data[64];

        strcpy(key, "TRAILBLAZERS");
        prepare_key(key);

        /* 测试9: 题目示例，全大写 */
        strcpy(data, "ATTACKATDAWN");
        encrypt(data, key);
        printf("Test 9: encrypt \"ATTACKATDAWN\"\n");
        printf("  -> \"%s\" (expect \"TPPTADTPITVH\")\n\n", data);

        /* 测试10: 全小写，保留小写 */
        strcpy(data, "hello");
        encrypt(data, key);
        printf("Test 10: encrypt \"hello\"\n");
        printf("  -> \"%s\" (expect \"elffj\")\n\n", data);

        /* 测试11: 混合大小写 + 非字母字符 */
        strcpy(data, "Hello, World!");
        encrypt(data, key);
        printf("Test 11: encrypt \"Hello, World!\"\n");
        printf("  -> \"%s\" (expect \"Elffj, Vjnfi!\")\n\n", data);

        /* 测试12: 纯非字母字符，应保持不变 */
        strcpy(data, "123 !@#");
        encrypt(data, key);
        printf("Test 12: encrypt \"123 !@#\"\n");
        printf("  -> \"%s\" (expect \"123 !@#\")\n\n", data);

        /* 测试13: 空字符串 */
        strcpy(data, "");
        encrypt(data, key);
        printf("Test 13: encrypt \"\"\n");
        printf("  -> \"%s\" (expect \"\")\n\n", data);

        /* 测试14: 单字母大小写对比 */
        strcpy(data, "aA");
        encrypt(data, key);
        printf("Test 14: encrypt \"aA\"\n");
        printf("  -> \"%s\" (expect \"tT\")\n\n", data);
    }

    /* ================================================================
     * 第三部分: decrypt 解密测试
     * ================================================================ */
    {
        char data[64];

        strcpy(key, "TRAILBLAZERS");
        prepare_key(key);

        /* 测试15: 解密题目示例 */
        strcpy(data, "TPPTADTPITVH");
        decrypt(data, key);
        printf("Test 15: decrypt \"TPPTADTPITVH\"\n");
        printf("  -> \"%s\" (expect \"ATTACKATDAWN\")\n\n", data);

        /* 测试16: 解密小写 */
        strcpy(data, "elffj");
        decrypt(data, key);
        printf("Test 16: decrypt \"elffj\"\n");
        printf("  -> \"%s\" (expect \"hello\")\n\n", data);

        /* 测试17: 解密混合大小写 + 非字母 */
        strcpy(data, "Elffj, Vjnfi!");
        decrypt(data, key);
        printf("Test 17: decrypt \"Elffj, Vjnfi!\"\n");
        printf("  -> \"%s\" (expect \"Hello, World!\")\n\n", data);

        /* 测试18: 往返测试 — 加密再解密应还原原文 */
        strcpy(data, "The Quick Brown Fox");
        encrypt(data, key);
        decrypt(data, key);
        printf("Test 18: round-trip \"The Quick Brown Fox\"\n");
        printf("  -> \"%s\" (expect 原文)\n\n", data);

        /* 测试19: 纯非字母不变 */
        strcpy(data, "123 !@#");
        decrypt(data, key);
        printf("Test 19: decrypt \"123 !@#\"\n");
        printf("  -> \"%s\" (expect \"123 !@#\")\n", data);
    }

    return 0;
}

/*
 * prepare_key: 将密钥单词转换为 26 字符的编码数组
 *
 * 参数:
 *   key — 输入/输出。输入为密钥单词，输出为编码后的 26 字符数组。
 *         调用方保证至少有 27 字节空间。
 *
 * 返回:
 *   1 — 成功
 *   0 — key 为空或包含非字母字符
 *
 * 算法:
 *   1. 校验: 空串或含非字母 → 返回 0
 *   2. 用 used[26] 标记字母是否已出现，遍历 key:
 *        - 转大写
 *        - 未出现过则保留，已出现则跳过（去重）
 *   3. 从 A 到 Z 遍历，把未出现的字母依次填到末尾
 */
int prepare_key(char *key)
{
    int  used[26] = {0};   /* used[i]=1 表示字母 'A'+i 已出现 */
    char *read;            /* 读指针，遍历原 key */
    char *write;           /* 写指针，指向去重后的 key */
    int  i;

    /* 空串 → 失败 */
    if (*key == '\0')
        return 0;

    /* 第1步: 校验所有字符都是字母 */
    for (read = key; *read != '\0'; read++) {
        if (!isalpha((unsigned char)*read))
            return 0;   /* 含非字母字符 → 失败 */
    }

    /* 第2步: 转大写 + 去重（保留第一次出现） */
    read  = key;
    write = key;
    while (*read != '\0') {
        int idx = toupper((unsigned char)*read) - 'A';

        if (!used[idx]) {          /* 首次出现 */
            used[idx] = 1;
            *write++ = (char)('A' + idx);   /* 保留大写形式 */
        }
        /* 重复字母直接跳过，不写 */

        read++;
    }

    /* 第3步: 填充字母表中剩余（未出现）的字母 */
    for (i = 0; i < 26; i++) {
        if (!used[i]) {
            *write++ = (char)('A' + i);
        }
    }

    *write = '\0';   /* 收尾 */

    return 1;
}

/*
 * encrypt: 使用编码密钥加密 data（就地修改）
 *
 * 参数:
 *   data — 待加密的字符串（就地修改）
 *   key  — prepare_key 生成的 26 字符编码数组
 *
 * 算法:
 *   遍历 data 每个字符:
 *     - 小写字母: 用下标 (*data - 'a') 查表，结果再转小写（保留大小写）
 *     - 大写字母: 用下标 (*data - 'A') 查表（key 本身是大写）
 *     - 非字母:   不做修改
 *
 * 注意:
 *   key[0] 对应 'A'/'a' 的密文，key[1] 对应 'B'/'b'，以此类推。
 */
void encrypt(char *data, char const *key)
{
    while (*data != '\0') {
        if (*data >= 'a' && *data <= 'z') {
            /* 小写字母 → 查表 + 转小写，保留小写状态 */
            *data = tolower((unsigned char)*(key + (*data - 'a')));
        }
        else if (*data >= 'A' && *data <= 'Z') {
            /* 大写字母 → 查表，key 本身是大写，直接替换 */
            *data = *(key + (*data - 'A'));
        }
        /* 非字母字符：两个分支都不匹配，原样保留 */

        data++;
    }
}

/*
 * decrypt: 使用编码密钥解密 data（encrypt 的逆操作，就地修改）
 *
 * 参数:
 *   data — 待解密的字符串（就地修改）
 *   key  — prepare_key 生成的 26 字符编码数组
 *
 * 算法:
 *   遍历 data 每个字符:
 *     - 小写字母: 在 key 中线性查找该字符的位置 i（忽略大小写），
 *                 则原文字母为 'a' + i
 *     - 大写字母: 在 key 中线性查找该字符的位置 i，
 *                 则原文字母为 'A' + i
 *     - 非字母:   不做修改
 *
 * 和 encrypt 的区别:
 *   encrypt 用"下标直接查表"（O(1)），
 *   decrypt 要"反查位置"，只能用线性查找（O(26)）。
 *   因为 key 是 26 个字母的排列，查找一定能命中，i 不会越界。
 */
void decrypt(char *data, char const *key)
{
    int i;

    while (*data != '\0') {
        if (*data >= 'a' && *data <= 'z') {
            /* 在 key 中找小写密文的位置（忽略大小写比较） */
            for (i = 0; i < 26; i++) {
                if (*data == tolower((unsigned char)*(key + i)))
                    break;
            }
            *data = (char)('a' + i);   /* 位置 i 还原为字母 'a'+i */
        }
        else if (*data >= 'A' && *data <= 'Z') {
            /* 在 key 中找大写密文的位置 */
            for (i = 0; i < 26; i++) {
                if (*data == *(key + i))
                    break;
            }
            *data = (char)('A' + i);   /* 位置 i 还原为字母 'A'+i */
        }
        /* 非字母字符：原样保留 */

        data++;
    }
}