#include "uconv.h"

#define _SURROGATE_RANGE_START 0xD800
#define _SURROGATE_RANGE_END 0xDFFF

#define _1BYTE_RANGE_START 0x0000
#define _1BYTE_RANGE_END 0x007F
#define _2BYTE_RANGE_START 0x0080
#define _2BYTE_RANGE_END 0x07FF
#define _3BYTE_RANGE_START 0x0800
#define _3BYTE_RANGE_END 0xFFFF
#define _4BYTE_RANGE_START 0x010000
#define _4BYTE_RANGE_END 0x10FFFF


#include <stdlib.h>
/* UTF8 SEQUENCES:
 * 1 BYTE SEQUENCE - FORMAT: 0xxxxxxx
 * 2 BYTE SEQUENCE - FORMAT: 110xxxxx
 * 3 BYTE SEQUENCE - FORMAT: 1110xxxx
 * 4 BYTE SEQUENCE - FORMAT: 11110xxx
 * COUCINUATION BYTE - FORMAT: 10xxxxxx */

static inline bool _codepoint_surrogate(uint32_t codepoint)
{
    return ((codepoint >= _SURROGATE_RANGE_START) &&
            (codepoint <= _SURROGATE_RANGE_END));
}

static inline bool _codepoint_overflow(uint32_t codepoint)
{
    return (codepoint > _4BYTE_RANGE_END);
}

static inline bool _utf8_overlong(uint32_t codepoint, size_t unit_utf8_size)
{
    switch(unit_utf8_size)
    {
        case 1:
            return false;
        case 2:
            return (codepoint < _2BYTE_RANGE_START);
        case 3:
            return (codepoint < _3BYTE_RANGE_START);
        case 4:
            return (codepoint < _4BYTE_RANGE_START);
        default: // undefined
            return true;
    }
}

#define __RETURN(processed_bytes_val, len_val, status_val)                     \
    if(out_processed_bytes != NULL)                                            \
        *out_processed_bytes = (processed_bytes_val);                          \
    if(out_len != NULL)                                                        \
        *out_len = (len_val);                                                  \
    if(out_status != NULL)                                                     \
        *out_status = (status_val);                                            \
    return;                                                                    \


void uc_utf8_to_utf32(const uint8_t* utf8_seq, size_t size,
        uint32_t* out_utf32_seq, size_t capacity, uint8_t flags,
        size_t* out_processed_bytes, size_t* out_len, uc_status_t* out_status)
{
    if((utf8_seq == NULL) || (out_utf32_seq == NULL))
    {
        __RETURN(0, 0, UC_ERR_INVALID_ARG);
    }

    size_t i = 0;
    size_t counter = 0;
    size_t it_len;
    uint32_t it_codepoint;
    for(; i < size; counter++)
    {
        if(counter >= capacity)
        {
            __RETURN(i, counter, UC_ERR_NOT_ENOUGH_CAPACITY);
        }

        if((utf8_seq[i] & 0x80) == 0) // 1 byte
        {
            it_len = 1;
            it_codepoint = (uint32_t)(utf8_seq[i]);
        }
        else if((utf8_seq[i] & 0xE0) == 0xC0) // 2 bytes
        {
            it_len = 2;
            // set the first 3 bytes to 0
            it_codepoint = (uint32_t)(utf8_seq[i] & (~0xE0));
        }
        else if((utf8_seq[i] & 0xF0) == 0xE0) // 3 bytes
        {
            it_len = 3;
            // set the first 4 bytes to 0
            it_codepoint = (uint32_t)(utf8_seq[i] & (~0xF0));
        }
        else if((utf8_seq[i] & 0xF8) == 0xF0) // 4 bytes
        {
            it_len = 4;
            // set the first 5 bytes to 0
            it_codepoint = (uint32_t)(utf8_seq[i] & (~0xF8));
        }
        else // invalid start byte
        {
            __RETURN(i, counter, UC_ERR_INVALID_SBYTE);
        }

        // current start byte + size of current symbol > size of the whole utf8 sequence
        if((i + it_len) > size)
        {
            __RETURN(i, counter, UC_ERR_INVALID_SIZE);
        }

        size_t end = i + it_len;
        i++;
        for(; i < end; i++) // process continuation bytes
        {
            it_codepoint <<= 6;

            if((utf8_seq[i] & 0xC0) != 0x80) // invalid continuation byte
            {
                __RETURN(i - 1, counter, UC_ERR_INVALID_CBYTE);
            }

            it_codepoint |= (utf8_seq[i] ^ 0x80);
        }

        // if(_is_overflow(it_codepoint))
        // {
        //     __RETURN(i, counter, UC_ERR_UTF_OVERFLOW);
        // }

        // Check for overlong if needed
        if(!(flags & UC_ALLOW_OVERLONG) &&
            _utf8_overlong(it_codepoint, it_len))
        {
            __RETURN(i, counter, UC_ERR_OVERLONG);
        }

        // Check for surrogate if needed
        if(!(flags & UC_ALLOW_SURROGATES) && _codepoint_surrogate(it_codepoint))
        {
            __RETURN(i, counter, UC_ERR_SURROGATE);
        }

        out_utf32_seq[counter] = it_codepoint;
    }

    __RETURN(i, counter, UC_SUCCESS);
}

