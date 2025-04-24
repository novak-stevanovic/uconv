#ifndef _UCONV_H_
#define _UCONV_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* -------------------------------------------------------------------------- */

typedef int uc_status_t;

#define UC_SUCCESS 0
#define UC_ERR_INVALID_ARG 1
#define UC_ERR_UNHANDLED 2
#define UC_ERR_NOT_ENOUGH_CAPACITY 3
#define UC_ERR_INVALID_SIZE 4

#define UC_ERR_OVERLONG 101
#define UC_ERR_SURROGATE 102
#define UC_ERR_OVERFLOW 103
#define UC_ERR_INVALID_SBYTE 104
#define UC_ERR_INVALID_CBYTE 105

/* -------------------------------------------------------------------------- */

typedef uint8_t uc_flags_t;

#define UC_ALLOW_SURROGATES (1 << 0)
#define UC_ALLOW_OVERLONG (1 << 1)

void uc_utf8_to_utf32(const uint8_t* utf8_seq, size_t size,
        uint32_t* out_utf32_seq, size_t capacity, uc_flags_t flags,
        size_t* out_processed_bytes, size_t* out_len, uc_status_t* out_status);

void uc_utf32_to_utf8(const uint32_t* utf32_seq, size_t len,
        uint8_t* out_utf8_seq, size_t capacity, uc_flags_t flags,
        size_t* out_processed_bytes, size_t* out_len, uc_status_t* out_status);

#endif // _UCONV_H_
