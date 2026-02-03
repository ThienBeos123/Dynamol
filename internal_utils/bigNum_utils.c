#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>

#include "../big_numbers/bigNums.h"
#include "util.h"
#include "../system/compiler.h"

/* Constructors and Destructors */
inline void __BIGINT_INTERNAL_EMPINIT__(bigInt *x) {
    x->limbs = malloc(sizeof(uint64_t));
    x->n     = 0;
    x->cap   = 1;
    x->sign  = 1;
}
inline void __BIGINT_INTERNAL_LINIT__(bigInt *x, size_t k) {
    x->limbs = malloc(k * sizeof(uint64_t));
    x->n     = 0;
    x->cap   = k;
    x->sign  = 1;
}
inline void __BIGINT_INTERNAL_ENSCAP__(bigInt *x, size_t k) {
    if (x->cap > k) return;
    size_t new_cap = x->cap ? x->cap : 1;
    while (new_cap < k) new_cap *= 2;
    uint64_t *__BUFFER_P = realloc(x->limbs, new_cap * sizeof(uint64_t));
    assert(__BUFFER_P != NULL);
    x->limbs = __BUFFER_P;
    x->cap   = new_cap;
}
inline void __BIGINT_INTERNAL_REALLOC__(bigInt *x, size_t k) {
    uint64_t *__BUFFER_P = realloc(x->limbs, k * sizeof(uint64_t));
    if (__BUFFER_P == NULL) return 1;
    x->limbs = __BUFFER_P;
    x->cap   = k;
    return 0;
}
inline void __BIGINT_INTERNAL_FREE__(bigInt *x) {
    if (x->limbs != NULL) free(x->limbs);
    x->n    = 1;
    x->cap  = 0;
    x->sign = 0;
}

/* Safety Utilities */
inline uint8_t __BIGINT_INTERNAL_VALID__() {}
inline uint8_t __BIGINT_INTERNAL_PVALID__() {}

/* General Utilities */
void __BIGINT_INTERNAL_TRIM_LZ__(bigInt *x) {
    while (x->n > 0 && x->limbs[x->n - 1] == 0) --x->n;
}
inline void __BIGINT_INTERNAL_ZSET__(bigInt *x) {
    assert(x->limbs != NULL);
    x->n    = 0;
    x->sign = 1;
}
size_t __BIGINT_COUNTDB__() {}
uint8_t __COUNT_DIGIT_BASE__() {}

/* Internal Arithmetic */
void __BIGINT_INTERNAL_ADD_UI64__(bigInt *x, uint64_t val) {
    if (val == 0) return;
    uint64_t carry = val;
    for (size_t i = 0; (carry & i < x->n) ; ++i) { x->limbs[i] = __ADD_UI64__(x->limbs[i], carry, &carry); }
    if (carry) { __BIGINT_INTERNAL_ENSURE_CAP__(x, x->n + 1);
                 x->limbs[x->n++] = carry; }
}
void __BIGINT_INTERNAL_MUL_UI64__(bigInt *x, uint64_t val) {
    if (val == 0) { __BIGINT_INTERNAL_ZSET__(x); return; } // x * 0 = 0 ---> Reset back to 0
    if (val == 1) { return; } // x * 1 = x ----> Stays the same, no operation ---> Sames time and space
    __BIGINT_INTERNAL_ENSURE_CAP__(x, x->n + 1);
    uint64_t carry = 0;
    for (size_t i = 0; i < x->n; ++i) {
        uint64_t low, high;
        __MUL_UI64__(x->limbs[i], val, &low, &high);
        uint64_t sum = x->limbs[i] + low + carry;
        carry = high + (sum < low) + (sum < carry);
        x->limbs[i] = sum;
    }
    if (carry) { x->limbs[x->n] = carry; 
                 x->n           += 1; }
}
uint64_t __BIGINT_INTERNAL_DIVMOD_UI64__(bigInt *x, uint64_t val) {}
void __BIGINT_INTERNAL_SUB__(bigInt *x, const bigInt *y) {}