void uc_utf32_to_utf8(const uint32_t* utf32_seq, size_t count,
        uint8_t* out_utf8_seq, size_t capacity, uint8_t flags,
        size_t* out_processed_bytes, size_t* out_len, uc_status_t* out_status)
{
    if((utf32_seq == NULL) || (out_utf8_seq == NULL))
    {
        __RETURN(0, 0, UC_ERR_INVALID_ARG);
    }

    size_t i = 0, j;
    size_t bcount = 0;
    uint32_t it_cp;
    size_t it_len;
    for(; i < count; i++, bcount += it_len)
    {
        it_cp = utf32_seq[i];
        if(!(flags & UC_ALLOW_SURROGATES) && _codepoint_surrogate(it_cp))
        {
            __RETURN(bcount, i, UC_ERR_SURROGATE);
        }
        if(_codepoint_overflow(it_cp))
        {
            __RETURN(bcount, i, UC_ERR_OVERFLOW);
        }

        if(it_cp <= _1BYTE_RANGE_END)
        {
            it_len = 1;
            out_utf8_seq[bcount] = (uint8_t)it_cp;
        }
        else if(it_cp <= _2BYTE_RANGE_END)
        {
            // 0 0 00000abc defghijk
            // 1. 110abcde
            // 2. 10fghijk

            it_len = 2;
            out_utf8_seq[bcount] = ((uint8_t)(it_cp >> 6)) | 0xC0;
        }
        else if(it_cp <= _3BYTE_RANGE_END)
        {
            // 0 0 abcdefgh ijklmnop
            // 1. 1110abcd
            // 2. 10efghij
            // 3. 10klmnop

            it_len = 3;
            out_utf8_seq[bcount] = ((uint8_t)(it_cp >> 12)) | 0xE0;
        }
        else // 4 bytes
        {
            // 0 000abcde fghijklm nopqrstu
            // 1. 11110abc
            // 2. 10defghi
            // 3. 10jklmno
            // 4. 10pqrstu

            it_len = 4;
            out_utf8_seq[bcount] = ((uint8_t)(it_cp >> 18)) | 0xF0;
        }

        if((bcount + it_len) > capacity)
        {
            __RETURN(bcount, i, UC_ERR_NOT_ENOUGH_CAPACITY);
        }

        size_t it_shift;
        uint8_t it_cbyte;
        for(j = 0; j < it_len - 1; j++)
        {
            it_shift = 6 * (it_len - 1 - j - 1);
            it_cbyte = (((uint8_t)(it_cp >> it_shift)) & (~0xC0)) | 0x80;
            out_utf8_seq[bcount + j + 1] = it_cbyte;
        }

    }

    __RETURN(bcount, i, UC_SUCCESS);
}
