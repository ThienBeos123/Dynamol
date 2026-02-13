#include "bigNums_func.h"

    /* NOTE: +) For any function of type uint8_t, 
    *           it is to be treated like a void function that handles errors (0 = success, 1 = error)  
    */


//todo ============================================ INTRODUCTION ============================================= */
/* Attribute Explanation:
*   +) sign     (uint8_t)       : Stores the sign (negative or positive)
*   +) limbs    (*uint64_t)     : Pointer to each limb that holds part of the bigInt number
*   +) n        (size_t)        : Number of currently used limbs (used to determine the bigInt value and in arithmetic)
*   +) cap      (size_t)        : Essentially the bigInt object/number's maximum capacity 
* /

/* Terms Explanation:
*   +) Mutative Model: Changes an existing bigInt object/variable value in place (Eg: x += 10; )
*   +) Functional Model: -) Creates a new variable with the value of the expression (Eg: int x = 5 + 10; ) 
*                        -) This transfer ownership of the allocated limbs to the caller, 
*                           forcing a manual deletion of the object (See more detailed explanation below on Functional Arithmetic)
*/




//* ======================================== CONSTRUCTORS & DESTRUCTOR ======================================= */
void __BIGINT_EMPTY_INIT__(bigInt *x) {
    if (x->limbs != NULL) return; // The bigInt is already initialized
    limb_t *P_BUFFER__ = malloc(sizeof(limb_t));
    if (P_BUFFER__ == NULL) abort();
    x->limbs = P_BUFFER__;
    x->cap   = 1;
    x->n     = 0;
    x->sign  = 1;
}
void __BIGINT_FREE__(bigInt *x) {
    if (x->limbs == NULL) return;
    free(x->limbs);
    x->limbs = NULL;              
    x->n     = 0;
    x->cap   = 0;
    x->sign  = 1;
}
void __BIGINT_LIMBS_INIT__(bigInt *x, size_t n) {
    if (n == 0 || x->limbs != NULL) return; // Intializing 0 cap OR already initialized
    limb_t *__BUFFER_P = calloc(n, sizeof(limb_t));
    if (__BUFFER_P == NULL) abort();
    x->limbs = __BUFFER_P;
    x->cap   = n; // A capacity of n (n spaces in the heap)
    x->n     = 0; // Currently using no limb
    x->sign  = 1;
}
bigint_status __BIGINT_STANDARD_INIT__(bigInt *x, const bigInt y) {
    if (x->limbs != NULL) return BIGINT_SUCCESS; // Already Initialized
    #ifdef _DNML_DEBUG_MODE
        assert(__BIGINT_STATE_VALIDATE__(y));
    #else
        if (!__BIGINT_STATE_VALIDATE__(y)) return BIGINT_ERR_INVAL;
    #endif

    size_t alloc_size = (y.n) ? y.n : 1;
    limb_t *__BUFFER_P = malloc(alloc_size * sizeof(limb_t));
    if (__BUFFER_P == NULL) abort();
    x->limbs    = __BUFFER_P;
    if (y.n) memcpy(x->limbs, y.limbs, y.n * sizeof(limb_t));
    x->n        = y.n;
    x->cap      = alloc_size;
    x->sign     = (y.n) ? y.sign : 1;
    return BIGINT_SUCCESS;
}
void __BIGINT_UI64_INIT__(bigInt *x, uint64_t in) {
    if (x->limbs != NULL) return; // ALREADY INITIALIZED
    limb_t *__BUFFER_P = malloc(sizeof(limb_t));
    if (__BUFFER_P == NULL) abort();
    x->limbs    = __BUFFER_P;
    x->limbs[0] = in;
    x->n        = (in) ? 1 : 0;
    x->cap      = 1;
    x->sign     = 1;
}
void __BIGINT_I64_INIT__(bigInt *x, int64_t in) {
    if (x->limbs != NULL) return; // ALREADY INITIALIZED
    limb_t *__BUFFER_P = malloc(sizeof(limb_t));
    if (__BUFFER_P == NULL) abort();
    x->limbs    = __BUFFER_P;
    x->limbs[0] = __MAG_I64__(in);
    x->n        = (in) ? 1 : 0;
    x->cap      = 1;
    x->sign     = (in < 0) ? -1 : 1;
}
void __BIGINT_LD_INIT__(bigInt *x, long double in) {}




//* =============================================== ASSIGNMENTS ============================================== */
void __BIGINT_SET_BIGINT__(const bigInt x, bigInt *receiver) {}
/* --------- BigInt --> Primitive Types --------- */
void __BIGINT_SET_UI64__(const bigInt x, uint64_t *receiver) {
    assert(__BIGINT_STATE_VALIDATE__(x));
    *receiver = (x.n) ? x.limbs[0] : 0;
}
void __BIGINT_SET_I64__(const bigInt x, int64_t *receiver) {
    assert(__BIGINT_STATE_VALIDATE__(x));
    uint64_t raw_u64 = (x.n) ? x.limbs[0] : 0;
    uint64_t abs_int64_min = (uint64_t)(llabs(INT64_MIN + 1)) + 1;
    if (raw_u64 > abs_int64_min && x.sign == -1) *receiver = (int64_t)(raw_u64 & I64_MIN_BIT_MASK) * x.sign;
    else if (raw_u64 > INT64_MAX && x.sign == 1) *receiver = (int64_t)(raw_u64 & I64_MAX_BIT_MASK) * x.sign;
    else *receiver = ((int64_t)raw_u64) * x.sign;
}
void __BIGINT_SET_LD__(const bigInt x, long double *receiver) {}
bigint_status __BIGINT_SET_UI64_SAFE__(const bigInt x, uint64_t *receiver) {
    assert(__BIGINT_VALIDATE__(x));
    if (x.sign == -1 || x.n > 1) return BIGINT_ERR_RANGE;
    *receiver = (x.n) ? x.limbs[0] : 0;
    return BIGINT_SUCCESS;
}
bigint_status __BIGINT_SET_I64_SAFE__(const bigInt x, int64_t *receiver) {
    assert(__BIGINT_VALIDATE__(x));
    if (x.n > 1) BIGINT_ERR_RANGE;
    uint64_t raw_u64 = (x.n) ? x.limbs[0] : 0;
    uint64_t abs_int64_min = (uint64_t)(llabs(INT64_MIN + 1)) + 1;
    if (raw_u64 > abs_int64_min && x.sign == -1) return BIGINT_ERR_RANGE;
    if (raw_u64 > INT64_MAX && x.sign == 1) return BIGINT_ERR_RANGE;
    *receiver = ((int64_t)raw_u64) * x.sign;
    return BIGINT_SUCCESS;
}
bigint_status __BIGINT_SET_LD_SAFE__(const bigInt x, long double *receiver) {}
/* --------- Primitive Types --> BigInt --------- */
void __BIGINT_GET_UI64__(uint64_t val, bigInt *receiver) {
    assert(__BIGINT_MUTATIVE_SUBJECT_VALIDATE__(receiver));
    receiver->limbs[0] = val;
    receiver->n        = (val) ? 1 : 0;
    receiver->sign     = 1;
}
void __BIGINT_GET_I64__(int64_t val, bigInt *receiver) {
    assert(__BIGINT_MUTATIVE_SUBJECT_VALIDATE__(receiver));
    uint64_t abs_val = __MAG_I64__(val);
    receiver->limbs[0] = abs_val;
    receiver->n        = (val) ? 1 : 0;
    receiver->sign     = (val < 0) ? -1 : 1;
}
void __BIGINT_GET_LD__(long double x, bigInt *receiver) {}
bigint_status __BIGINT_GET_LD_SAFE__(long double x, bigInt *receiver) {}




//* =============================================== CONVERSIONS ============================================== */
/* --------- BigInt --> Primitive Types --------- */
uint64_t __BIGINT_TO_UI64__(const bigInt x) {
    assert(__BIGINT_STATE_VALIDATE__(x));
    uint64_t res = (x.n) ? x.limbs[0] : 0;
    return res;
}
int64_t __BIGINT_TO_I64__(const bigInt x) {
    assert(__BIGINT_STATE_VALIDATE__(x));
    int64_t res;
    uint64_t raw_u64 = (x.n) ? x.limbs[0] : 0;
    uint64_t abs_int64_min = (uint64_t)(llabs(INT64_MIN + 1)) + 1;
    if (raw_u64 > abs_int64_min && x.sign == -1) res = (int64_t)(raw_u64 & I64_MIN_BIT_MASK) * x.sign;
    else if (raw_u64 > INT64_MAX && x.sign == 1) res = (int64_t)(raw_u64 & I64_MAX_BIT_MASK) * x.sign;
    else res = ((int64_t)raw_u64) * x.sign;
    return res;
}
long double __BIGINT_TO_LD_(const bigInt x) {}
uint64_t __BIGINT_TO_UI64_SAFE__(const bigInt x, bigint_status *err) {
    assert(err);
    assert(__BIGINT_VALIDATE__(x));
    if (x.sign == -1 || x.n > 1) { *err = BIGINT_ERR_RANGE; return -1; }
    uint64_t res = (x.n) ? x.limbs[0] : 0;
    *err = BIGINT_SUCCESS; return res;
}
int64_t __BIGINT_TO_I64_SAFE__(const bigInt x, bigint_status *err) {
    assert(err);
    assert(__BIGINT_VALIDATE__(x));
    if (x.n > 1) { *err = BIGINT_ERR_RANGE; return INT64_MIN; }
    uint64_t raw_u64 = (x.n) ? x.limbs[0] : 0;
    uint64_t abs_int64_min = (uint64_t)(llabs(INT64_MIN + 1)) + 1;
    if (raw_u64 > abs_int64_min && x.sign == -1) { *err = BIGINT_ERR_RANGE; return INT_MIN; }
    if (raw_u64 > INT64_MAX && x.sign == 1) { *err = BIGINT_ERR_RANGE; return INT_MIN; }
    int64_t res = ((int64_t)raw_u64) * x.sign;
    *err = BIGINT_SUCCESS; return res;
}
long double __BIGINT_TO_LD_SAFE_(const bigInt x, bigint_status *err) {}
/* --------- Primitive Types --> BigInt --------- */
bigInt __BIGINT_FROM_UI64__(uint64_t x) {
    bigInt res; __BIGINT_EMPTY_INIT__(&res);
    if (x) {
        res.limbs[0] = x;
        res.n        = 1;
    }
    return res;
}
bigInt __BIGINT_FROM_I64__(int64_t x) {
    bigInt res; __BIGINT_EMPTY_INIT__(&res);
    if (x) {
        res.limbs[0] = __MAG_I64__(x);
        res.n        = 1;
        res.sign     = (x < 0) ? -1 : 1;
    }
    return res;
}
bigInt __BIGINT_FROM_LD_(long double x) {}
bigInt __BIGINT_FROM_LD_SAFE__(long double x) {}




