// cb64 v1.0 - Simple & fast base64 encoding & decoding
//
// This single file contains all source for cb64.
// Simply include this file wherever you wish to use it and add:
//      #define CB64_IMPLEMENTATION
// above the include directive in *one* source file to define the implementation.
//
// See the bottom of this file for the license.

#ifndef CB64_HEADER_5B22515C
#define CB64_HEADER_5B22515C

#include <stddef.h>

// Fills the given output_buffer with the data from input_buffer encoded as base64.
// Returns the number of bytes written to output_buffer on success, or 0 on failure.
// As long as input_buffer_length > 0, a return value of 0 will always indicate failure.
size_t base64_encode(unsigned char* input_buffer, size_t input_buffer_length, unsigned char* output_buffer, size_t output_buffer_length);

// Fills the given output_buffer with the data from input_buffer decoded as base64.
// Returns the number of bytes written to output_buffer on success, or 0 on failure.
// As long as input_buffer_length > 0, a return value of 0 will always indicate failure.
size_t base64_decode(unsigned char* input_buffer, size_t input_buffer_length, unsigned char* output_buffer, size_t output_buffer_length);

// ====================
// Implementation below
// ====================
//
#ifdef CB64_IMPLEMENTATION

// Size is 64 + 1 for the null-terminator that C inserts at the end of a string literal
const unsigned char cb64_alphabet[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

const unsigned char cb64_alphabet_lookup[] = 
{
    // Unused ASCII characters
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    62, // Plus
    0, 0, 0, // Unused ASCII characters
    63, // Slash
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // Decimal digits
    0, 0, 0, 0, 0, 0, 0, // Unused ASCII characters
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // Upper-case characters
    0, 0, 0, 0, 0, 0, // Unused ASCII characters
    26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // Lower-case characters
};

size_t base64_encode(unsigned char* input_buffer, size_t input_buffer_length, unsigned char* output_buffer, size_t output_buffer_length)
{
    // TODO: Support alternative alphabets (e.g -_ in place of +/), for things like url-safe en/decoding
    unsigned char padding = '=';

    size_t input_parity = input_buffer_length % 3;
    size_t triplet_count = input_buffer_length/3;

    if(output_buffer_length < 4*(triplet_count + (input_parity != 0)))
    {
        // Insufficient output buffer space
        return 0;
    }

    for(size_t i=0; i<triplet_count; i++)
    {
        // | Input Octet Hex       |      0x3F      |      0x2A     |      0x99      |
        // | Input Octet Decimal   |       63       |       42      |       153      |
        // | Equivalent Bits       | 0 0 1 1 1 1 1 1 0 0 1 0 1 0 1 0 1 0 0 1 1 0 0 1 |
        // | Output Sextet Decimal |      15    |     50    |     42    |     25     |
        // | Output Alphabet ASCII |      P     |     y     |     q     |     Z      |
        unsigned int sextet1 = (input_buffer[3*i] >> 2);
        unsigned int sextet2 = ((input_buffer[3*i] & 0x03) << 4) | (input_buffer[3*i + 1] >> 4);
        unsigned int sextet3 = ((input_buffer[3*i + 1] & 0x0F) << 2) | (input_buffer[3*i + 2] >> 6);
        unsigned int sextet4 = (input_buffer[3*i + 2] & 0x3F);
        output_buffer[4*i] = cb64_alphabet[sextet1];
        output_buffer[4*i + 1] = cb64_alphabet[sextet2];
        output_buffer[4*i + 2] = cb64_alphabet[sextet3];
        output_buffer[4*i + 3] = cb64_alphabet[sextet4];
    }

    size_t bytes_written = 4*triplet_count;
    if(input_parity == 2)
    {
        unsigned int sextet1 = (input_buffer[3*triplet_count] >> 2);
        unsigned int sextet2 = ((input_buffer[3*triplet_count] & 0x03) << 4) | (input_buffer[3*triplet_count + 1] >> 4);
        unsigned int sextet3 = ((input_buffer[3*triplet_count + 1] & 0x0F) << 2);
        output_buffer[bytes_written] = cb64_alphabet[sextet1];
        output_buffer[bytes_written + 1] = cb64_alphabet[sextet2];
        output_buffer[bytes_written + 2] = cb64_alphabet[sextet3];
        output_buffer[bytes_written + 3] = padding;
        bytes_written += 4;
    }
    else if(input_parity == 1)
    {
        unsigned int sextet1 = (input_buffer[3*triplet_count] >> 2);
        unsigned int sextet2 = ((input_buffer[3*triplet_count] & 0x03) << 4);
        output_buffer[bytes_written] = cb64_alphabet[sextet1];
        output_buffer[bytes_written + 1] = cb64_alphabet[sextet2];
        output_buffer[bytes_written + 2] = padding;
        output_buffer[bytes_written + 3] = padding;
        bytes_written += 4;
    }

    return bytes_written;
}

size_t base64_decode(unsigned char* input_buffer, size_t input_buffer_length, unsigned char* output_buffer, size_t output_buffer_length)
{
    if(input_buffer_length % 4 != 0)
    {
        // Input buffer is not valid base64, which must be a multiple of 4 bytes/chars long
        return 0;
    }

    size_t quadruplet_count = input_buffer_length/4;
    size_t output_byte_count = quadruplet_count * 3; // TODO: This is pessimistic when the input has been padded with =
    if(output_buffer_length < output_byte_count)
    {
        // Insufficient output buffer space
        return 0;
    }

    for(size_t i=0; i<input_buffer_length; i++)
    {
        // Invalid characters in the input buffer
        if(input_buffer[i] >= sizeof(cb64_alphabet_lookup))
        {
            return 0;
        }
    }

    if((input_buffer_length > 0) && (input_buffer[input_buffer_length-1] == '='))
    {
        quadruplet_count--;
    }

    for(size_t i=0; i<quadruplet_count; i++)
    {
        // | Input Alphabet ASCII  |      P     |     y     |     q     |     Z      |
        // | Input Sextet Decimal  |      15    |     50    |     42    |     25     |
        // | Equivalent Bits       | 0 0 1 1 1 1 1 1 0 0 1 0 1 0 1 0 1 0 0 1 1 0 0 1 |
        // | Output Octet Decimal  |       63       |       42      |       153      |
        // | Output Octet Hex      |      0x3F      |      0x2A     |      0x99      |
        unsigned int sextet1 = cb64_alphabet_lookup[input_buffer[4*i]];
        unsigned int sextet2 = cb64_alphabet_lookup[input_buffer[4*i + 1]];
        unsigned int sextet3 = cb64_alphabet_lookup[input_buffer[4*i + 2]];
        unsigned int sextet4 = cb64_alphabet_lookup[input_buffer[4*i + 3]];
        unsigned int octet1 = (sextet1 << 2) | (sextet2 >> 4);
        unsigned int octet2 = ((sextet2 & 0x0F) << 4) | (sextet3 >> 2);
        unsigned int octet3 = ((sextet3 & 0x03) << 6) | sextet4;
        output_buffer[3*i] = octet1;
        output_buffer[3*i + 1] = octet2;
        output_buffer[3*i + 2] = octet3;
    }

    size_t bytes_written = 3*quadruplet_count;
    if((input_buffer_length > 0) && (input_buffer[input_buffer_length-1] == '='))
    {
        if((input_buffer_length > 1) && (input_buffer[input_buffer_length-2] == '='))
        {
            unsigned int sextet1 = cb64_alphabet_lookup[input_buffer[4*quadruplet_count]];
            unsigned int sextet2 = cb64_alphabet_lookup[input_buffer[4*quadruplet_count + 1]];
            unsigned int octet1 = (sextet1 << 2) | (sextet2 >> 4);
            output_buffer[bytes_written] = octet1;
            bytes_written += 1;
        }
        else
        {
            unsigned int sextet1 = cb64_alphabet_lookup[input_buffer[4*quadruplet_count]];
            unsigned int sextet2 = cb64_alphabet_lookup[input_buffer[4*quadruplet_count + 1]];
            unsigned int sextet3 = cb64_alphabet_lookup[input_buffer[4*quadruplet_count + 2]];
            unsigned int octet1 = (sextet1 << 2) | (sextet2 >> 4);
            unsigned int octet2 = ((sextet2 & 0x0F) << 4) | (sextet3 >> 2);
            output_buffer[bytes_written] = octet1;
            output_buffer[bytes_written + 1] = octet2;
            bytes_written += 2;
        }
    }

    return bytes_written;
}

#endif // CB64_IMPLEMENTATION

// -----------------------------------------------------------------------
// This is free and unencumbered software released into the public domain.
// 
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
// 
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
// 
// For more information, please refer to <https://unlicense.org>
// -----------------------------------------------------------------------
#endif // CB64_HEADER_5B22515C
