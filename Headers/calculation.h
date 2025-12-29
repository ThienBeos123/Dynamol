#include "Calculation Algorithms/singleLimb.h"
#include "Calculation Algorithms/div.h"
#include "Calculation Algorithms/mul.h"
#include "Calculation Algorithms/pow_root.h"
#include "Calculation Algorithms/num_theory.h"

#if HAVE_BUILTIN_CLZ == 1
static inline uint8_t __COUNT_LZ__(uint64_t x) { 
    if (!x) return sizeof(uint64_t) * CHAR_BIT;
    return __builtin_clzll(x)
}
#elif HAVE_BUILTIN_CLZ == 2
#include <intrin.h>
static inline uint8_t __COUNT_LZ__(uint64_t x) { 
    if (!x) return sizeof(uint64_t) * CHAR_BIT;
    uint8_t index;
    _BitScanReverse(&index, x);
    return sizeof(uint64_t) - 1 - index;
}
#else
static inline uint8_t __COUNT_LZ__(uint64_t x) {
    if (!x) return sizeof(uint64_t) * CHAR_BIT;
    uint8_t __COUNT = 0;
    while (x) { x >>= 1; ++__COUNT; }
    return sizeof(uint64_t) * CHAR_BIT - __COUNT;
}
#endif