//* =========================================== BITWISE OPERATIONS =========================================== */
bigInt __BIGINT_NOT__(const bigInt x) {
    assert(__BIGINT_STATE_VALIDATE__(x));
    bigInt res; __BIGINT_LIMBS_INIT__(&res, x.n);
    for (size_t i = 0; i < x.n; ++i) {
        res.limbs[0] = ~x.limbs[0];
    }
    res.n    = x.n;
    res.sign = x.sign;
    __BIGINT_NORMALIZE__(&res);
    return res;
}
bigInt __BIGINT_RSHIFT__(const bigInt x, size_t k) {
    assert(__BIGINT_VALIDATE__(x));
    uint64_t discarded_bits = 0;
    bigInt res; __BIGINT_LIMBS_INIT__(&res, k);
    for (size_t i = 0; i < x.n; ++i) {
        uint64_t positioned_bits = discarded_bits << (BITS_IN_UINT64_T - k);
        res.limbs[i] = (x.limbs[i] >> k) | positioned_bits;
        discarded_bits = x.limbs[i] & ((1U << k) - 1);
    }
    return res;
}
bigInt __BIGINT_LSHIFT__(const bigInt x, size_t k) {
    assert(__BIGINT_VALIDATE__(x));
    uint64_t discarded_bits = 0;
    bigInt res; __BIGINT_LIMBS_INIT__(&res, x.n);
    for (size_t i = 0; i < x.n; ++i) {
        res.limbs[i] = (x.limbs[i] << k) | discarded_bits;
        uint64_t iso_mask = (1U << k) - 1;
        discarded_bits = x.limbs[i] & (iso_mask << BITS_IN_UINT64_T - k);
    }
    return res;
}
void __BIGINT_MUT_RSHIFT__(bigInt *x, size_t k) {
    assert(__BIGINT_PVALIDATE__(x));
    uint64_t discarded_bits = 0;
    for (size_t i = 0; i < x->n; ++i) {
        uint64_t positioned_bits = discarded_bits << (BITS_IN_UINT64_T - k);
        discarded_bits = x->limbs[i] & ((1U << k) - 1);
        x->limbs[i] = (x->limbs[i] >> k) | positioned_bits;
    }
}
void __BIGINT_MUT_LSHIFT__(bigInt *x, size_t k) {
    assert(__BIGINT_PVALIDATE__(x));
    uint64_t discarded_bits = 0;
    for (size_t i = 0; i < x->n; ++i) {
        uint64_t previous_dbits = discarded_bits;
        uint64_t iso_mask = (1U << k) - 1;
        discarded_bits = x->limbs[i] & (iso_mask << BITS_IN_UINT64_T - k);
        x->limbs[i] = (x->limbs[i] << k) | previous_dbits;
    }
}
/* ------------- Mutative Bitwise Operations ------------- */
void __BIGINT_MUT_AND_UI64__  (bigInt *x, uint64_t val) {
    assert(__BIGINT_PVALIDATE__(x));
    if (x->n == 0) return;
    x->limbs[0] = x->limbs[0] & val;
    x->n        = (x->limbs[0]) ? 1 : 0;
    x->sign     = (x->limbs[0]) ? x->sign : 1;
    return;
}
void __BIGINT_MUT_NAND_UI64__ (bigInt *x, uint64_t val) {
    assert(__BIGINT_PVALIDATE__(x));
    if (x->n == 0) {
        x->limbs[0] = INT64_MAX;
        x->n        = 1;
    } else {
        x->limbs[0] = ~(x->limbs[0] & val);
        if (x->n > 1) memset(&x->limbs[1], UINT64_MAX, x->n - 1);
    }
}
void __BIGINT_MUT_OR_UI64__   (bigInt *x, uint64_t val) {
    assert(__BIGINT_PVALIDATE__(x));
    if (!val);
    else if (x->n == 0) {
        uint64_t res = 0 | val;
        x->limbs[0] = res;
        x->n        = (res) ? 1 : 0;
        x->sign     = (res) ? x->sign : 1;
    } else {
        for (size_t i = 0; i < x->n; ++i) {
            uint64_t b = (i == 0) ? val : 0;
            x->limbs[0] = x->limbs[0] | b;
        } __BIGINT_NORMALIZE__(x);
    }
}
void __BIGINT_MUT_NOR_UI64__  (bigInt *x, uint64_t val) {
    assert(__BIGINT_PVALIDATE__(x));
    if (x->n == 0) {
        uint64_t res = ~(0 | val);
        x->limbs[0] = res;
        x->n        = (res) ? 1 : 0;
        x->sign     = (res) ? x->sign : 1;
    } else {
        for (size_t i = 0; i < x->n; ++i) {
            uint64_t b = (i == 0) ? val : 0;
            x->limbs[0] = ~(x->limbs[0] | b);
        } __BIGINT_NORMALIZE__(x);
    }
}
void __BIGINT_MUT_XOR_UI64__  (bigInt *x, uint64_t val) {
    assert(__BIGINT_PVALIDATE__(x));
    if (x->n == 0) {
        uint64_t res = 0 ^ val;
        x->limbs[0] = res;
        x->n        = (res) ? 1 : 0;
        x->sign     = (res) ? x->sign : 1;
    } else {
        for (size_t i = 0; i < x->n; ++i) {
            uint64_t b = (i == 0) ? val : 0;
            x->limbs[0] = x->limbs[0] ^ b;
        } __BIGINT_NORMALIZE__(x);
    }
}
void __BIGINT_MUT_XNOR_UI64__ (bigInt *x, uint64_t val) {
    assert(__BIGINT_PVALIDATE__(x));
    if (x->n == 0) {
        uint64_t res = ~(0 ^ val);
        x->limbs[0] = res;
        x->n        = (res) ? 1 : 0;
        x->sign     = (res) ? x->sign : 1;
    } else {
        for (size_t i = 0; i < x->n; ++i) {
            uint64_t b = (i == 0) ? val : 0;
            x->limbs[0] = ~(x->limbs[0] ^ b);
        } __BIGINT_NORMALIZE__(x);
    }
}
void __BIGINT_MUT_AND__  (bigInt *x, const bigInt y) {
    assert(__BIGINT_PVALIDATE__(x) && __BIGINT_VALIDATE__(y));
    if (!x->n);
    else if (!y.n) __BIGINT_RESET__(x);
    else {
        size_t operation_range = max(x->n, y.n);
        __BIGINT_RESERVE__(x, operation_range);
        for (size_t i = 0; i < operation_range; ++i) {
            uint64_t a = (i < x->n) ? x->limbs[i] : 0;
            uint64_t b = (i < y.n)  ? y.limbs[i]  : 0;
            x->limbs[0] = a & b;
        } __BIGINT_NORMALIZE__(x);
    }
}
void __BIGINT_MUT_NAND__ (bigInt *x, const bigInt y) {
    assert(__BIGINT_PVALIDATE__(x) && __BIGINT_VALIDATE__(y));
    if (!x->n) {
        size_t expanded_cap = (y.n) ? y.n : 1;
        __BIGINT_RESERVE__(x, expanded_cap);
        memset(x->limbs, UINT64_MAX, expanded_cap);
    } else if (!y.n) memset(x->limbs, UINT64_MAX, x->n);
    else {
        size_t operation_range = max(x->n, y.n);
        __BIGINT_RESERVE__(x, operation_range);
        for (size_t i = 0; i < operation_range; ++i) {
            uint64_t a = (i < x->n) ? x->limbs[i] : 0;
            uint64_t b = (i < y.n)  ? y.limbs[i]  : 0;
            x->limbs[0] = ~(a & b);
        } __BIGINT_NORMALIZE__(x);
    }
}
void __BIGINT_MUT_OR__   (bigInt *x, const bigInt y) {
    assert(__BIGINT_PVALIDATE__(x) && __BIGINT_VALIDATE__(y));
    if (!y.n);
    else if (!x->n) __BIGINT_MUT_COPY__(x, y);
    else {
        size_t operation_range = max(x->n, y.n);
        __BIGINT_RESERVE__(x, operation_range);
        for (size_t i = 0; i < operation_range; ++i) {
            uint64_t a = (i < x->n) ? x->limbs[i] : 0;
            uint64_t b = (i < y.n)  ? y.limbs[i]  : 0;
            x->limbs[0] = a | b;
        } __BIGINT_NORMALIZE__(x);
    }
}
void __BIGINT_MUT_NOR__  (bigInt *x, const bigInt y) {
    assert(__BIGINT_PVALIDATE__(x) && __BIGINT_VALIDATE__(y));
    if (!x->n && !y.n) {
        x->limbs[0] = UINT64_MAX;
        x->n        = 1;
    } else {
        size_t operation_range = max(x->n, y.n);
        __BIGINT_RESERVE__(x, operation_range);
        for (size_t i = 0; i < operation_range; ++i) {
            uint64_t a = (i < x->n) ? x->limbs[i] : 0;
            uint64_t b = (i < y.n)  ? y.limbs[i]  : 0;
            x->limbs[0] = ~(a | b);
        } __BIGINT_NORMALIZE__(x);
    }
}
void __BIGINT_MUT_XOR__  (bigInt *x, const bigInt y) {
    assert(__BIGINT_PVALIDATE__(x) && __BIGINT_VALIDATE__(y));
    if (!x->n && !y.n);
    else {
        size_t operation_range = max(x->n, y.n);
        __BIGINT_RESERVE__(x, operation_range);
        for (size_t i = 0; i < operation_range; ++i) {
            uint64_t a = (i < x->n) ? x->limbs[i] : 0;
            uint64_t b = (i < y.n)  ? y.limbs[i]  : 0;
            x->limbs[0] = a ^ b;
        } __BIGINT_NORMALIZE__(x);
    }
}
void __BIGINT_MUT_XNOR__ (bigInt *x, const bigInt y) {
    assert(__BIGINT_PVALIDATE__(x) && __BIGINT_VALIDATE__(y));
    if (!x->n && !y.n) {
        x->limbs[0] = UINT64_MAX;
        x->n        = 1;
    } else {
        size_t operation_range = max(x->n, y.n);
        __BIGINT_RESERVE__(x, operation_range);
        for (size_t i = 0; i < operation_range; ++i) {
            uint64_t a = (i < x->n) ? x->limbs[i] : 0;
            uint64_t b = (i < y.n)  ? y.limbs[i]  : 0;
            x->limbs[0] = ~(a ^ b);
        } __BIGINT_NORMALIZE__(x);
    }
}
/* ------------- Functional, Fixed-width ------------- */
bigInt __BIGINT_AND_UI64__  (const bigInt x, uint64_t val) {}
bigInt __BIGINT_NAND_UI64__ (const bigInt x, uint64_t val) {}
bigInt __BIGINT_OR_UI64__   (const bigInt x, uint64_t val) {}
bigInt __BIGINT_NOR_UI64__  (const bigInt x, uint64_t val) {}
bigInt __BIGINT_XOR_UI64__  (const bigInt x, uint64_t val) {}
bigInt __BIGINT_XNOR_UI64__ (const bigInt x, uint64_t val) {}
bigInt __BIGINT_AND__   (const bigInt x, const bigInt y) {}
bigInt __BIGINT_NAND__  (const bigInt x, const bigInt y) {}
bigInt __BIGINT_OR__    (const bigInt x, const bigInt y) {}
bigInt __BIGINT_NOR__   (const bigInt x, const bigInt y) {}
bigInt __BIGINT_XOR__   (const bigInt x, const bigInt y) {}
bigInt __BIGINT_XNOR__  (const bigInt x, const bigInt y) {}
/* ------------- Functional, Explicit-width ------------- */
bigInt __BIGINT_EX_AND_UI64__  (const bigInt x, uint64_t val, size_t width) {}
bigInt __BIGINT_EX_NAND_UI64__ (const bigInt x, uint64_t val, size_t width) {}
bigInt __BIGINT_EX_OR_UI64__   (const bigInt x, uint64_t val, size_t width) {}
bigInt __BIGINT_EX_NOR_UI64__  (const bigInt x, uint64_t val, size_t width) {}
bigInt __BIGINT_EX_XOR_UI64__  (const bigInt x, uint64_t val, size_t width) {}
bigInt __BIGINT_EX_XNOR_UI64__ (const bigInt x, uint64_t val, size_t width) {}
bigInt __BIGINT_EX_AND_I64__  (const bigInt x, int64_t val, size_t width) {}
bigInt __BIGINT_EX_NAND_I64__ (const bigInt x, int64_t val, size_t width) {}
bigInt __BIGINT_EX_OR_I64__   (const bigInt x, int64_t val, size_t width) {}
bigInt __BIGINT_EX_NOR_I64__  (const bigInt x, int64_t val, size_t width) {}
bigInt __BIGINT_EX_XOR_I64__  (const bigInt x, int64_t val, size_t width) {}
bigInt __BIGINT_EX_XNOR_I64__ (const bigInt x, int64_t val, size_t width) {}
bigInt __BIGINT_EX_AND__   (const bigInt x, const bigInt y, size_t width) {}
bigInt __BIGINT_EX_NAND__  (const bigInt x, const bigInt y, size_t width) {}
bigInt __BIGINT_EX_OR__    (const bigInt x, const bigInt y, size_t width) {}
bigInt __BIGINT_EX_NOR__   (const bigInt x, const bigInt y, size_t width) {}
bigInt __BIGINT_EX_XOR__   (const bigInt x, const bigInt y, size_t width) {}
bigInt __BIGINT_EX_XNOR__  (const bigInt x, const bigInt y, size_t width) {}





