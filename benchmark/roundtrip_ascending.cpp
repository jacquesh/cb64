#include <stdint.h>
#include <stdio.h>

#if defined(API_CB64)
#define CB64_IMPLEMENTATION
#include "cb64.h"

#elif defined(API_OPENSSL_EVP)
#include "openssl/evp.h"
#endif

int main()
{
    uint64_t accum = 0;
    unsigned char encoded[9] = {};
    unsigned char decoded[9] = {};
    for(uint32_t i=0; i<0x0FFFFFFF; i++)
    {
#if defined(API_CB64)
        size_t bytes_encoded = base64_encode((uint8_t*)&i, sizeof(i), encoded, sizeof(encoded));
        size_t bytes_decoded = base64_decode(encoded, bytes_encoded, decoded, sizeof(decoded));
#elif defined(API_OPENSSL_EVP)
        size_t bytes_encoded = EVP_EncodeBlock(encoded, (uint8_t*)&i, sizeof(i));
        size_t bytes_decoded = EVP_DecodeBlock(decoded, encoded, (int)bytes_encoded);
#endif

        accum += bytes_encoded;
        accum += bytes_decoded;
        accum += decoded[0];
        accum += decoded[1];
        accum += decoded[2];
        accum += decoded[3];
        accum += decoded[4];
        accum += decoded[5];
        accum += decoded[6];
        accum += decoded[7];
    }
    printf("Don't optimise out our operation! %llx\n", accum);
    return 0;
}
