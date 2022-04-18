#include <stdint.h>
#include <stdio.h>

#if defined(API_CB64)
#define CB64_IMPLEMENTATION
#include "cb64.h"

#elif defined(API_OPENSSL_EVP)
#include "openssl/evp.h"
#endif

uint64_t splitmix64() {
    static uint64_t state = 0x9735b39bf611d800;
	uint64_t z = (state += 0x9e3779b97f4a7c15);
	z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
	z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
	return z ^ (z >> 31);
}

int main()
{
    size_t num_random_u64 = 10 * 1024 * 1024; // 80Mb of uint64_t's
    uint64_t* random_u64 = new uint64_t[num_random_u64];
    for(size_t i=0; i<num_random_u64; i++)
    {
        random_u64[i] = splitmix64();
    }

    size_t encoded_len = 4*((8*num_random_u64)/3 + 1);
    size_t decoded_len = 3*(encoded_len/4);
    uint8_t* encoded = new uint8_t[encoded_len];
    uint8_t* decoded = new uint8_t[decoded_len];

    uint64_t accum = 0;
    for(uint32_t i=0; i<20; i++)
    {
#if defined(API_CB64)
        size_t bytes_encoded = base64_encode((uint8_t*)random_u64, sizeof(uint64_t)*num_random_u64, encoded, encoded_len);
        size_t bytes_decoded = base64_decode(encoded, bytes_encoded, decoded, decoded_len);
#elif defined(API_OPENSSL_EVP)
        size_t bytes_encoded = EVP_EncodeBlock(encoded, (uint8_t*)random_u64, sizeof(uint64_t)*num_random_u64);
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

    delete[] encoded;
    delete[] decoded;
    delete[] random_u64;
    return 0;
}
