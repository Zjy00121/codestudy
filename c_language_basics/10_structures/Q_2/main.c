#include <stdio.h>
#include <assert.h>
#include <string.h>

// 题目中的字符串长度不含 '\0'，数组容量需要多留一个字符。
enum {
    NAME_MAX = 20,
    ADDRESS_MAX = 40,
    MODEL_MAX = 20,
    BANK_MAX = 20
};

enum SALE_TYPE {
    CASH_SALE,
    LEASE_SALE,
    LOAN_SALE
};

struct CASH_INFO {
    float suggested_price;  // 生产厂家建议零售价
    float selling_price;    // 实际售出价格
    float sales_tax;        // 营业税
    float licensing_fee;    // 许可费用
};

struct LEASE_INFO {
    float suggested_price;
    float selling_price;
    float down_payment;     // 预付定金
    float security_deposit; // 安全抵押
    float monthly_payment;  // 月付金额
    int lease_term;         // 租赁期限，本例约定单位为月
};

struct LOAN_INFO {
    float suggested_price;
    float selling_price;
    float sales_tax;
    float licensing_fee;
    float down_payment;
    int loan_duration;      // 贷款期限，本例约定单位为月
    float interest_rate;    // 本例用小数表示年利率，0.05 表示 5%
    float monthly_payment;
    char bank_name[BANK_MAX + 1];
};

struct SALES_RECORD {
    char customer_name[NAME_MAX + 1];
    char customer_address[ADDRESS_MAX + 1];
    char model[MODEL_MAX + 1];
    enum SALE_TYPE type;    // 标记联合当前使用哪个成员
    union {
        struct CASH_INFO cash;
        struct LEASE_INFO lease;
        struct LOAN_INFO loan;
    } details;
};

// 根据标记读取对应成员，不读取联合中其他交易类型的成员。
static void print_record(const struct SALES_RECORD *record)
{
    printf("Customer: %s\nAddress: %s\nModel: %s\n",
           record->customer_name, record->customer_address, record->model);

    switch (record->type) {
    case CASH_SALE: {
        const struct CASH_INFO *cash = &record->details.cash;
        printf("Type: Cash\nSuggested price: %.2f\nSelling price: %.2f\n"
               "Sales tax: %.2f\nLicensing fee: %.2f\n",
               cash->suggested_price, cash->selling_price,
               cash->sales_tax, cash->licensing_fee);
        break;
    }
    case LEASE_SALE: {
        const struct LEASE_INFO *lease = &record->details.lease;
        printf("Type: Lease\nSuggested price: %.2f\nSelling price: %.2f\n"
               "Down payment: %.2f\nSecurity deposit: %.2f\n"
               "Monthly payment: %.2f\nLease term: %d months\n",
               lease->suggested_price, lease->selling_price,
               lease->down_payment, lease->security_deposit,
               lease->monthly_payment, lease->lease_term);
        break;
    }
    case LOAN_SALE: {
        const struct LOAN_INFO *loan = &record->details.loan;
        printf("Type: Loan\nSuggested price: %.2f\nSelling price: %.2f\n"
               "Sales tax: %.2f\nLicensing fee: %.2f\nDown payment: %.2f\n"
               "Loan duration: %d months\nInterest rate: %.2f%%\n"
               "Monthly payment: %.2f\nBank: %s\n",
               loan->suggested_price, loan->selling_price,
               loan->sales_tax, loan->licensing_fee, loan->down_payment,
               loan->loan_duration, (double)loan->interest_rate * 100.0,
               loan->monthly_payment, loan->bank_name);
        break;
    }
    default:
        puts("Unknown sale type.");
        break;
    }
    putchar('\n');
}

