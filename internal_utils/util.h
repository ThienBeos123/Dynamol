#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <limits.h>

#include "../big_numbers/bigNums.h"


/* bigNum_utils.c */
inline void __BIGINT_INTERNAL_EMPINIT__(bigInt *x);
inline void __BIGINT_INTERNAL_LINIT__(bigInt *x, size_t k);
inline void __BIGINT_INTERNAL_ENSCAP__(bigInt *x, size_t k);
inline void __BIGINT_INTERNAL_REALLOC__(bigInt *x, size_t k);
inline void __BIGINT_INTERNAL_FREE__(bigInt *x);
inline uint8_t __BIGINT_INTERNAL_VALID__();
inline uint8_t __BIGINT_INTERNAL_PVALID__();
void __BIGINT_INTERNAL_TRIM_LZ__(bigInt *x);
inline void __BIGINT_INTERNAL_ZSET__(bigInt *x);
void __BIGINT_INTERNAL_ADD_UI64__(bigInt *x, uint64_t val);
void __BIGINT_INTERNAL_MUL_UI64__(bigInt *x, uint64_t val);
uint64_t __BIGINT_INTERNAL_DIVMOD_UI64__(bigInt *x, uint64_t val);
void __BIGINT_INTERNAL_SUB__(bigInt *x, const bigInt *y);



#ifdef __cplusplus
}
#endif