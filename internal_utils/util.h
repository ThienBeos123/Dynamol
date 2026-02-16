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
static uint8_t is_numeric(char c);
unsigned char _sign_handle_(const char *str, size_t *curr_pos, uint8_t *sign);
unsigned char _sign_handle_nlen_(const char *str, size_t *curr_pos, uint8_t *sign, size_t len);
unsigned char _prefix_handle_(const char *str, size_t *curr_pos, uint8_t *base);
unsigned char _prefix_handle_nlen_(const char *str, size_t *curr_pos, uint8_t *base, size_t len);

/* ---------------------- */
/* misc_utils.c */
/* ---------------------- */
size_t __BITCOUNT__(size_t digit_count, uint8_t base);
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

void __BIGINT_INTERNAL_ADD_UI64__(bigInt *x, uint64_t val);
void __BIGINT_INTERNAL_MUL_UI64__(bigInt *x, uint64_t val);
uint64_t __BIGINT_INTERNAL_DIVMOD_UI64__(bigInt *x, uint64_t val);
void __BIGINT_INTERNAL_SUB__(bigInt *x, const bigInt *y);



#ifdef __cplusplus
}
#endif

#endif

