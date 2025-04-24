#ifndef _UCONV_H_
#define _UCONV_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* UTF8 SEQUENCES:
 * 1 BYTE SEQUENCE - START BYTE FORMAT: 0xxxxxxx
 * 2 BYTE SEQUENCE - START BYTE FORMAT: 110xxxxx
 * 3 BYTE SEQUENCE - START BYTE FORMAT: 1110xxxx
 * 4 BYTE SEQUENCE - START BYTE FORMAT: 11110xxx
 * CONTINUATIOn BYTE - FORMAT: 10xxxxxx */

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

#define UC_ALLOW_SURROGATE (1 << 0)
#define UC_ALLOW_OVERLONG (1 << 1)

void uc_utf8_to_utf32(const uint8_t* utf8_seq, size_t len,
        uint32_t* out_utf32_seq, size_t capacity, uc_flags_t flags,
        size_t* out_width, size_t* out_len, uc_status_t* out_status);

void uc_utf32_to_utf8(const uint32_t* utf32_seq, size_t width,
        uint8_t* out_utf8_seq, size_t capacity, uc_flags_t flags,
        size_t* out_width, size_t* out_len, uc_status_t* out_status);

#endif // _UCONV_H_
