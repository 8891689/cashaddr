CashAddr Library (cashaddr.h, cashaddr.c) Detailed Documentation
1. Introduction

This is a C library designed for encoding and decoding Bitcoin Cash (BCH) addresses in the CashAddr format. CashAddr is a more modern and user-friendly BCH address format, intended to reduce the likelihood of errors when users copy and paste addresses. The library provides two core functions: decode_cashaddr and encode_cashaddr, for decoding and encoding CashAddr addresses, respectively. It also includes a series of internal helper functions to perform the underlying bitwise operations, checksum calculations, and Base32 conversions.

2. Why CashAddr?

The traditional BCH address format (usually starting with '1' or '3') is based on Base58Check encoding, which is prone to visual ambiguity (e.g., uppercase 'I' and lowercase 'l', the digit '0' and uppercase 'O'). CashAddr addresses these issues by:

Using Base32 encoding: CashAddr utilizes a smaller character set (qpzry9x8gf2tvdw0s3jn54khce6mua7l), avoiding visually similar characters.

Including a checksum: CashAddr incorporates a robust checksum mechanism that detects most common input errors.

Case-insensitive (technically, but unified to lowercase internally): While technically case-insensitive according to the specification, the BCH ecosystem generally recommends and uses lowercase CashAddr addresses. This improves readability and consistency. The CashAddr library internally converts addresses to lowercase.

Including a prefix: CashAddr addresses include a prefix (e.g., "bitcoincash"), used to distinguish between different networks (mainnet, testnet, etc.) and address types.

3. Core Functions
3.1. decode_cashaddr Function

Purpose: Decodes a CashAddr address string into its constituent parts.

Input:

address: A pointer to the CashAddr address string to be decoded (e.g., "bitcoincash:qpm2qsznhks23z7629mms6s4cwef74vcwvy22gdx6a").

result: A pointer to a CashAddrResult structure, used to store the decoded results.

CashAddrResult Structure:

typedef struct {
    char prefix[32]; // Address prefix (e.g., "bitcoincash")
    int  version;    // Version number (0-7, the lower 3 bits of the version byte)
    char type[16];   // Address type ("P2PKH" or "P2SH")
    char hash160[41];// 160-bit hash (hexadecimal string, 40 characters + '\0')
} CashAddrResult;


Return Value:

0: Decoding successful.

-1: Decoding failed. Possible reasons include:

Invalid CashAddr address string.

Checksum mismatch.

Memory allocation failure.

Address is too short or too long

Invalid character.

Invalid Payload

Decoding Process (Detailed Steps):

Prefix Separation:

Find the colon (':') in the address string.

If a colon is found, the part before the colon is the prefix, and the part after the colon is the Base32 encoded data.

If a colon is not found, the default prefix "bitcoincash" is used, and the entire string is treated as Base32 encoded data.

Base32 Decoding:

Convert the Base32 encoded data part into an array of 5-bit integers.

Iterate through each character in the Base32 string.

Find the position of each character in the CHARSET (qpzry9x8gf2tvdw0s3jn54khce6mua7l).

If a character is not in the CHARSET, return an error (invalid character).

Store the character's position (0-31) in the 5-bit integer array.

Checksum Verification:

Extract the last 8 5-bit integers from the Base32 decoded data; these are the checksum.

Recalculate the checksum using the _polymod and _calculate_checksum functions.

The _polymod function is the polynomial modulo operation defined in the CashAddr specification.

The _calculate_checksum function calculates the checksum using the prefix and the 5-bit data (excluding the checksum).

Compare the recalculated checksum with the extracted checksum.

If the checksums do not match, return an error (invalid checksum).

Payload Unpacking:

Convert the 5-bit integer array (excluding the checksum) back into a byte array (the payload).

Use the _unpack_5bit function to perform this conversion.

Version and Type Parsing:

The first byte of the payload is the version byte.

The lower 3 bits of the version byte are the address version (0-7).

Bits 4-8 (the upper 5 bits) of the version byte represent the address type:

0: P2PKH (Pay-to-Public-Key-Hash)

1: P2SH (Pay-to-Script-Hash)

Other values: "Unknown Type"

Hash Extraction:

The 20 bytes starting from the second byte in the payload are the 160-bit hash.

Convert these 20 bytes into a 40-character hexadecimal string.

Populate Result:

Copy the decoded prefix, version, type, and hash into the CashAddrResult structure.

3.2. encode_cashaddr Function

Purpose: Encodes address components into a CashAddr address string.

Input:

prefix: The address prefix (e.g., "bitcoincash").

version: The address version (0-7).

type: The address type ("P2PKH" or "P2SH").

hash160: The 160-bit hash (a 40-character hexadecimal string).

out_address: A character buffer to store the encoded CashAddr address.

out_size: The size of the out_address buffer.

Return Value:

0: Encoding successful.

-1: Encoding failed. Possible reasons include:

Invalid address type (not "P2PKH" or "P2SH").

Invalid 160-bit hash (length is not 40 characters, or contains non-hexadecimal characters).

Output buffer is too small.

Encoding Process (Detailed Steps):

