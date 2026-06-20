#include <stdio.h>
/*
编写一个程序，一行行地读取输入行，直至达到文件尾，算出每行输入行的长度，
然后把最长的那行打印出来，为了简单起见，你可以假定所有的输入行均不超过1000个字符。
*/
int main(){
    int ch;
    int len = 0;
    int max_len = 0;
    char buf[1001];
    char max[1001];
    while ( (ch = getchar()) != EOF )
    {
        if ( ch != '\n' )
        {
            buf[len] = ch;
            len ++;
        }
        else
        {
            if ( len > max_len )
            {
                max_len = len;
                for ( int i = 0; i < len; i ++ )
                {
                    max[i] = buf[i];
                }
                max[len] = '\0';
            }
            len = 0;
        }
    }
    printf ( "%s %d", max, max_len );
    return 0;
}