//* =============================================== COMPARISONS ============================================== */
int8_t __BIGINT_COMPARE_MAGNITUDE_UI64__(const bigInt *x, const uint64_t val) {
    if (x->n > 1) return 1;
    if (x->sign == -1) return -1;
    if (x->limbs[0] > val) return 1;
    else if (x->limbs[0] < val) return -1;
    return 0;
}
int8_t __BIGINT_COMPARE_MAGNITUDE__(const bigInt *a, const bigInt *b) {
    if (a->n != b->n) return (a->n > b->n) ? 1 : -1;
    // Loops from most-significant digit down to least-significant digit
    for (size_t i = a->n - 1; i >= 0; --i) {
        if (a->limbs[i] != b->limbs[i]) return (a->limbs[i] > b->limbs[i]) ? 1 : -1; 
        // Compare which one current most-significant digit is bigger
    }
    return 0;
}
/* --------------- Integer - I64 --------------- */
uint8_t __BIGINT_EQUAL_I64__(const bigInt x, const int64_t val) {
    assert(__BIGINT_VALIDATE__(x));
    if (x.n == 0) return (val) ? 0 : 1;
    int8_t val_sign = (val < 0) ? -1 : 1;
    if (val_sign != x.sign) return 0;
    if (x.n      > 1)       return 0;
    return x.limbs[0] == __MAG_I64__(val);
}
uint8_t __BIGINT_LESS_I64__(const bigInt x, const int64_t val) { 
    assert(__BIGINT_VALIDATE__(x));
    if (x.n == 0) return (val > 0) ? 1 : 0;
    int8_t val_sign = (val < 0) ? -1 : 1;
    if (val_sign != x.sign) return (x.sign < val_sign);
    if (x.n      > 1)       return (x.sign == -1);
    if (x.limbs[0] > __MAG_I64__(val)) return (x.sign == -1);
    return (x.limbs[0] < __MAG_I64__(val)) && (x.sign == 1);
}
uint8_t __BIGINT_MORE_I64__(const bigInt x, const int64_t val) {
    assert(__BIGINT_VALIDATE__(x));
    if (x.n == 0) return (val < 0) ? 1 : 0;
    int8_t val_sign = (val < 0) ? -1 : 1;
    if (val_sign != x.sign) return (x.sign > val_sign);
    if (x.n      > 1)       return (x.sign == 1);
    if (x.limbs[0] < __MAG_I64__(val)) return (x.sign == -1);
    return (x.limbs[0] > __MAG_I64__(val)) && (x.sign == 1);
}
uint8_t __BIGINT_LESS_OR_EQUAL_I64__(const bigInt x, const int64_t val) {
    assert(__BIGINT_VALIDATE__(x));
    if (x.n == 0) return (val >= 0) ? 1 : 0;
    int8_t val_sign = (val < 0) ? -1 : 1;
    if (x.sign != val_sign) return (x.sign < val_sign);
    if (x.n    > 1)         return (x.sign == -1);
    // Case eg: 189 > 171  |  -189 < -171
    if (x.limbs[0] > __MAG_I64__(val)) return (x.sign == -1);
    return (x.sign == 1); // Case eg: 178 < 181  |   -178 > -181
}
uint8_t __BIGINT_MORE_OR_EQUALL_I64__(const bigInt x, const int64_t val) {
    assert(__BIGINT_VALIDATE__(x));
    if (x.n == 0) return (val <= 0) ? 1 : 0;
    int8_t val_sign = (val < 0) ? -1 : 1;
    if (x.sign != val_sign) return (x.sign > val_sign);
    if (x.n    > 1)         return (x.sign == 1);
    // Case eg: 189 > 171  |  -189 < -171
    if (x.limbs[0] > __MAG_I64__(val)) return (x.sign == 1);
    return (x.sign == -1); // Case eg: 178 < 181  |   -178 > -181
}
/* ---------- Unsigned Integer - UI64 ---------- */
uint8_t __BIGINT_EQUAL_UI64__(const bigInt x, const uint64_t val) {
    assert(__BIGINT_VALIDATE__(x));
    if (x.n == 0) return (val) ? 0 : 1;
    if (x.sign == -1) return 0;
    if (x.n    > 1) return 0;
    return (x.limbs[0] == val);
}
uint8_t __BIGINT_LESS_UI64__(const bigInt x, const uint64_t val) {
    assert(__BIGINT_VALIDATE__(x));
    if (x.n == 0) return (val) ? 1 : 0;
    if (x.sign == -1) return 1;
    if (x.n    > 1) return 0;
    return (x.limbs[0] < val);
}
uint8_t __BIGINT_MORE_UI64__(const bigInt x, const uint64_t val) {
    assert(__BIGINT_VALIDATE__(x));
    if (x.n == 0) return (val) ? 0 : 1;
    if (x.sign == -1) return 0;
    if (x.n    > 1) return 1;
    return (x.limbs[0] > val);
}
uint8_t __BIGINT_LESS_OR_EQUAL_UI64__(const bigInt x, const uint64_t val) {
    assert(__BIGINT_VALIDATE__(x));
    if (x.n == 0) return 1; // Always true, as every R+ numbers are always >= 0
    if (x.sign == -1) return 1;
    if (x.n    > 1) return 0;
    return (x.limbs[0] <= val);
}
uint8_t __BIGINT_MORE_OR_EQUALL_UI64__(const bigInt x, const uint64_t val) {
    assert(__BIGINT_VALIDATE__(x));
    if (x.n == 0) return (val) ? 0 : 1;
    if (x.sign == -1) return 0;
    if (x.n    > 1) return 1;
    return (x.limbs[0] >= val);
}
/* ------------------- BigInt ------------------ */
uint8_t __BIGINT_EQUAL__(const bigInt a, const bigInt b) {
    assert(__BIGINT_VALIDATE__(a) && __BIGINT_VALIDATE__(b));
    if (!a.n) return (!b.n) ? 1 : 0;
    if (a.sign != b.sign) return 0;
    if (a.n    != b.n)    return 0;
    return memcmp(a.limbs, b.limbs, a.n * sizeof(uint64_t)) == 0;
}
uint8_t __BIGINT_LESS__(const bigInt a, const bigInt b) {
    assert(__BIGINT_VALIDATE__(a) && __BIGINT_VALIDATE__(b));
    if (a.sign != b.sign) return (a.sign < b.sign);
    if (a.n    != b.n)    return (a.sign == 1) ? (a.n < b.n) : (a.n > b.n);
    return (a.sign == 1) ? 
                memcmp(a.limbs, b.limbs, a.n * sizeof(uint64_t)) < 0 :
                memcmp(a.limbs, b.limbs, a.n * sizeof(uint64_t)) > 0;
}
uint8_t __BIGINT_MORE__(const bigInt a, const bigInt b) {
    assert(__BIGINT_VALIDATE__(a) && __BIGINT_VALIDATE__(b));
    if (a.sign != b.sign) return (a.sign > b.sign);
    if (a.n    != b.n)    return (a.sign == 1) ? (a.n > b.n) : (a.n < b.n);
    return (a.sign == 1) ? 
                memcmp(a.limbs, b.limbs, a.n * sizeof(uint64_t)) > 0 :
                memcmp(a.limbs, b.limbs, a.n * sizeof(uint64_t)) < 0;
}
uint8_t __BIGINT_LESS_OR_EQUAL__(const bigInt a, const bigInt b) {
    assert(__BIGINT_VALIDATE__(a) && __BIGINT_VALIDATE__(b));
    if (a.sign != b.sign) return (a.sign < b.sign);
    if (a.n    != b.n)    return (a.sign == 1) ? (a.n < b.n) : (a.n > b.n);
    return (a.sign == 1) ? 
                memcmp(a.limbs, b.limbs, a.n * sizeof(uint64_t)) <= 0 :
                memcmp(a.limbs, b.limbs, a.n * sizeof(uint64_t)) >= 0;
}
uint8_t __BIGINT_MORE_OR_EQUAL__(const bigInt a, const bigInt b) {
    assert(__BIGINT_VALIDATE__(a) && __BIGINT_VALIDATE__(b));
    if (a.sign != b.sign) return (a.sign > b.sign);
    if (a.n    != b.n)    return (a.sign == 1) ? (a.n > b.n) : (a.n < b.n);
    return (a.sign == 1) ? 
                memcmp(a.limbs, b.limbs, a.n * sizeof(uint64_t)) >= 0 :
                memcmp(a.limbs, b.limbs, a.n * sizeof(uint64_t)) <= 0;
}





