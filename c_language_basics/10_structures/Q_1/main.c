#include <stdio.h>
#include <assert.h>

int main(void) {
    struct PHONE_NUMBER {
        int area;      // 区号
        int exchange;  // 交换台号码
        int station;   // 站号码
    };
    struct LONG_DISTANCE_BILL {
        int date;  // 用 YYYYMMDD 表示日期，例如 20260925
        int time;  // 用 HHMMSS 表示时间，例如 143005 表示 14:30:05
        struct PHONE_NUMBER called;   // 被叫电话
        struct PHONE_NUMBER calling;  // 主叫电话，即使用的电话
        struct PHONE_NUMBER billed;   // 付账电话
    };

    // 测试 1：三个号码使用不同数据，检查各成员能否独立保存。
    struct LONG_DISTANCE_BILL bill = {
        .date = 20260925,
        .time = 143005,
        .called = {.area = 212, .exchange = 555, .station = 1234},
        .calling = {.area = 415, .exchange = 666, .station = 5678},
        .billed = {.area = 617, .exchange = 777, .station = 9012}
    };

    assert(bill.date == 20260925 && bill.time == 143005);
    assert(bill.called.area == 212 && bill.called.exchange == 555
           && bill.called.station == 1234);
    assert(bill.calling.area == 415 && bill.calling.exchange == 666
           && bill.calling.station == 5678);
    assert(bill.billed.area == 617 && bill.billed.exchange == 777
           && bill.billed.station == 9012);

    printf("Date: %04d-%02d-%02d\n",
           bill.date / 10000, bill.date / 100 % 100, bill.date % 100);
    printf("Time: %02d:%02d:%02d\n",
           bill.time / 10000, bill.time / 100 % 100, bill.time % 100);
    printf("Called: %d-%d-%d\n",
           bill.called.area, bill.called.exchange, bill.called.station);
    printf("Calling: %d-%d-%d\n",
           bill.calling.area, bill.calling.exchange, bill.calling.station);
    printf("Billed: %d-%d-%d\n",
           bill.billed.area, bill.billed.exchange, bill.billed.station);

    // 测试 2：付账电话可以与主叫电话相同，赋值不会让两者共享存储。
    bill.billed = bill.calling;
    bill.billed.station = 8888;
    assert(bill.billed.area == 415 && bill.billed.exchange == 666
           && bill.billed.station == 8888);
    assert(bill.calling.station == 5678);
    assert(bill.called.station == 1234);

    // 测试 3：复制整个记录后修改副本，不影响原记录；午夜编码为 0。
    struct LONG_DISTANCE_BILL copy = bill;
    copy.date = 20260926;
    copy.time = 0;
    copy.billed.station = 9999;

    assert(copy.date == 20260926 && copy.time == 0);
    assert(copy.billed.station == 9999);
    assert(bill.date == 20260925 && bill.time == 143005);
    assert(bill.billed.station == 8888);
    printf("Midnight: %02d:%02d:%02d\n",
           copy.time / 10000, copy.time / 100 % 100, copy.time % 100);

    // assert 条件不成立时程序会终止；执行到这里说明以上检查均已通过。
    puts("All tests passed.");
    return 0;
}
