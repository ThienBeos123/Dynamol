#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <limits.h>

#ifdef __SIZEOF_INT128__
#define HAVE_UINT128 1
#else
#define HAVE_UINT128 0
#endif

static inline uint64_t __ADD_UI64__(uint64_t __a__, uint64_t __b__, uint64_t *__CARRY);
static inline uint64_t __SUB_UI64__(uint64_t __a__, uint64_t __b__, uint64_t *__BORROW);
static inline void __MUL_UI64__(uint64_t __a__, uint64_t __b__, uint64_t *__LOW_RES__, uint64_t *__HIGH_RES);
static inline uint64_t __DIV_HELPER_128_64__(uint64_t __hi__, uint64_t __lo__, uint64_t __d__, uint64_t *__REMAINDER); 

#ifdef __cplusplus
}
#endif