//* ========================================= MAGNITUDE MATHEMATICA ========================================== */
/* -------------------- MAGNITUDED ARITHMETIC --------------------- */
void __BIGINT_MAGNITUDED_ADD__(bigInt *res, const bigInt *a, const bigInt *b) {
    size_t max = max(a->n, b->n);
    // Set the minimum capacity of res to be 1 bigger
    // than the largest capacity between a & b ----> res->cap = max + 1
    __BIGINT_RESERVE__(res, max + 1);
    uint64_t carry = 0;
    for (size_t i = 0; i < max; ++i) {
        uint64_t x = (i < a->n) ? a->limbs[i] : 0; // Assigning limb at position i of a to x
        uint64_t y = (i < b->n) ? b->limbs[i] : 0; // Assigning limb at position i of b to x
        res->limbs[i] = __ADD_UI64__(x, y, &carry); // Do single-limb addition with carry (if have) --> Stores the carry
    }
    if (carry) res->limbs[max] = carry; // If carry still needed ---> stores the carry in the (res->cap - 1) limb
    res->n = max + (carry != 0);
}
void __BIGINT_MAGNITUDED_SUB__(bigInt *res, const bigInt *a, const bigInt *b) {
    __BIGINT_RESERVE__(res, a->n);
    uint64_t borrow = 0;
    for (size_t i = 0; i < a->n; ++i) {
        uint64_t y = (i < b->n) ? b->limbs[i] : 0;
        res->limbs[i] = __SUB_UI64__(a->limbs[i], y, &borrow); // Do single-limb subtraction with borrow ---> Stores the borrow
    }
    res->n = a->n;
}
void __BIGINT_MAGNITUDED_MUL__(bigInt *res, const bigInt *a, const bigInt *b) {
    __BIGINT_MUL_DISPATCH__(res, a, b);
}
void __BIGINT_MAGNITUDED_DIVMOD__(bigInt *quot, bigInt *rem, const bigInt *a, const bigInt *b) {
    __BIGINT_DIVMOD_DISPATCH__(a, b, quot, rem);
}
void __BIGINT_MAGNITUDED_MUL_UI64__(bigInt *res, const bigInt *x, const uint64_t val) {
    // Since the divisor size is small (n <= 1), we implement inline schoolbook multiplication
    __BIGINT_RESERVE__(res, x->n + 1);
    uint64_t carry = 0;
    for (size_t i = 0; i < x->n; ++i) {
        uint64_t low, high;
        low = __MUL_UI64__(x->limbs[i], val, &high);
        uint64_t sum = low + carry;
        carry = high + (sum < low) + (sum < carry);
        res->limbs[i] = sum;
    }
    res->n = x->n;
    if (carry) { res->limbs[res->n++] = carry; }
}
void __BIGINT_MAGNITUDED_DIVMOD_UI64__(bigInt *quot, uint64_t *rem, const bigInt *x, const uint64_t val) {
    // Since the divisior size is small (n <= 1), we implement inline normal/long division
    assert(val); // Checks for invalid operation ( x / 0 )
    __BIGINT_RESERVE__(&quot, x->n+1); quot->n = x->n;
    uint64_t remainder = 0;
    for (size_t i = x->n; i-- > 0;) {
       __DIV_HELPER_UI64__(remainder, 0, val, &quot->limbs[i], &remainder);
    }
    *rem = remainder;
    __BIGINT_NORMALIZE__(quot);
}
/* --------------- MAGNITUDED CORE NUMBER-THEORETIC ---------------- */
uint64_t ___GCD_UI64___(uint64_t a, uint64_t b) {}
void __BIGINT_MAGNITUDED_GCD_UI64__(uint64_t *res, const bigInt *a, uint64_t val) {}
void __BIGINT_MAGNITUDED_LCM_UI64__(bigInt *res, const bigInt *a, uint64_t val) {}
void __BIGINT_MAGNITUDED_GCD__(bigInt *res, const bigInt *a, const bigInt *b) {}
void __BIGINT_MAGNITUDED_LCM__(bigInt *res, const bigInt *a, const bigInt *b) {}
void __BIGINT_MAGNITUDED_EUCMOD_UI64__(uint64_t *res, const bigInt *a, uint64_t modulus) {}
void __BIGINT_MAGNITUDED_EUCMOD__(bigInt *res, const bigInt *a, const bigInt *modulus) {}
void __BIGINT_MAGNITUDED_PRIMATEST__(const bigInt *x) {}
/* ----------------- MAGNITUDED MODULAR-ARITHMETIC ------------------ */
void __BIGINT_MAGNITUDED_MODADD__(bigInt *res, const bigInt *a, const bigInt *b, const bigInt *mod) {}
void __BIGINT_MAGNITUDED_MODSUB__(bigInt *res, const bigInt *a, const bigInt *b, const bigInt *mod) {}
void __BIGINT_MAGNITUDED_MODMUL__(bigInt *res, const bigInt *a, const bigInt *b, const bigInt *mod) {}
void __BIGINT_MAGNITUDED_MODDIV__(bigInt *res, const bigInt *a, const bigInt *b, const bigInt *mod) {}
void __BIGINT_MAGNITUDED_MODEXP__(bigInt *res, const bigInt *a, const bigInt *b, const bigInt *mod) {}
void __BIGINT_MAGNITUDED_MODSQR__(bigInt *res, const bigInt *a, const bigInt *b, const bigInt *mod) {}
void __BIGINT_MAGNITUDED_MODINV__(bigInt *res, const bigInt *a, const bigInt *b, const bigInt *mod) {}





