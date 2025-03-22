/* gcc test.c test_cashaddr.c -o test */
#include "cashaddr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <cashaddr address>\n", argv[0]);
        return 1;
    }
    const char *input_address = argv[1];
    CashAddrResult result;
    printf("====================================\n");
    if (decode_cashaddr(input_address, &result) != 0) {
        fprintf(stderr, "Failed to decode address\n");
        return 1;
    }
    printf("\nDecoding Result:\n");
    printf("Prefix: %s\n", result.prefix);
    printf("Version: %d\n", result.version);
    printf("Type: %s\n", result.type);
    printf("Hash160: %s\n", result.hash160);

    char encoded_address[512] = {0};
    if (encode_cashaddr(result.prefix, result.version, result.type, result.hash160,
                        encoded_address, sizeof(encoded_address)) != 0) {
        fprintf(stderr, "Failed to encode address\n");
        return 1;
    }
    printf("\nRe-encoded Address:\n%s\n", encoded_address);
    printf("====================================\n");
    return 0;
}