int main(void)
{
    // 测试数据只验证存储和读取，不计算贷款或税费。
    struct SALES_RECORD cash = {
        .customer_name = "Alice",
        .customer_address = "10 Oak Street",
        .model = "Sedan A",
        .type = CASH_SALE,
        .details.cash = {
            .suggested_price = 25000.0f, .selling_price = 24000.0f,
            .sales_tax = 1800.0f, .licensing_fee = 200.0f
        }
    };
    struct SALES_RECORD lease = {
        .customer_name = "Bob",
        .customer_address = "20 Pine Road",
        .model = "SUV B",
        .type = LEASE_SALE,
        .details.lease = {
            .suggested_price = 32000.0f, .selling_price = 30000.0f,
            .down_payment = 3000.0f, .security_deposit = 500.0f,
            .monthly_payment = 450.0f, .lease_term = 36
        }
    };
    struct SALES_RECORD loan = {
        .customer_name = "Carol",
        .customer_address = "30 Maple Avenue",
        .model = "Hatchback C",
        .type = LOAN_SALE,
        .details.loan = {
            .suggested_price = 22000.0f, .selling_price = 21000.0f,
            .sales_tax = 1500.0f, .licensing_fee = 180.0f,
            .down_payment = 5000.0f, .loan_duration = 48,
            .interest_rate = 0.05f, .monthly_payment = 410.0f,
            .bank_name = "Example Bank"
        }
    };

    assert(cash.type == CASH_SALE && lease.type == LEASE_SALE
           && loan.type == LOAN_SALE);
    assert(cash.details.cash.suggested_price == 25000.0f
           && cash.details.cash.selling_price == 24000.0f
           && cash.details.cash.sales_tax == 1800.0f
           && cash.details.cash.licensing_fee == 200.0f);
    assert(lease.details.lease.suggested_price == 32000.0f
           && lease.details.lease.selling_price == 30000.0f
           && lease.details.lease.down_payment == 3000.0f
           && lease.details.lease.security_deposit == 500.0f
           && lease.details.lease.monthly_payment == 450.0f
           && lease.details.lease.lease_term == 36);
    // 这里比较的是直接存入的浮点常量，没有浮点运算累积误差。
    assert(loan.details.loan.suggested_price == 22000.0f
           && loan.details.loan.selling_price == 21000.0f
           && loan.details.loan.sales_tax == 1500.0f
           && loan.details.loan.licensing_fee == 180.0f
           && loan.details.loan.down_payment == 5000.0f
           && loan.details.loan.loan_duration == 48
           && loan.details.loan.interest_rate == 0.05f
           && loan.details.loan.monthly_payment == 410.0f);
    assert(strcmp(loan.details.loan.bank_name, "Example Bank") == 0);

    print_record(&cash);
    print_record(&lease);
    print_record(&loan);

    // 边界测试：填满题目允许的字符数，并保留最后一个位置存放 '\0'。
    struct SALES_RECORD longest = loan;
    strcpy(longest.customer_name, "12345678901234567890");
    strcpy(longest.customer_address,
           "1234567890123456789012345678901234567890");
    strcpy(longest.model, "ABCDEFGHIJKLMNOPQRST");
    strcpy(longest.details.loan.bank_name, "abcdefghijklmnopqrst");
    assert(strlen(longest.customer_name) == NAME_MAX
           && longest.customer_name[NAME_MAX] == '\0');
    assert(strlen(longest.customer_address) == ADDRESS_MAX
           && longest.customer_address[ADDRESS_MAX] == '\0');
    assert(strlen(longest.model) == MODEL_MAX
           && longest.model[MODEL_MAX] == '\0');
    assert(strlen(longest.details.loan.bank_name) == BANK_MAX
           && longest.details.loan.bank_name[BANK_MAX] == '\0');

    // 结构体复制包含数组内容，修改副本不会改变原记录的字符串。
    assert(strcmp(loan.customer_name, "Carol") == 0);
    assert(strcmp(loan.customer_address, "30 Maple Avenue") == 0);
    assert(strcmp(loan.model, "Hatchback C") == 0);
    assert(strcmp(loan.details.loan.bank_name, "Example Bank") == 0);
    puts("All tests passed: three sale types, maximum strings and record copy.");
    return 0;
}