//* ============================================ SIGNED ARITHMETIC ========================================== */
/* ------------------- MUTATIVE ARITHMETIC -------------------- */
void __BIGINT_MUT_MUL_UI64__(bigInt *x, uint64_t val) {
    assert(__BIGINT_PVALIDATE__(x));
    static local_thread dnml_arena _MASTER_ARENA; 
    init_arena(&_MASTER_ARENA, BYTES_IN_UINT64_T * (x->n + 1));

    if (x->n == 0);
    else if (val == 1);
    else if (!val) __BIGINT_RESET__(x);
    else if (x->n == 1 && x->limbs[0] == 1) __BIGINT_MUT_COPY_UI64__(x, val);
    else {
        __BIGINT_RESERVE__(x, x->n + 1); // Prevents memory leak of __TEMP_PROD__ on OOM inside MUT_COPY
        size_t tmp_mark = arena_mark(&_MASTER_ARENA);
        limb_t *tmp_limbs = arena_alloc(&_MASTER_ARENA, BYTES_IN_UINT64_T * (x->n + 1));
        bigInt __TEMP_PROD__ = {
            .limbs  = tmp_limbs,
            .cap    = x->n + 1,
            .n      = 0,
            .sign   = 1
        };

        __BIGINT_MAGNITUDED_MUL_UI64__(&__TEMP_PROD__, x, val);
        __BIGINT_MUT_COPY__(x, __TEMP_PROD__);
        arena_reset(&_MASTER_ARENA, tmp_mark);
    }
}
bigint_status __BIGINT_MUT_DIV_UI64__(bigInt *x, uint64_t val) {
    assert(__BIGINT_PVALIDATE__(x));
    if (!val) return BIGINT_ERR_DOMAIN;
    static local_thread dnml_arena _MASTER_ARENA; 
    init_arena(&_MASTER_ARENA, BYTES_IN_UINT64_T * x->n);

    if (x->n == 0);
    else if (val == 1);
    else if (x->n == 1 && x->limbs[0] == 1) __BIGINT_RESET__(x);
    else {
        size_t tmp_mark = arena_mark(&_MASTER_ARENA);
        limb_t *tmp_limbs = arena_alloc(&_MASTER_ARENA, BYTES_IN_UINT64_T * x->n);
        bigInt temp_quot = { 
            .limbs = tmp_limbs, 
            .cap = x->n, 
            .n = 0, 
            .sign = 1 
        }; uint64_t temp_rem;

        __BIGINT_MAGNITUDED_DIVMOD_UI64__(&temp_quot, &temp_rem, x, val);
        temp_quot.sign = x->sign;
        __BIGINT_NORMALIZE__(&temp_quot);
        __BIGINT_MUT_COPY__(x, temp_quot);
        arena_reset(&_MASTER_ARENA, tmp_mark);
    } return BIGINT_SUCCESS;
}
bigint_status __BIGINT_MUT_MOD_UI64__(bigInt *x, uint64_t val) {
    assert(__BIGINT_PVALIDATE__(x));
    if (!val) return BIGINT_ERR_DOMAIN;
    static local_thread dnml_arena _MASTER_ARENA; 
    init_arena(&_MASTER_ARENA, BYTES_IN_UINT64_T * x->n);

    if (x->n == 0);
    else if (val == 1) __BIGINT_RESET__(x);
    else {
        int8_t comp_res = __BIGINT_COMPARE_MAGNITUDE_UI64__(x, val);
        if (comp_res < 0);
        else if (!comp_res) __BIGINT_RESET__(x);
        else {
            size_t tmp_mark = arena_mark(&_MASTER_ARENA);
            limb_t *tmp_limbs = arena_alloc(&_MASTER_ARENA, BYTES_IN_UINT64_T * x->n);
            bigInt temp_quot = {
                .limbs = tmp_limbs,
                .cap   = x->n,
                .n     = 0,
                .sign  = 1
            }; uint64_t temp_rem;

            __BIGINT_MAGNITUDED_DIVMOD_UI64__(&temp_quot, &temp_rem, x, val);
            x->limbs[0] = temp_rem;
            x->n        = (temp_rem) ? 1 : 0;
            x->sign     = (temp_rem) ? x->sign : 1;
            arena_reset(&_MASTER_ARENA, tmp_mark);
        }
    } return BIGINT_SUCCESS;
}
void __BIGINT_MUT_MUL_I64__(bigInt *x, int64_t val) {
    assert(__BIGINT_PVALIDATE__(x));
    static local_thread dnml_arena _MASTER_ARENA; 
    init_arena(&_MASTER_ARENA, BYTES_IN_UINT64_T * (x->n + 1));

    if (x->n == 0) return;
    if (!val) __BIGINT_RESET__(x);
    else if (val == 1 || val == -1);
    else if (x->n == 1 && x->limbs[0] == 1) __BIGINT_MUT_COPY_I64__(x, val);
    else {
        __BIGINT_RESERVE__(x, x->n + 1);
        size_t tmp_mark = arena_mark(&_MASTER_ARENA);
        limb_t *tmp_limbs = arena_alloc(&_MASTER_ARENA, BYTES_IN_UINT64_T * (x->n + 1));
        bigInt __TEMP_PROD__ = {
            .limbs  = tmp_limbs,
            .cap    = x->n + 1,
            .n      = 0,
            .sign   = 1
        }; uint64_t mag_val = __MAG_I64__(val);

        __BIGINT_MAGNITUDED_MUL_UI64__(&__TEMP_PROD__, x, mag_val);
        __BIGINT_MUT_COPY__(x, __TEMP_PROD__);
        arena_reset(&_MASTER_ARENA, tmp_mark);
    } 
    x->sign *= (val < 0) ? -1 : 1;
}
bigint_status __BIGINT_MUT_DIV_I64__(bigInt *x, int64_t val) {
    assert(__BIGINT_PVALIDATE__(x));
    if (!val) return BIGINT_ERR_DOMAIN;
    static local_thread dnml_arena _MASTER_ARENA; 
    init_arena(&_MASTER_ARENA, BYTES_IN_UINT64_T * x->n);

    if (x->n == 0);
    else if (val == 1 || val == -1) x->sign *= val;
    else if (x->n == 1 && x->limbs[0] == 1) __BIGINT_RESET__(x);
    else {
        size_t tmp_mark = arena_mark(&_MASTER_ARENA);
        limb_t *tmp_limbs = arena_alloc(&_MASTER_ARENA, BYTES_IN_UINT64_T * x->n);
        bigInt temp_quot = {
            .limbs  = tmp_limbs,
            .cap    = x->n,
            .n      = 0,
            .sign   = 1
        }; uint64_t temp_rem; 
        uint64_t mag_val = __MAG_I64__(val);

        __BIGINT_MAGNITUDED_DIVMOD_UI64__(&temp_quot, &temp_rem, x, mag_val);
        temp_quot.sign = x->sign * ((val < 0) ? -1 : 1);
        __BIGINT_NORMALIZE__(&temp_quot);
        __BIGINT_MUT_COPY__(x, temp_quot);
        arena_reset(&_MASTER_ARENA, tmp_mark);
    } return BIGINT_SUCCESS;
}
bigint_status __BIGINT_MUT_MOD_I64__(bigInt *x, int64_t val) {
    assert(__BIGINT_PVALIDATE__(x) && val);
    if (!val) return BIGINT_ERR_DOMAIN;
    static local_thread dnml_arena _MASTER_ARENA; 
    init_arena(&_MASTER_ARENA, BYTES_IN_UINT64_T * x->n);

    if (x->n == 0);
    else if (val == 1 || val == -1) __BIGINT_RESET__(x);
    else {
        uint64_t mag_val = __MAG_I64__(val);
        int8_t comp_res = __BIGINT_COMPARE_MAGNITUDE_UI64__(x, mag_val);
        if (comp_res < 0);
        else if (!comp_res) __BIGINT_RESET__(x);
        else {
            size_t tmp_mark = arena_mark(&_MASTER_ARENA);
            limb_t *tmp_limbs = arena_alloc(&_MASTER_ARENA, BYTES_IN_UINT64_T * x->n);
            bigInt temp_quot = {
                .limbs  = tmp_limbs,
                .cap    = x->n,
                .n      = 0,
                .sign   = 1
            }; uint64_t temp_rem; 

            __BIGINT_MAGNITUDED_DIVMOD_UI64__(&temp_quot, &temp_rem, x, mag_val);
            x->limbs[0] = temp_rem;
            x->n        = (temp_rem) ? 1 : 0;
            x->sign     = (temp_rem) ? x->sign : 1;
            arena_reset(&_MASTER_ARENA, tmp_mark);
        }
    } return BIGINT_SUCCESS;
}
void __BIGINT_MUT_ADD__(bigInt *x, const bigInt y) {
    assert(__BIGINT_PVALIDATE__(x) && __BIGINT_VALIDATE__(y));
    static local_thread dnml_arena _MASTER_ARENA;
    init_arena(&_MASTER_ARENA, BYTES_IN_UINT64_T * (max(x->n, y.n) + 1));

    if (!y.n);
    else if (!x->n) __BIGINT_MUT_COPY__(x, y);
    else if (x->sign == y.sign) {
        __BIGINT_RESERVE__(x, max(x->n, y.n) + 1); // Prevents memory leaked from OOM early
        size_t tmp_mark = arena_mark(&_MASTER_ARENA);
        limb_t *tmp_limbs = arena_alloc(&_MASTER_ARENA, BYTES_IN_UINT64_T * (max(x->n, y.n) + 1));
        bigInt temp_sum = {
            .limbs = tmp_limbs,
            .cap   = max(x->n, y.n) + 1,
            .n     = 0,
            .sign  = 1
        };

        __BIGINT_MAGNITUDED_ADD__(&temp_sum, x, &y);
        temp_sum.sign = x->sign;
        __BIGINT_MUT_COPY__(x, temp_sum);
        arena_reset(&_MASTER_ARENA, tmp_mark);
    } else {
        int8_t comp_res = __BIGINT_COMPARE_MAGNITUDE__(x, &y);
        if (!comp_res) __BIGINT_RESET__(x);
        else {
            size_t tmp_mark = arena_mark(&_MASTER_ARENA);
            limb_t *tmp_limbs = arena_alloc(&_MASTER_ARENA, BYTES_IN_UINT64_T * x->n);
            bigInt temp_sum = {
                .limbs = tmp_limbs,
                .cap   = x->n,
                .n     = 0,
                .sign  = 1
            };
            if (comp_res > 0)   { __BIGINT_MAGNITUDED_SUB__(&temp_sum, x, &y); temp_sum.sign = x->sign; }
            else                { __BIGINT_MAGNITUDED_SUB__(&temp_sum, x, &y); temp_sum.sign = y.sign; }
            __BIGINT_MUT_COPY__(x, temp_sum);
            arena_reset(&_MASTER_ARENA, tmp_mark);
        }
    }
}
void __BIGINT_MUT_SUB__(bigInt *x, const bigInt y) {
    assert(__BIGINT_PVALIDATE__(x) && __BIGINT_VALIDATE__(y));
    static local_thread dnml_arena _MASTER_ARENA;
    init_arena(&_MASTER_ARENA, BYTES_IN_UINT64_T * (max(x->n, y.n) + 1));

    if (!y.n);
    else if (!x->n) { __BIGINT_MUT_COPY__(x, y);  x->sign = -y.sign; }
    else if (x->sign == y.sign) {
        int8_t comp_res = __BIGINT_COMPARE_MAGNITUDE__(x, &y);
        if (!comp_res) __BIGINT_RESET__(x);
        else {
            size_t tmp_mark = arena_mark(&_MASTER_ARENA);
            limb_t *tmp_limbs = arena_alloc(&_MASTER_ARENA, BYTES_IN_UINT64_T * x->n);
            bigInt temp_diff = {
                .limbs = tmp_limbs, 
                .cap   = x->n,
                .n     = 0,
                .sign  = 1
            };
            if (comp_res > 0)   { __BIGINT_MAGNITUDED_SUB__(&temp_diff, x, &y); temp_diff.sign = x->sign; }
            else                { __BIGINT_MAGNITUDED_SUB__(&temp_diff, x,  &y); temp_diff.sign = -x->sign; }
            __BIGINT_MUT_COPY__(x, temp_diff);
            arena_reset(&_MASTER_ARENA, tmp_mark);
        }
    } else {
        size_t tmp_mark = arena_mark(&_MASTER_ARENA);
        limb_t *tmp_limbs = arena_alloc(&_MASTER_ARENA, BYTES_IN_UINT64_T * (max(x->n, y.n) + 1));
        bigInt temp_diff = {
            .limbs = tmp_limbs, 
            .cap   = max(x->n, y.n) + 1,
            .n     = 0,
            .sign  = 1
        };
        __BIGINT_MAGNITUDED_ADD__(&temp_diff, x, &y);
        temp_diff.sign = x->sign;
        __BIGINT_MUT_COPY__(x, temp_diff);
        arena_reset(&_MASTER_ARENA, tmp_mark);
    }
}
void __BIGINT_MUT_MUL__(bigInt *x, const bigInt y) {
    assert(__BIGINT_PVALIDATE__(x) && __BIGINT_VALIDATE__(y));
    static local_thread dnml_arena _MASTER_ARENA;
    init_arena(&_MASTER_ARENA, BYTES_IN_UINT64_T * (x->n * y.n));

    if (x->n == 0) return;
    if (!y.n) __BIGINT_RESET__(x);
    else if (x->n == 1 && x->limbs[0] == 1) __BIGINT_MUT_COPY__(x, y);
    else {
        size_t tmp_mark = arena_mark(&_MASTER_ARENA);
        limb_t *tmp_limbs = arena_alloc(&_MASTER_ARENA, BYTES_IN_UINT64_T * (x->n * y.n));
        bigInt __TEMP_PROD__ = {
            .limbs = tmp_limbs,
            .cap   = x->n * y.n,
            .n     = 0,
            .sign  = 1
        };
        __BIGINT_MAGNITUDED_MUL__(&__TEMP_PROD__, x, &y);
        __BIGINT_MUT_COPY__(x, __TEMP_PROD__);
        arena_reset(&_MASTER_ARENA, tmp_mark);
    }
    x->sign *= y.sign;
}
bigint_status __BIGINT_MUT_DIV__(bigInt *x, const bigInt y) {
    assert(__BIGINT_PVALIDATE__(x) && __BIGINT_VALIDATE__(y));
    if (!y.n) return BIGINT_ERR_DOMAIN;
    static local_thread dnml_arena _MASTER_ARENA;
    init_arena(&_MASTER_ARENA, BYTES_IN_UINT64_T * (x->n + y.n));

    if (x->n == 0);
    else if (y.n == 1 && y.limbs[0] == 1) x->sign *= y.sign;
    else if (x->n == 1 && x->limbs[0] == 1) __BIGINT_RESET__(x);
    else {
        size_t quot_mark = arena_mark(&_MASTER_ARENA), rem_mark = arena_mark(&_MASTER_ARENA);
        limb_t *quot_limbs = arena_alloc(&_MASTER_ARENA, BYTES_IN_UINT64_T * x->n);
        limb_t *rem_limbs = arena_alloc(&_MASTER_ARENA, BYTES_IN_UINT64_T * y.n);
        bigInt temp_quot = {
            .limbs = quot_limbs,
            .cap   = x->n,
            .n     = 0,
            .sign  = 1
        }; 
        bigInt temp_rem = {
            .limbs = rem_limbs,
            .cap   = y.n,
            .n     = 0,
            .sign  = 1
        };

        __BIGINT_MAGNITUDED_DIVMOD__(&temp_quot, &temp_rem, x, &y);
        temp_quot.sign = x->sign * y.sign;
        __BIGINT_NORMALIZE__(&temp_quot);
        __BIGINT_MUT_COPY__(x, temp_quot);
        arena_reset(&_MASTER_ARENA, rem_mark); arena_reset(&_MASTER_ARENA, quot_mark);
    } return BIGINT_SUCCESS;
}
bigint_status __BIGINT_MUT_MOD__(bigInt *x, const bigInt y) {
    assert(__BIGINT_PVALIDATE__(x) && __BIGINT_VALIDATE__(y));
    if (!y.n) return BIGINT_ERR_DOMAIN;
    static local_thread dnml_arena _MASTER_ARENA;
    init_arena(&_MASTER_ARENA, BYTES_IN_UINT64_T * (x->n + y.n));

    if (x->n == 0);
    else if (y.n == 1 && y.limbs[0] == 1) __BIGINT_RESET__(x);
    else {
        int8_t comp_res = __BIGINT_COMPARE_MAGNITUDE__(x, &y);
        if (comp_res < 0);
        else if (!comp_res) __BIGINT_RESET__(x);
        else {
            size_t quot_mark = arena_mark(&_MASTER_ARENA), rem_mark = arena_mark(&_MASTER_ARENA);
            limb_t *quot_limbs = arena_alloc(&_MASTER_ARENA, BYTES_IN_UINT64_T * x->n);
            limb_t *rem_limbs = arena_alloc(&_MASTER_ARENA, BYTES_IN_UINT64_T * y.n);
            bigInt temp_quot = {
                .limbs = quot_limbs,
                .cap   = x->n,
                .n     = 0,
                .sign  = 1
            }; 
            bigInt temp_rem = {
                .limbs = rem_limbs,
                .cap   = y.n,
                .n     = 0,
                .sign  = 1
            };

            __BIGINT_MAGNITUDED_DIVMOD__(&temp_quot, &temp_rem, x, &y);
            temp_rem.sign = x->sign;
            __BIGINT_MUT_COPY__(x, temp_rem);
            arena_reset(&_MASTER_ARENA, rem_mark); 
            arena_reset(&_MASTER_ARENA, quot_mark);
        }
    } return BIGINT_SUCCESS;
}
/* ------------------ FUNCTIONAL ARITHMETIC ------------------- */
bigInt __BIGINT_MUL_UI64__(const bigInt x, uint64_t val) {
    assert(__BIGINT_VALIDATE__(x));
    bigInt res;
    if (!x.n || !val) __BIGINT_EMPTY_INIT__(&res);
    else if (x.n == 1 && x.limbs[0] == 1) __BIGINT_UI64_INIT__(&res, val);
    else if (val == 1) __BIGINT_STANDARD_INIT__(&res, x);
    else { // Standard Case
        __BIGINT_EMPTY_INIT__(&res);
        __BIGINT_MAGNITUDED_MUL_UI64__(&res, &x, val);
    }
    res.sign = x.sign;
    return res;
}
bigInt __BIGINT_DIV_UI64__(const bigInt x, uint64_t val, bigint_status *err) {
    assert(__BIGINT_VALIDATE__(x) && err);
    if (!val) { *err = BIGINT_ERR_DOMAIN; return __BIGINT_ERROR_VALUE__(); }

    bigInt quot;
    if (x.n == 0) __BIGINT_EMPTY_INIT__(&quot);
    else if (val == 1) __BIGINT_STANDARD_INIT__(&quot, x);
    else if (x.n == 1 && x.limbs[0]) __BIGINT_EMPTY_INIT__(&quot);
    else {
        uint64_t temp_rem; __BIGINT_EMPTY_INIT__(&quot);
        __BIGINT_MAGNITUDED_DIVMOD_UI64__(&quot, &temp_rem, &x, val);
        quot.sign = x.sign;
        __BIGINT_NORMALIZE__(&quot);
    } 
    *err = BIGINT_SUCCESS;
    return quot;
}
bigInt __BIGINT_MOD_UI64__(const bigInt x, uint64_t val, bigint_status *err) {      //* ARENA REFACTORED
    assert(__BIGINT_VALIDATE__(x) && err);
    if (!val) { *err = BIGINT_ERR_DOMAIN; return __BIGINT_ERROR_VALUE__(); }
    static local_thread dnml_arena _MASTER_ARENA;
    init_arena(&_MASTER_ARENA, x.n); // Stores the temp_quots

    bigInt rem;
    if (x.n == 0 || val == 1) __BIGINT_EMPTY_INIT__(&rem);
    else {
        int8_t comp_res = __BIGINT_COMPARE_MAGNITUDE_UI64__(&x, val);
        if (comp_res < 0) __BIGINT_STANDARD_INIT__(&rem, x);
        else if (!comp_res) __BIGINT_EMPTY_INIT__(&rem);
        else {
            __BIGINT_EMPTY_INIT__(&rem);
            size_t tmp_mark = arena_mark(&_MASTER_ARENA);
            limb_t *tmp_limbs = arena_alloc(&_MASTER_ARENA, x.n);
            bigInt temp_quot = {
                .limbs = tmp_limbs,
                .cap   = x.n,
                .n     = 0,
                .sign  = 1
            }; uint64_t temp_rem;
            __BIGINT_MAGNITUDED_DIVMOD_UI64__(&temp_quot, &temp_rem, &x, val);
            rem.limbs[0] = temp_rem;
            rem.n        = (temp_rem) ? 1 : 0;
            rem.sign     = (temp_rem) ? x.sign : 1;
            arena_reset(&_MASTER_ARENA, tmp_mark);
        }
    } 
    *err = BIGINT_SUCCESS;
    return rem;
}
bigInt __BIGINT_MUL_I64__(const bigInt x, int64_t val) {
    assert(__BIGINT_VALIDATE__(x));
    bigInt res;
    if (!x.n || !val) __BIGINT_EMPTY_INIT__(&res);
    else if (x.n == 1 && x.limbs[0] == 1) __BIGINT_I64_INIT__(&res, val);
    else if (val == 1 || val == -1) { __BIGINT_STANDARD_INIT__(&res, x); res.sign = x.sign * val; }
    else {
        uint64_t mag_val = __MAG_I64__(val);
        __BIGINT_EMPTY_INIT__(&res);
        __BIGINT_MAGNITUDED_MUL_UI64__(&res, &x, mag_val);
    }
    res.sign = x.sign * ((val < 0) ? -1 : 1);
    return res;
}
bigInt __BIGINT_DIV_I64__(const bigInt x, int64_t val, bigint_status *err) {
    assert(__BIGINT_VALIDATE__(x) && err);
    if (!val) { *err = BIGINT_ERR_DOMAIN; return __BIGINT_ERROR_VALUE__(); }

    bigInt quot;
    if (x.n == 0) __BIGINT_EMPTY_INIT__(&quot);
    else if (val == 1 || val == -1) { __BIGINT_STANDARD_INIT__(&quot, x); quot.sign = x.sign * val; }
    // Idiomatic C-style Integer Division
    else if (x.n == 1 && x.limbs[0]) __BIGINT_EMPTY_INIT__(&quot);
    else {
        uint64_t temp_rem, mag_val = __MAG_I64__(val);
        __BIGINT_EMPTY_INIT__(&quot);
        __BIGINT_MAGNITUDED_DIVMOD_UI64__(&quot, &temp_rem, &x, mag_val);
        quot.sign = x.sign * ((val < 0) ? -1 : 1);
        __BIGINT_NORMALIZE__(&quot);
    }
    *err = BIGINT_SUCCESS;
    return quot;
}
bigInt __BIGINT_MOD_I64__(const bigInt x, int64_t val, bigint_status *err) {        //? ARENA REFACTORED
    assert(__BIGINT_VALIDATE__(x) && err);
    if (!val) { *err = BIGINT_ERR_DOMAIN; return __BIGINT_ERROR_VALUE__(); }
    static local_thread dnml_arena _MASTER_ARENA;
    init_arena(&_MASTER_ARENA, x.n);

    bigInt rem;
    if (x.n == 0 || val == 1 || val == -1) __BIGINT_EMPTY_INIT__(&rem);
    else {
        uint64_t mag_val = __MAG_I64__(val);
        int8_t comp_res = __BIGINT_COMPARE_MAGNITUDE_UI64__(&x, mag_val);
        if (comp_res < 0) __BIGINT_STANDARD_INIT__(&rem, x);
        else if (!comp_res) __BIGINT_EMPTY_INIT__(&rem);
        else {
            __BIGINT_EMPTY_INIT__(&rem);
            size_t tmp_mark = arena_mark(&_MASTER_ARENA);
            limb_t *tmp_limbs = arena_alloc(&_MASTER_ARENA, x.n);
            bigInt temp_quot = {
                .limbs = tmp_limbs,
                .cap   = x.n,
                .n     = 0,
                .sign  = 1
            }; uint64_t temp_rem;
            __BIGINT_MAGNITUDED_DIVMOD_UI64__(&temp_quot, &temp_rem, &x, mag_val);
            rem.limbs[0] = temp_rem;
            rem.n        = (temp_rem) ? 1 : 0;
            rem.sign     = x.sign;
            arena_reset(&_MASTER_ARENA, tmp_mark);
        }
    }
    *err = BIGINT_SUCCESS;
    return rem;
}
bigInt __BIGINT_ADD__(const bigInt x, const bigInt y) {
    assert(__BIGINT_VALIDATE__(x) && __BIGINT_VALIDATE__(y));
    bigInt sum;
    if (!y.n) __BIGINT_STANDARD_INIT__(&sum, x);
    else if (!x.n) __BIGINT_STANDARD_INIT__(&sum, y);
    else if (x.sign == y.sign) {
        __BIGINT_EMPTY_INIT__(&sum);
        __BIGINT_MAGNITUDED_ADD__(&sum, &x, &y);
        sum.sign = x.sign;
    } else {
        int8_t comp_res = __BIGINT_COMPARE_MAGNITUDE__(&x, &y);
        __BIGINT_EMPTY_INIT__(&sum);
        if      (comp_res > 0) { __BIGINT_MAGNITUDED_SUB__(&sum, &x, &y); sum.sign = x.sign; }
        else if (comp_res < 0) { __BIGINT_MAGNITUDED_SUB__(&sum, &y, &x); sum.sign = y.sign; }
    } return sum;
}
bigInt __BIGINT_SUB__(const bigInt x, const bigInt y) {
    assert(__BIGINT_VALIDATE__(x) && __BIGINT_VALIDATE__(y));
    bigInt diff;
    if (!y.n) __BIGINT_STANDARD_INIT__(&diff, x);
    else if (!x.n) { __BIGINT_STANDARD_INIT__(&diff, y); diff.sign = -y.sign; }
    else if (x.sign == y.sign) {
        int8_t comp_res = __BIGINT_COMPARE_MAGNITUDE__(&x, &y);
        __BIGINT_EMPTY_INIT__(&diff);
        if      (comp_res > 0) { __BIGINT_MAGNITUDED_SUB__(&diff, &x, &y); diff.sign =  x.sign; }
        else if (comp_res < 0) { __BIGINT_MAGNITUDED_SUB__(&diff, &y, &x); diff.sign = -x.sign; }
    } else {
        __BIGINT_EMPTY_INIT__(&diff);
        __BIGINT_MAGNITUDED_ADD__(&diff, &x, &y);
        diff.sign = x.sign;
    } return diff;
}
bigInt __BIGINT_MUL__(const bigInt x, const bigInt y) {
    assert(__BIGINT_VALIDATE__(x) && __BIGINT_VALIDATE__(y));
    bigInt res;
    if (!x.n || !y.n) __BIGINT_EMPTY_INIT__(&res);
    else if (x.n == 1 && x.limbs[0] == 1) __BIGINT_STANDARD_INIT__(&res, y);
    else if (y.n == 1 && y.limbs[0] == 1) __BIGINT_STANDARD_INIT__(&res, x);
    else {
        __BIGINT_EMPTY_INIT__(&res);
        __BIGINT_MAGNITUDED_MUL__(&res, &x, &y);
    }
    res.sign = x.sign * y.sign;
    return res;
}
bigInt __BIGINT_DIV__(const bigInt x, const bigInt y, bigint_status *err) {         //todo ARENA REFACTORED
    assert(__BIGINT_VALIDATE__(x) && __BIGINT_VALIDATE__(y) && err);
    if (!y.n) { *err = BIGINT_ERR_DOMAIN; return __BIGINT_ERROR_VALUE__(); }
    static local_thread dnml_arena _MASTER_ARENA;
    init_arena(&_MASTER_ARENA, y.n);

    bigInt quot;
    if (x.n == 0) __BIGINT_EMPTY_INIT__(&quot);
    else if (y.n == 1 && y.limbs[0] == 1) { __BIGINT_STANDARD_INIT__(&quot, x); quot.sign *= y.sign; }
    else if (x.n == 1 && x.limbs[0] == 1) __BIGINT_EMPTY_INIT__(&quot);
    else {
        __BIGINT_LIMBS_INIT__(&quot, x.n);
        size_t tmp_mark = arena_mark(&_MASTER_ARENA);
        limb_t *tmp_limbs = arena_alloc(&_MASTER_ARENA, y.n);
        bigInt temp_rem = {
            .limbs = tmp_limbs,
            .cap   = y.n,
            .n     = 0,
            .sign  = 1
        };
        __BIGINT_MAGNITUDED_DIVMOD__(&quot, &temp_rem, &x, &y);
        quot.sign = x.sign * y.sign;
        __BIGINT_NORMALIZE__(&quot);
        arena_reset(&_MASTER_ARENA, tmp_mark);
    }
    *err = BIGINT_SUCCESS; 
    return quot;
}
bigInt __BIGINT_MOD__(const bigInt x, const bigInt y, bigint_status *err) {         //todo ARENA REFACTORED
    assert(__BIGINT_VALIDATE__(x) && __BIGINT_VALIDATE__(y) && err);
    if (!y.n) { *err = BIGINT_ERR_DOMAIN; return __BIGINT_ERROR_VALUE__(); }
    static local_thread dnml_arena _MASTER_ARENA;
    init_arena(&_MASTER_ARENA, x.n);

    bigInt rem;
    if (x.n == 0) __BIGINT_EMPTY_INIT__(&rem);
    else if (y.n == 1 && y.limbs[0] == 1) __BIGINT_EMPTY_INIT__(&rem);
    else {
        int8_t comp_res = __BIGINT_COMPARE_MAGNITUDE__(&x, &y);
        if (comp_res < 0) __BIGINT_STANDARD_INIT__(&rem, x);
        else if (!comp_res) __BIGINT_EMPTY_INIT__(&rem);
        else {
            __BIGINT_LIMBS_INIT__(&rem, y.n);
            size_t tmp_mark = arena_mark(&_MASTER_ARENA);
            limb_t *tmp_limbs = arena_alloc(&_MASTER_ARENA, x.n);
            bigInt temp_quot = {
                .limbs = tmp_limbs,
                .cap   = x.n,
                .n     = 0,
                .sign  = 1
            };
            __BIGINT_MAGNITUDED_DIVMOD__(&temp_quot, &rem, &x, &y);
            rem.sign = x.sign;
            __BIGINT_NORMALIZE__(&rem);
            arena_reset(&_MASTER_ARENA, tmp_mark);
        }
    }
    *err = BIGINT_SUCCESS;
    return rem;
}





