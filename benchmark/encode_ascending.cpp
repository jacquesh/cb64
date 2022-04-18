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
    unsigned char encoded[9] = {}; // 4 bytes encodes to 8 bytes (since it covers 2 triplets) + 1 for a null terminator, if required
    for(uint32_t i=0; i<0x0FFFFFFF; i++)
    {
#if defined(API_CB64)
        size_t encoded_bytes = base64_encode((uint8_t*)&i, sizeof(i), encoded, sizeof(encoded));
#elif defined(API_OPENSSL_EVP)
        int encoded_bytes = EVP_EncodeBlock(encoded, (uint8_t*)&i, sizeof(i));
#endif
        accum += encoded_bytes;
        accum += encoded[0];
        accum += encoded[1];
        accum += encoded[2];
        accum += encoded[3];
        accum += encoded[4];
        accum += encoded[5];
        accum += encoded[6];
        accum += encoded[7];
    }
    printf("Don't optimise out our operation! %llx\n", accum);
    return 0;
}
