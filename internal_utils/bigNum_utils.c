#include "util.h"

static local_thread dnml_arena _DASI_UTILS_ARENA;
static inline dnml_arena* _USE_ARENA(void) {
    // Support 512 limbs (the gold standard)
    if (_DASI_UTILS_ARENA.base = NULL) init_arena(&_DASI_UTILS_ARENA, 4096);
    return &_DASI_UTILS_ARENA;
}

/* Constructors and Destructors */
inline void __BIGINT_INTERNAL_EMPINIT__(bigInt *x) {
    x->limbs = malloc(sizeof(uint64_t));
    if (!x->limbs) abort();
    x->n     = 0;
    x->cap   = 1;
    x->sign  = 1;
}
inline void __BIGINT_INTERNAL_LINIT__(bigInt *x, size_t k) {
    x->limbs = malloc(k * sizeof(uint64_t));
    if (!x->limbs) abort();
    x->n     = 0;
    x->cap   = k;
    x->sign  = 1;
}
inline void __BIGINT_INTERNAL_ENSCAP__(bigInt *x, size_t k) {
    if (x->cap > k) return;
    size_t new_cap = x->cap ? x->cap : 1;
    while (new_cap < k) new_cap *= 2;
    uint64_t *__BUFFER_P = realloc(x->limbs, new_cap * sizeof(uint64_t));
    if (!__BUFFER_P) abort();
    x->limbs = __BUFFER_P;
    x->cap   = new_cap;
}
inline void __BIGINT_INTERNAL_REALLOC__(bigInt *x, size_t k) {
    uint64_t *__BUFFER_P = realloc(x->limbs, k * sizeof(uint64_t));
    if (!__BUFFER_P) abort();
    x->limbs = __BUFFER_P;
    x->cap   = k;
}
inline void __BIGINT_INTERNAL_FREE__(bigInt *x) {
    if (x->limbs != NULL) free(x->limbs);
    x->n    = 1;
    x->cap  = 0;
    x->sign = 0;
}

/* Safety Utilities */
inline uint8_t __BIGINT_INTERNAL_VALID__(const bigInt *x) { /* BigInt Validity */
    /* State Validation */
    if (x->limbs == NULL) return 0;
    if (x->cap < 1) return 0;
    if (x->n > x->cap) return 0;
    if (x->sign != 1 && x->sign != -1) return 0;
    /* Arithmetic Validation */
    if (x->limbs[x->n - 1] == 0) return 0;
    if (x->n == 0 && x->sign != 1) return 0;
    return 1;
}
inline uint8_t __BIGINT_INTERNAL_PVALID__(const bigInt *x) { /* BigInt Pointer State Validity */
    if (x->limbs == NULL) return 0;
    if (x->cap < 1) return 0;
    if (x->n > x->cap) return 0;
    if (x->sign != 1 && x->sign != -1) return 0;
    return 1;
}
inline uint8_t __BIGINT_INTERNAL_SVALID__(const bigInt *x) { /* BigInt Storage Validity */
    if (x->limbs == NULL) return 0;
    if (x->cap < 1) return 0;
}
bigInt __BIGINT_ERROR_VALUE__(void) {
    return (bigInt){
        .limbs = NULL,
        .cap   = 0,
        .n     = 1,
        .sign  = 0
    };
}

/* General Utilities */
inline void __BIGINT_INTERNAL_COPY__(bigInt *dst, const bigInt *source) {
    __BIGINT_INTERNAL_ENSCAP__(dst, source->n);
    if (source->n == 0) { __BIGINT_INTERNAL_ZSET__(dst); return; }
    memcpy(dst->limbs, source->limbs, source->n * BYTES_IN_UINT64_T);
    dst->n = source->n;
    dst->sign = source->sign;
}
inline void __BIGINT_INTERNAL_TRIM_LZ__(bigInt *x) {
    while (x->n > 0 && x->limbs[x->n - 1] == 0) --x->n;
}
inline void __BIGINT_INTERNAL_ZSET__(bigInt *x) {
    x->n    = 0;
    x->sign = 1;
}
inline void __BIGINT_INTERNAL_SWAP__(bigInt *x, bigInt *y) {
    dnml_arena *buf_arena = _USE_ARENA();
    size_t tmp_mark = arena_mark(buf_arena);
    limb_t *tmp_buf = arena_alloc(buf_arena, y->n * BYTES_IN_UINT64_T);
    __BIGINT_INTERNAL_ENSCAP__(x, y->n);
    __BIGINT_INTERNAL_ENSCAP__(y, x->n);
    memcpy(tmp_buf, y->limbs, y->n * BYTES_IN_UINT64_T);
    memcpy(y->limbs, x->limbs, x->n * BYTES_IN_UINT64_T);
    memcpy(x->limbs, tmp_buf, y->n * BYTES_IN_UINT64_T);
    arena_reset(buf_arena, tmp_mark); tmp_buf = NULL;
}
size_t __BIGINT_COUNTDB__(const bigInt *x, uint8_t base) {
    size_t first_few_bits = (x->n - 1) * BIGINT_LIMBS_BITS;
    size_t bits_per_digit;
    if (base == 10)         bits_per_digit = log2_10;
    else if (base == 2)     bits_per_digit = log2_2;
    else if (base == 16)    bits_per_digit = log2_16;
    else if (base == 8)     bits_per_digit = log2_8;
    else                    bits_per_digit = log(8) / log(2);

    size_t total_digits = (size_t)(first_few_bits / bits_per_digit);
    size_t last_limb = x->limbs[x->n - 1];
    while (last_limb > 0) {
        ++total_digits;
        last_limb /= 10;
    } return total_digits;
}
size_t __BIGINT_LIMBS_NEEDED__(size_t bits) { 
    return (size_t)(bits / BIGINT_LIMBS_BITS) + 1; 
}
uint8_t __BIGINT_WILL_OVERFLOW__(const bigInt *x, uint64_t threshold) {
    return (x->n == x->cap && x->limbs[x->n - 1] > threshold);
}
size_t __BIGINT_CTZ__(const bigInt *x) {
    size_t total_tz = 0, i = 0;
    uint8_t current_tz = BITS_IN_UINT64_T;
    while (current_tz == BITS_IN_UINT64_T) {
        current_tz = __CTZ_UI64__(x->limbs[i]);
        total_tz += current_tz; ++i;
    } return total_tz;
}