//* ======================================== SIGNED NUMBER THEORETIC ========================================= */
/* -------------- Pure Number Theoretic -------------- */
uint64_t __BIGINT_GCD_UI64__(const bigInt x, uint64_t val) {}
int64_t __BIGINT_GCD_I64__(const bigInt x, int64_t val) {}
bigInt __BIGINT_GCD__(const bigInt x, const bigInt y) {}
bigInt __BIGINT_LCM_UI64__(const bigInt x, uint64_t val) {}
bigInt __BIGINT_LCM_I64__(const bigInt x, int64_t val) {}
bigInt __BIGINT_LCM__(const bigInt x, const bigInt y) {}
uint8_t __BIGINT_IS_PRIME__(const bigInt x) {}
/* ---------------- Modular Reduction ---------------- */
void __BIGINT_MUT_MODULO_UI64__(bigInt *x, uint64_t modulus) {}
void __BIGINT_MUT_MODULO_I64__(bigInt *x, int64_t modulus) {}
void __BIGINT_MUT_MODULO__(bigInt *x, const bigInt modulus) {}
uint64_t __BIGINT_MODULO_UI64__(const bigInt x, uint64_t modulus) {}
int64_t __BIGINT_MODULO_I64__(const bigInt x, int64_t modulus) {}
bigInt __BIGINT_MODULO__(const bigInt x, const bigInt modulus) {}
/* ---------------- SMALL Modular Arithmetic --------------- */
void __BIGINT_MUT_MODADD_UI64__(bigInt *x, const bigInt y, uint64_t modulus) {}
void __BIGINT_MUT_MODSUB_UI64__(bigInt *x, const bigInt y, uint64_t modulus) {}
void __BIGINT_MUT_MODADD__(bigInt *x, const bigInt y, const bigInt modulus) {}
void __BIGINT_MUT_MODSUB__(bigInt *x, const bigInt y, const bigInt modulus) {}
uint64_t __BIGINT_MODADD_UI64__(const bigInt x, const bigInt y, uint64_t modulus) {}
uint64_t __BIGINT_MODSUB_UI64__(const bigInt x, const bigInt y, uint64_t modulus) {}
bigInt __BIGINT_MODADD__(const bigInt x, const bigInt y, const bigInt modulus) {}
bigInt __BIGINT_MODSUB__(const bigInt x, const bigInt y, const bigInt modulus) {}
/* ---------------- LARGE Modular Arithmetic --------------- */
void __BIGINT_MUT_MODMUL_UI64_UI64__(bigInt *x, uint64_t y, uint64_t modulus) {}
void __BIGINT_MUT_MODDIV_UI64_UI64__(bigInt *x, uint64_t y, uint64_t modulus) {}
void __BIGINT_MUT_MODMUL_BI_UI64__(bigInt *x, const bigInt y, uint64_t modulus) {}
void __BIGINT_MUT_MODDIV_BI_UI64__(bigInt *x, const bigInt y, uint64_t modulus) {}
void __BIGINT_MUT_MODMUL_UI64_BI__(bigInt *x, uint64_t y, const bigInt modulus) {}
void __BIGINT_MUT_MODDIV_UI64_BI__(bigInt *x, uint64_t y, const bigInt modulus) {}
void __BIGINT_MUT_MODMUL__(bigInt *x, const bigInt y, const bigInt modulus) {}
void __BIGINT_MUT_MODDIV__(bigInt *x, const bigInt y, const bigInt modulus) {}
uint64_t __BIGINT_MODMUL_UI64_UI64__(const bigInt x, uint64_t y, uint64_t modulus) {}
uint64_t __BIGINT_MODDIV_UI64_UI64__(const bigInt x, uint64_t y, uint64_t modulus) {}
uint64_t __BIGINT_MODMUL_BI_UI64__(const bigInt x, const bigInt y, uint64_t modulus) {}
uint64_t __BIGINT_MODDIV_BI_UI64__(const bigInt x, const bigInt y, uint64_t modulus) {}
bigInt __BIGINT_MODMUL_UI64_BI__(const bigInt x, uint64_t y, const bigInt modulus) {}
bigInt __BIGINT_MODDIV_UI64_BI__(const bigInt x, uint64_t y, const bigInt modulus) {}
bigInt __BIGINT_MODMUL__(const bigInt x, const bigInt y, const bigInt modulus) {}
bigInt __BIGINT_MODDIV__(const bigInt x, const bigInt y, const bigInt modulus) {}
/* ---------------------- Modular Algebraic ------------------ */
void __BIGINT_MUT_MODEXP_UI64__(bigInt *x, const bigInt y, uint64_t modulus) {}
void __BIGINT_MUT_MODSQR_UI64__(bigInt *x, uint64_t modulus) {}
void __BIGINT_MUT_MODINV_UI64__(bigInt *x, uint64_t modulus) {}
void __BIGINT_MUT_MODEXP__(bigInt *x, const bigInt y, const bigInt modulus) {}
void __BIGINT_MUT_MODSQR__(bigInt *x, const bigInt modulus) {}
void __BIGINT_MUT_MODINV__(bigInt *x, const bigInt modulus) {}
uint64_t __BIGINT_MODEXP_UI64__(const bigInt x, const bigInt y, uint64_t modulus) {}
uint64_t __BIGINT_MODSQR_UI64__(const bigInt x, uint64_t modulus) {}
uint64_t __BIGINT_MODINV_UI64__(const bigInt x, uint64_t modulus) {}
bigInt __BIGINT_MODEXP__(const bigInt x, const bigInt y, const bigInt modulus) {}
bigInt __BIGINT_MODSQR__(const bigInt x, const bigInt modulus) {}
bigInt __BIGINT_MODINV__(const bigInt x, const bigInt modulus) {}





