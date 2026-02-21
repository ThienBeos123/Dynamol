#ifndef DNML_UTIL_H
#define DNML_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <math.h>
#include <assert.h>

#include "../big_numbers/bigNums.h"
#include "../intrinsics/intrinsics.h"
#include "../intrinsics/limb_math.h"

#include "../system/compiler.h"
#include "../sconfigs/numeric_config.h"
#include "../sconfigs/arena.h"

/* ---------------------- */
/* str_parse.c */
/* ---------------------- */
uint8_t is_numeric(char c);
uint16_t _skip_whitespace__(FILE *stream);
uint8_t _is_valid_digit__(uint16_t *curr_char);
uint8_t _sign_handle_(const char *str, size_t *curr_pos, uint8_t *sign);
uint8_t _sign_handle_nlen_(const char *str, size_t *curr_pos, uint8_t *sign, size_t len);
uint8_t _prefix_handle_(const char *str, size_t *curr_pos, uint8_t *base);
uint8_t _prefix_handle_nlen_(const char *str, size_t *curr_pos, uint8_t *base, size_t len);
uint8_t _prefix_handle_stream__(FILE* stream, uint8_t *base, uint16_t *curr_char);

/* ---------------------- */
/* misc_utils.c */
/* ---------------------- */
size_t __BITCOUNT___(size_t digit_count, uint8_t base);
uint8_t __BASEN_DCOUNT__(uint64_t val, uint8_t base);
uint64_t __MAG_I64__(int64_t val);


/* ---------------------- */
/* bigNum_utils.c */
/* ---------------------- */
inline void __BIGINT_INTERNAL_EMPINIT__(bigInt *x);
inline void __BIGINT_INTERNAL_LINIT__(bigInt *x, size_t k);
inline void __BIGINT_INTERNAL_ENSCAP__(bigInt *x, size_t k);
inline void __BIGINT_INTERNAL_REALLOC__(bigInt *x, size_t k);
inline void __BIGINT_INTERNAL_FREE__(bigInt *x);

inline uint8_t __BIGINT_INTERNAL_VALID__(const bigInt *x);
inline uint8_t __BIGINT_INTERNAL_SVALID__(const bigInt *x);
inline uint8_t __BIGINT_INTERNAL_PVALID__(const bigInt *x);
bigInt __BIGINT_ERROR_VALUE__(void);

void __BIGINT_INTERNAL_TRIM_LZ__(bigInt *x);
inline void __BIGINT_INTERNAL_ZSET__(bigInt *x);
size_t __BIGINT_COUNTDB__(const bigInt *x, uint8_t base);
size_t __BIGINT_LIMBS_NEEDED__(size_t bits);
uint8_t __BIGINT_WILL_OVERFLOW__(const bigInt *x, uint64_t threshold);
size_t __BIGINT_CTZ__(const bigInt *x);

void __BIGINT_INTERNAL_ADD_UI64__(bigInt *x, uint64_t val);
void __BIGINT_INTERNAL_MUL_UI64__(bigInt *x, uint64_t val);
uint64_t __BIGINT_INTERNAL_DIVMOD_UI64__(bigInt *x, uint64_t val);
void __BIGINT_INTERNAL_SUB__(bigInt *x, const bigInt *y);
void __BIGINT_INTERNAL_RSHIFT__(bigInt *x, size_t k);
void __BIGINT_INTERNAL_LSHIFT__(bigInt *x, size_t k);



#ifdef __cplusplus
}
#endif

#endif

