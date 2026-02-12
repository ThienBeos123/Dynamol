#ifndef DNML_UTIL_H
#define DNML_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>

#include "../big_numbers/bigNums.h"
#include "../intrinsics/intrinsics.h"
#include "../intrinsics/limb_math.h"
#include "../system/compiler.h"
#include "../settings/numeric_config.h"
#include "../settings/settings.h"

/* ---------------------- */
/* misc_utils.c */
/* ---------------------- */
uint8_t __COUNT_DIGIT_BASE__();
uint64_t __MAG_I64__(int64_t val);


/* ---------------------- */
/* bigNum_utils.c */
/* ---------------------- */
inline void __BIGINT_INTERNAL_EMPINIT__(bigInt *x);
inline void __BIGINT_INTERNAL_LINIT__(bigInt *x, size_t k);
inline void __BIGINT_INTERNAL_ENSCAP__(bigInt *x, size_t k);
inline uint8_t __BIGINT_INTERNAL_REALLOC__(bigInt *x, size_t k);
inline void __BIGINT_INTERNAL_FREE__(bigInt *x);

inline uint8_t __BIGINT_INTERNAL_SVALID__(const bigInt *x);
inline uint8_t __BIGINT_INTERNAL_PVALID__(const bigInt *x);
bigInt __BIGINT_ERROR_VALUE__(void);

void __BIGINT_INTERNAL_TRIM_LZ__(bigInt *x);
inline void __BIGINT_INTERNAL_ZSET__(bigInt *x);
size_t __BIGINT_COUNTDB__(const bigInt x);

void __BIGINT_INTERNAL_ADD_UI64__(bigInt *x, uint64_t val);
void __BIGINT_INTERNAL_MUL_UI64__(bigInt *x, uint64_t val);
uint64_t __BIGINT_INTERNAL_DIVMOD_UI64__(bigInt *x, uint64_t val);
void __BIGINT_INTERNAL_SUB__(bigInt *x, const bigInt *y);



#ifdef __cplusplus
}
#endif

#endif