//* ================================================= COPIES ================================================= */
/*
todo 1) Handle overlapping memory in bigInt COPIES that uses MEMCPY
*/
/* -------------  Mutative SMALL Copies ------------- */
void __BIGINT_MUT_COPY_UI64__(bigInt *dst__, uint64_t source__) {
    assert(__BIGINT_MUTATIVE_SUBJECT_VALIDATE__(dst__));
    __BIGINT_CANONICALIZE__(dst__);
    if (dst__->n == 0 && !source__) return;
    if (dst__->n == 1 && dst__->limbs[0] == source__) return;
    dst__->limbs[0] = source__;
    dst__->n        = source__ ? 1 : 0;
    dst__->sign     = 1;
}
void __BIGINT_MUT_COPY_DEEP_UI64__(bigInt *dst__, uint64_t source__) {
    assert(__BIGINT_MUTATIVE_SUBJECT_VALIDATE__(dst__));
    __BIGINT_CANONICALIZE__(dst__);
    // Always reallocate and resize if dst__->cap is more than 1
    if (dst__->cap > 1) {
        uint64_t *__BUFFER_P = realloc(dst__->limbs, sizeof(uint64_t));
        if (__BUFFER_P == NULL) abort();
        dst__->limbs = __BUFFER_P;
        dst__->cap     = 1;
    }
    if (dst__->n == 0 && !source__) return;
    if (dst__->n == 1 && dst__->limbs[0] == source__) return;
    dst__->limbs[0] = source__;
    dst__->n        = source__ ? 1 : 0;
    dst__->sign     = 1;
}
void __BIGINT_MUT_COPY_I64__(bigInt *dst__, int64_t source__) {
    assert(__BIGINT_MUTATIVE_SUBJECT_VALIDATE__(dst__));
    __BIGINT_CANONICALIZE__(dst__);
    if (dst__->n == 0 && !source__) return;
    if (dst__->n == 1 && dst__->limbs[0] == __MAG_I64__(source__)) {
        dst__->sign = (source__ < 0) ? -1 : 1;
        return;
    }
    dst__->limbs[0] = __MAG_I64__(source__);
    dst__->n        = source__ ? 1 : 0;
    dst__->sign     = (source__< 0 ? -1 : 1);
}
void __BIGINT_MUT_COPY_DEEP_I64__(bigInt *dst__, int64_t source__) {
    assert(__BIGINT_MUTATIVE_SUBJECT_VALIDATE__(dst__));
    __BIGINT_CANONICALIZE__(dst__);
    // Always reallocate and resize if dst__->cap is more than 1
    if (dst__->cap > 1) {
        uint64_t *__BUFFER_P = realloc(dst__->limbs, sizeof(uint64_t));
        if (__BUFFER_P == NULL) abort();
        dst__->limbs = __BUFFER_P;
        dst__->cap     = 1;
    }
    if (dst__->n == 0 && !source__) return;
    if (dst__->n == 1 && dst__->limbs[0] == __MAG_I64__(source__)) {
        dst__->sign = (source__ < 0) ? -1 : 1;
        return;
    }
    dst__->limbs[0] = __MAG_I64__(source__);
    dst__->n        = source__ ? 1 : 0;
    dst__->sign     = (source__< 0 ? -1 : 1);
}
/* -------------  Mutative LARGE Copies ------------- */
void __BIGINT_MUT_COPY_LD__(bigInt *dst__, long double source__) {}
void __BIGINT_MUT_COPY_DEEP_LD__(bigInt *dst__, long double source__) {}
void __BIGINT_MUT_COPY_OVER_LD__(bigInt *dst__, long double source__) {}
void __BIGINT_MUT_COPY_TRUNCOVER_LD__(bigInt *dst__, long double source__) {}
void __BIGINT_MUT_COPY__(bigInt *dst__, bigInt source__) {
    assert(__BIGINT_STATE_VALIDATE__(source__));
    assert(__BIGINT_MUTATIVE_SUBJECT_VALIDATE__(dst__));
    __BIGINT_CANONICALIZE__(dst__); // Enforce contracts, ESPECAILLY Contract 3
    /* Fast Paths */
    // Since they're equal, and due to Contract 3
    //  ------> They're not subjected to resizing if these cases are true
    if (dst__->n == 0 && source__.n == 0) return ;
    if (dst__->n == source__.n && !memcmp(dst__->limbs, source__.limbs, source__.n)) {
        dst__->sign = source__.sign;
        return;
    }
    /* Standard Route */
    if (dst__->cap < source__.n) __BIGINT_RESERVE__(dst__, source__.n);
    memcpy(dst__->limbs, source__.limbs, source__.n);
    dst__->n    = source__.n;
    dst__->sign = source__.sign;
}
void __BIGINT_MUT_COPY_DEEP__(bigInt *dst__, bigInt source__) {
    assert(__BIGINT_STATE_VALIDATE__(source__));
    assert(__BIGINT_MUTATIVE_SUBJECT_VALIDATE__(dst__));
    __BIGINT_CANONICALIZE__(dst__); // Enforce contracts, ESPECAILLY Contract 3
    /* Reallocation and resize */
    if (dst__->cap != source__.n) {
        size_t size_to_change = source__.n;
        if (source__.n == 0) size_to_change = 1;
        __BIGINT_RESIZE__(dst__, size_to_change);
    }
    /* Fast Paths */
    // The equal fast path (dst__ != 0 && source__ != 0) is not here since
    // Reallocation and Resizing may tamper with the size metadata,
    //  -----> Tampering with the validity of memcmp()
    if (dst__->n == 0 && source__.n == 0) return;

    /* Standard Path */
    memcpy(dst__->limbs, source__.limbs, source__.n * sizeof(uint64_t));
    dst__->n    = source__.n;
    dst__->sign = source__.sign;
}
void __BIGINT_MUT_COPY_OVER__(bigInt *dst__, bigInt source__) {
    assert(__BIGINT_STATE_VALIDATE__(source__));
    assert(__BIGINT_MUTATIVE_SUBJECT_VALIDATE__(dst__));
    __BIGINT_CANONICALIZE__(dst__); // Enforce contracts, ESPECAILLY Contract 3
    /* Fast Paths */
    // Since they're equal, and due to Contract 3
    //  ------> They're not subjected to errors if these cases are true
    if (dst__->n == 0 && source__.n == 0) return;
    if (dst__->n == source__.n && !memcmp(dst__->limbs, source__.limbs, source__.n * sizeof(uint64_t))) {
        dst__->sign = source__.sign;
        return;
    }
    /* Standard Route */
    assert(dst__->cap < source__.n);
    memcpy(dst__->limbs, source__.limbs, source__.n * sizeof(uint64_t));
    dst__->n    = source__.n;
    dst__->sign = source__.sign;
}
void __BIGINT_MUT_COPY_TRUNCOVER__(bigInt *dst__, bigInt source__) {
    assert(__BIGINT_STATE_VALIDATE__(source__));
    assert(__BIGINT_MUTATIVE_SUBJECT_VALIDATE__(dst__));
    __BIGINT_CANONICALIZE__(dst__); // Enforce contracts, ESPECAILLY Contract 3
    /* Fast Paths */
    // Since they're equal, and due to Contract 3
    //  ------> They're not subjected to truncation if these cases are true
    if (dst__->n == 0 && source__.n == 0) return;
    if (dst__->n == source__.n && !memcmp(dst__->limbs, source__.limbs, source__.n * sizeof(uint64_t))) {
        dst__->sign = source__.sign;
        return;
    }
    /* Standard Route */
    size_t operation_range = source__.n;
    if (dst__->cap < source__.n) operation_range = dst__->cap; // Truncation (reducing operation range)
    memcpy(dst__->limbs, source__.limbs, operation_range * sizeof(uint64_t));
    dst__->n    = operation_range;
    dst__->sign = source__.sign;
}
/* -------------  Functional SMALL Copies ------------- */
bigInt __BIGINT_COPY_UI64__(uint64_t source__) {
    bigInt dst__; __BIGINT_EMPTY_INIT__(&dst__);
    if (source__) {
        dst__.limbs[0] = source__;
        dst__.n        = 1;
    }
    return dst__;
}
bigInt __BIGINT_COPY_I64__(int64_t source__) {
    bigInt dst__; __BIGINT_EMPTY_INIT__(&dst__);
    if (source__) {
        dst__.limbs[0] = __MAG_I64__(source__);
        dst__.n        = 1;
        dst__.sign     = (source__ < 0) ? -1 : 1;
    }
    return dst__;
}
/* -------------  Functional LARGE Copies ------------- */
bigInt __BIGINT_COPY_LD__(long double source__) {}
bigInt __BIGINT_COPY_OVER_LD__(long double source__, size_t output_cap) {}
bigInt __BIGINT_COPY_TRUNCOVER_LD__(long double source__, size_t output_cap) {}
bigInt __BIGINT_COPY__(const bigInt source__) {
    assert(__BIGINT_VALIDATE__(source__));
    bigInt dst__;
    if (source__.n == 0) {
        __BIGINT_EMPTY_INIT__(&dst__);
        return dst__;
    }
    __BIGINT_LIMBS_INIT__(&dst__, source__.n);
    memcpy(dst__.limbs, source__.limbs, source__.n * sizeof(uint64_t));
    dst__.n     = source__.n;
    dst__.sign  = source__.sign;
    return dst__;
}
bigInt __BIGINT_COPY_DEEP__(const bigInt source__) {
    assert(__BIGINT_STATE_VALIDATE__(source__));
    bigInt dst__;
    if (source__.n == 0) __BIGINT_EMPTY_INIT__(&dst__);
    else {
        __BIGINT_LIMBS_INIT__(&dst__, source__.n);
        memcpy(dst__.limbs, source__.limbs, source__.n * sizeof(uint64_t));
        dst__.n = source__.n;
    }
    dst__.sign = source__.sign;
    return dst__;
}
bigInt __BIGINT_COPY_OVER__(const bigInt source__, size_t output_cap, bigint_status *err) {
    assert(err);
    assert(__BIGINT_VALIDATE__(source__));
    if (output_cap < source__.n) {
        *err = BIGINT_ERR_RANGE;
        return __BIGINT_ERROR_VALUE__();
    }
    bigInt dst__;  
    __BIGINT_LIMBS_INIT__(&dst__, output_cap);
    memcpy(dst__.limbs, source__.limbs, source__.n * sizeof(uint64_t));
    dst__.n     = source__.n;
    dst__.sign  = source__.sign;
    *err = BIGINT_SUCCESS;
    return dst__;
}
bigInt __BIGINT_COPY_TRUNCOVER__(const bigInt source__, size_t output_cap) {
    assert(__BIGINT_VALIDATE__(source__));
    bigInt dst__;
    if (output_cap == 0) __BIGINT_EMPTY_INIT__(&dst__);
    else {
        __BIGINT_LIMBS_INIT__(&dst__, output_cap);
        size_t operation_range = (output_cap < source__.n) ? output_cap : source__.n;
        memcpy(dst__.limbs, source__.limbs, operation_range * sizeof(uint64_t));
        dst__.n     = operation_range;
        dst__.sign  = source__.sign;
    }
    return dst__;
}