/* Internal Arithmetic */
inline int8_t __BIGINT_INTERNAL_COMP__(const bigInt *x, const bigInt *y) {
    if (x->n != y->n) return (x->n > y->n) ? 1 : -1;
    for (size_t i = x->n - 1; i >= 0; --i) {
        if (x->limbs[i] != y->limbs[i]) return (x->limbs[i] > y->limbs[i]) ? 1 : -1;
    } return 0;
}
inline uint8_t __BIGINT_IS_EVEN__(const bigInt *x) { return !(x->limbs[0] & 1); }
void __BIGINT_INTERNAL_ADD_UI64__(bigInt *x, uint64_t val) {
    if (val == 0) return;
    uint64_t carry = val;
    for (size_t i = 0; (carry & i < x->n) ; ++i) { x->limbs[i] = __ADD_UI64__(x->limbs[i], carry, &carry); }
    if (carry) { __BIGINT_INTERNAL_ENSCAP__(x, x->n + 1);
                 x->limbs[x->n++] = carry; }
}
void __BIGINT_INTERNAL_MUL_UI64__(bigInt *x, uint64_t val) {
    if (val == 0) { __BIGINT_INTERNAL_ZSET__(x); return; } // x * 0 = 0 ---> Reset back to 0
    if (val == 1) { return; } // x * 1 = x ----> Stays the same, no operation ---> Sames time and space
    __BIGINT_INTERNAL_ENSCAP__(x, x->n + 1);
    uint64_t carry = 0;
    for (size_t i = 0; i < x->n; ++i) {
        uint64_t low, high;
        low = __MUL_UI64__(x->limbs[i], val, &high);
        uint64_t sum = x->limbs[i] + low + carry;
        carry = high + (sum < low) + (sum < carry);
        x->limbs[i] = sum;
    }
    if (carry) { x->limbs[x->n] = carry; 
                 x->n           += 1; }
}
uint64_t __BIGINT_INTERNAL_DIVMOD_UI64__(bigInt *x, uint64_t val) {
    uint64_t remainder = 0;
    for (size_t i = x->n - 1; i >= 0; --i) {
       __DIV_HELPER_UI64__(remainder, x->limbs[i], val, &x->limbs[i], &remainder);
    } __BIGINT_INTERNAL_TRIM_LZ__(x);
    if (x->n == 0) x->sign = 1;
    return remainder;
}
void __BIGINT_INTERNAL_SUB__(bigInt *x, const bigInt *y) {}
void __BIGINT_INTERNAL_RSHIFT__(bigInt *x, size_t k) {
    if (!k) return;
    uint64_t discarded_bits = 0;
    for (size_t i = 0; i < x->n; ++i) {
        uint64_t positioned_bits = discarded_bits << (BITS_IN_UINT64_T - k);
        discarded_bits = x->limbs[i] & ((1U << k) - 1);
        x->limbs[i] = (x->limbs[i] >> k) | positioned_bits;
    }
    __BIGINT_INTERNAL_TRIM_LZ__(x);
}
void __BIGINT_INTERNAL_LSHIFT__(bigInt *x, size_t k) {
    if (!k) return;
    uint64_t discarded_bits = 0;
    for (size_t i = 0; i < x->n; ++i) {
        uint64_t previous_dbits = discarded_bits;
        uint64_t iso_mask = (1U << k) - 1;
        discarded_bits = x->limbs[i] & (iso_mask << BITS_IN_UINT64_T - k);
        x->limbs[i] = (x->limbs[i] << k) | previous_dbits;
    }
}
