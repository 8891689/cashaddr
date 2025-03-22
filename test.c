/* gcc test.c test_cashaddr.c -o test */
#include "cashaddr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "用法: %s <cashaddr地址>\n", argv[0]);
        return 1;
    }
    const char *input_address = argv[1];
    CashAddrResult result;
    printf("====================================\n");
    if (decode_cashaddr(input_address, &result) != 0) {
        fprintf(stderr, "解码地址失败\n");
        return 1;
    }
    printf("\n解码结果：\n");
    printf("前缀: %s\n", result.prefix);
    printf("版本: %d\n", result.version);
    printf("类型: %s\n", result.type);
    printf("哈希160: %s\n", result.hash160);

    char encoded_address[512] = {0};
    if (encode_cashaddr(result.prefix, result.version, result.type, result.hash160,
                        encoded_address, sizeof(encoded_address)) != 0) {
        fprintf(stderr, "编码地址失败\n");
        return 1;
    }
    printf("\n重新编码地址：\n%s\n", encoded_address);
    printf("====================================\n");
    return 0;
}