//* ========================================== GENERAL UTILITIES ============================================ */
inline void __BIGINT_CANONICALIZE__(bigInt *x) {
    if (x->cap < 1) {
        // Just reset to ensure value safety and certainty
        x->n = 0;
        x->sign = 1;
        x->cap = 1;
    }
    if (x->n > x->cap) x->n = x->cap;
    if (x->sign != 1 || x->sign != -1) {
        // Just reset to ensure value safety and certainty
        x->n = 0;
        x->sign = 1;
    }
}
void __BIGINT_NORMALIZE__(bigInt *x) {
    while (x->n > 0 && x->limbs[x->n - 1] == 0) --x->n; // Delete trailing/leading zeros
    if (x->n == 0) x->sign = 1; // Guarantees 0, not -0
}
void __BIGINT_RESIZE__(bigInt *x, size_t k) { //* Exact Capacity resize
    assert(__BIGINT_INTERNAL_SVALID__(x) || k);
    limb_t *__BUFFER_P = realloc(x->limbs, k * sizeof(limb_t));
    if (__BUFFER_P == NULL) abort();
    x->limbs = __BUFFER_P;
    x->cap   = k;
    if (x->n < x->cap) x->n = x->cap;
}
void __BIGINT_RESERVE__(bigInt *x, size_t k) { //* Minimum Capacity
    assert(__BIGINT_INTERNAL_SVALID__(x));
    if (x->cap >= k) return;
    size_t new_cap = x->cap;
    // Capacity doubles instead of incrementation, 
    // ---> Ensure less reallocation ---> Enhanced performance
    while (new_cap < k) new_cap *= 2;
    limb_t *__BUFFER_P = realloc(x->limbs, new_cap * sizeof(limb_t));
    if (__BUFFER_P == NULL) abort();
    x->limbs = __BUFFER_P;
    x->cap   = new_cap;
}
void __BIGINT_SHRINK__(bigInt *x, size_t k) { //* Maximum Capacity
    assert(__BIGINT_INTERNAL_PVALID__(x) && k);
    if (x->cap <= k) return;
    limb_t *__BUFFER_P = realloc(x->limbs, k * sizeof(limb_t));
    if (__BUFFER_P == NULL) abort();
    x->limbs = __BUFFER_P;
    x->cap   = k;
    if (x->n < x->cap) x->n = x->cap;
}
inline void __BIGINT_RESET__(bigInt *x) {
    assert(__BIGINT_INTERNAL_PVALID__(x));
    if (x->n >= 1) x->limbs[0] = 0;
    x->n    = 0;
    x->sign = 1;
}
static inline uint8_t __BIGINT_MUTATIVE_SUBJECT_VALIDATE__(bigInt *x) {
    if (x->limbs == NULL) return 0;
    return 1;
}
static inline uint8_t __BIGINT_STATE_VALIDATE__(bigInt x) {
    if (x.limbs == NULL) return 0;
    if (x.cap < 1) return 0;
    if (x.n > x.cap) return 0;
    if (x.sign != 1 && x.sign != -1) return 0;
    return 1;
}
inline uint8_t __BIGINT_VALIDATE__(bigInt x) {
    /* State Validation */
    if (x.limbs == NULL) return 0;
    if (x.cap < 1) return 0;
    if (x.n > x.cap) return 0;
    if (x.sign != 1 && x.sign != -1) return 0;
    /* Arithmetic Validation */
    if (x.limbs[x.n - 1] == 0) return 0;
    if (x.n == 0 && x.sign != 1) return 0;
    return 1;
}
inline uint8_t __BIGINT_PVALIDATE__(bigInt *x) {
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