Type Check:

Verify that the type string is either "P2PKH" or "P2SH".

If not, return an error (unsupported address type).

Version Byte Construction:

Construct the version byte based on type and version:

If type is "P2PKH", the upper 5 bits of the version byte are 0.

If type is "P2SH", the upper 5 bits of the version byte are 1.

The lower 3 bits of the version byte are version.

Hexadecimal Conversion:

Use the hexstr2bytes function to convert the 40-character hexadecimal hash string into a 20-byte array.

If the conversion fails (e.g., the hash contains non-hexadecimal characters), return an error.

Payload Construction:

Create a 21-byte payload array.

The first byte is the version byte.

The next 20 bytes are the byte array of the hash.

Payload Packing:

Use the _pack_5bit function to convert the 21-byte payload array into a 5-bit integer array.

Checksum Calculation:

Calculate the checksum using the _polymod and _calculate_checksum functions.

The _calculate_checksum function calculates the checksum using the prefix and the 5-bit payload array.

The checksum is an array of 8 5-bit integers.

Base32 Encoding:

Concatenate the 5-bit payload array and the 8 5-bit checksum integers.

Use the CHARSET to convert this combined 5-bit integer array into a Base32 encoded string.

Address Concatenation:

Concatenate the prefix, a colon (':'), and the Base32 encoded string to form the complete CashAddr address.

Use the snprintf function to perform the concatenation and check if the output buffer is large enough.

Return: Copy the concatenated address string into out_address.

4. Internal Helper Functions

_polymod(const int *values, size_t count): Implements the polynomial modulo operation defined in the CashAddr specification, used for checksum calculation. This is a core part of the CashAddr specification.

_calculate_checksum(const char *prefix, const int *payload, int payload_len, int checksum[8]): Calculates the checksum for a CashAddr address. It uses the _polymod function.

_pack_5bit(const unsigned char *data, int data_len, int *out, int max_out): Converts a byte array (data) into a 5-bit integer array (out). This is part of the encoding process, converting 8-bit bytes into the 5-bit data units used by CashAddr.

_unpack_5bit(const int *data, int data_len, unsigned char *out, int max_out): Converts a 5-bit integer array (data) back into a byte array (out). This is part of the decoding process, converting CashAddr's 5-bit data units back into 8-bit bytes.

hexchar2int(char c): Converts a single hexadecimal character ('0'-'9', 'a'-'f', 'A'-'F') to its corresponding integer value (0-15).

hexstr2bytes(const char *hex, unsigned char *out, int max_out): Converts a hexadecimal string (hex) into a byte array (out).

5. Compilation and Usage

Include Header: Include the cashaddr.h header file in your C code:

#include "cashaddr.h"
IGNORE_WHEN_COPYING_START
content_copy
download
Use code with caution.
C
IGNORE_WHEN_COPYING_END

Compilation: Compile cashaddr.c along with your main program file:

gcc your_program.c cashaddr.c -o your_program
IGNORE_WHEN_COPYING_START
content_copy
download
Use code with caution.
Bash
IGNORE_WHEN_COPYING_END

You can use any standard C compiler (e.g., GCC, Clang).
3. Calling the API:

#include "cashaddr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // Decoding Example
    CashAddrResult result;
    const char *address = "bitcoincash:qpm2qsznhks23z7629mms6s4cwef74vcwvy22gdx6a";
    if (decode_cashaddr(address, &result) == 0) {
        printf("Decoding successful:\n");
        printf("  Prefix: %s\n", result.prefix);
        printf("  Version: %d\n", result.version);
        printf("  Type: %s\n", result.type);
        printf("  Hash160: %s\n", result.hash160);
    } else {
        printf("Decoding failed.\n");
    }

    // Encoding Example (with deliberate error)
    char encoded_address[128];
    const char *prefix = "bitcoincash";
    int version = 0;
    const char *type = "P2PKH";
    const char *hash160 = "b774k7h5xlj29mms6s4cwef74vcwvy22gdx6a"; // Incorrect hash for testing
    if (encode_cashaddr(prefix, version, type, hash160, encoded_address, sizeof(encoded_address)) == 0)
        {
        printf("Encoding successful:\n%s\n",encoded_address);
        }
     else{
         printf("Encoding failed!\n");
     }

        // Correct Encoding example
         strcpy(hash160, "b774f3552c9f4b06b7485436644959979b758338");
        if (encode_cashaddr(prefix, version, type, hash160, encoded_address, sizeof(encoded_address)) == 0)
        {
            printf("Encoding successful:\n%s\n",encoded_address);
        }
        else{
            printf("Encoding failed!\n");
        }
    return 0;
}
IGNORE_WHEN_COPYING_START
content_copy
download
Use code with caution.
C
IGNORE_WHEN_COPYING_END
6. Conclusion

cashaddr.h and cashaddr.c provide a complete, reliable, and easy-to-use library for handling the CashAddr address format for Bitcoin Cash. It includes all the necessary functions to easily integrate CashAddr addresses into any C application that needs to work with BCH addresses. The detailed error handling and internal function design ensure the robustness and accuracy of the library.
