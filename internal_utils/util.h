#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <limits.h>

#include "../big_numbers/bigNums.h"

#ifdef __SIZEOF_INT128__ 
    #define HAVE_UINT128 1
#else
    #define HAVE_UINT128 0
#endif

#if defined(__GNUC__) || defined(__clang__)
    #define HAS_BUILTIN_CLZ 1
#elif defined(_MSC_VER)
    #define HAS_BUILTIN_CLZ 2
#else
    #define HAS_BUILTIN_CLZ 0
#endif

/* singleLimb.c */
static inline uint64_t __ADD_UI64__(uint64_t __a__, uint64_t __b__, uint64_t *__CARRY);
static inline uint64_t __SUB_UI64__(uint64_t __a__, uint64_t __b__, uint64_t *__BORROW);
static inline void __MUL_UI64__(uint64_t __a__, uint64_t __b__, uint64_t *__LOW_RES__, uint64_t *__HIGH_RES);
static inline void __DIV_HELPER_UI64__(uint64_t __lo__, uint64_t __hi__, uint64_t __div__, 
                                       uint64_t *__QUOT__, uint64_t *__REM__);

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