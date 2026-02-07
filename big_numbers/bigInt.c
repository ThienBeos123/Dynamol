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
                            forcing a manual deletion of the object (See more detailed explanation below on Functional Arithmetic)
*/




//* ======================================== CONSTRUCTORS & DESTRUCTOR ======================================= */
uint8_t __BIGINT_EMPTY_INIT__(bigInt *x) {
    if (x->limbs != NULL) return 1; // The bigInt is already initialized
    uint64_t *P_BUFFER__ = malloc(sizeof(uint64_t));
    if (P_BUFFER__ == NULL) return 1;
    x->limbs = P_BUFFER__;
    x->cap   = 1;
    x->n     = 0;
    x->sign  = 1;
    return 0;
}
uint8_t __BIGINT_FREE__(bigInt *x) {
    if (x->limbs == NULL) return 1; // It is already freed
    free(x->limbs);
    x->limbs = NULL;              
    x->n     = 0;
    x->cap   = 0;
    x->sign  = 1; 
    return 0;
}
uint8_t __BIGINT_LIMBS_INIT__(bigInt *x, size_t n) {
    if (n == 0 || x->limbs != NULL) return 1; // Intializing 0 cap OR already initialized
    uint64_t *__BUFFER_P = calloc(n, sizeof(uint64_t));
    if (__BUFFER_P == NULL) return 1;
    x->limbs = __BUFFER_P;    
    x->cap   = n; // A capacity of n (n spaces in the heap)
    x->n     = 0; // Currently using no limb
    x->sign  = 1; return 0;
}
uint8_t __BIGINT_STANDARD_INIT__(bigInt *x, const bigInt y) {
    if (x->limbs != NULL) return 1; // Already Initialized
    if (!__BIGINT_STATE_VALIDATE__(y)) return 1;
    size_t alloc_size = (y.n) ? y.n : 1;
    uint64_t *__BUFFER_P = malloc(alloc_size * sizeof(uint64_t));
    if (__BUFFER_P == NULL) return 1;
    x->limbs    = __BUFFER_P;
    if (y.n) memcpy(x->limbs, y.limbs, y.n * sizeof(uint64_t));
    x->n        = y.n;
    x->cap      = alloc_size;
    x->sign     = (y.n) ? y.sign : 1;
    return 0;
}
uint8_t __BIGINT_UI64_INIT__(bigInt *x, uint64_t in) {
    if (x->limbs != NULL) return 1; // ALREADY INITIALIZED
    uint64_t *__BUFFER_P = malloc(sizeof(uint64_t));
    if (__BUFFER_P == NULL) return 1;
    x->limbs    = __BUFFER_P;
    x->limbs[0] = in;
    x->n        = (in) ? 1 : 0;
    x->cap      = 1;
    x->sign     = 1; 
    return 0;
}
uint8_t __BIGINT_I64_INIT__(bigInt *x, int64_t in) {
    if (x->limbs != NULL) return 1; // ALREADY INITIALIZED
    uint64_t *__BUFFER_P = malloc(sizeof(uint64_t));
    if (__BUFFER_P == NULL) return 1;
    x->limbs    = __BUFFER_P;
    x->limbs[0] = (uint64_t)(llabs(in));
    x->n        = (in) ? 1 : 0;
    x->cap      = 1;
    x->sign     = (in < 0) ? -1 : 1; 
    return 0;
}
uint8_t __BIGINT_LD_INIT__(bigInt *x, long double in) {}




//* =============================================== ASSIGNMENTS ============================================== */
/* --------- BigInt --> Primitive Types --------- */
uint8_t __BIGINT_SET_UI64__(const bigInt x, uint64_t *receiver) {
    assert(__BIGINT_STATE_VALIDATE__(x));
    if (!__BIGINT_STATE_VALIDATE__(x)) return 1;
    *receiver = (x.n) ? x.limbs[0] : 0;
    return 0;
}
uint8_t __BIGINT_SET_I64__(const bigInt x, int64_t *receiver) {
    assert(__BIGINT_STATE_VALIDATE__(x));
    if (!__BIGINT_STATE_VALIDATE__(x)) return 1;
    uint64_t raw_u64 = (x.n) ? x.limbs[0] : 0;
    uint64_t abs_int64_min = (uint64_t)(llabs(INT64_MIN + 1)) + 1;
    if (raw_u64 > abs_int64_min && x.sign == -1) *receiver = (int64_t)(raw_u64 & I64_MIN_BIT_MASK) * x.sign;
    else if (raw_u64 > INT64_MAX && x.sign == 1) *receiver = (int64_t)(raw_u64 & I64_MAX_BIT_MASK) * x.sign;
    else *receiver = ((int64_t)raw_u64) * x.sign;
    return 0;
}
uint8_t __BIGINT_SET_LD__(const bigInt x, long double *receiver) {}
uint8_t __BIGINT_SET_UI64_SAFE__(const bigInt x, uint64_t *receiver) {
    assert(__BIGINT_VALIDATE__(x));
    if (!__BIGINT_VALIDATE__(x)) return 1;
    if (x.sign == -1 || x.n > 1) return 1;
    *receiver = (x.n) ? x.limbs[0] : 0;
    return 0;
}
uint8_t __BIGINT_SET_I64_SAFE__(const bigInt x, int64_t *receiver) {
    assert(__BIGINT_VALIDATE__(x));
    if (!__BIGINT_VALIDATE__(x)) return 1;
    if (x.n > 1) return 1;
    uint64_t raw_u64 = (x.n) ? x.limbs[0] : 0;
    uint64_t abs_int64_min = (uint64_t)(llabs(INT64_MIN + 1)) + 1;
    if (raw_u64 > abs_int64_min && x.sign == -1) return 1;
    if (raw_u64 > INT64_MAX && x.sign == 1) return 1;
    *receiver = ((int64_t)raw_u64) * x.sign;
    return 0;
}
uint8_t __BIGINT_SET_LD_SAFE__(const bigInt x, long double *receiver) {}
/* --------- Primitive Types --> BigInt --------- */
uint8_t __BIGINT_GET_UI64__(uint64_t val, bigInt *receiver) {
    assert(__BIGINT_MUTATIVE_SUBJECT_VALIDATE__(receiver));
    if (!__BIGINT_MUTATIVE_SUBJECT_VALIDATE__(receiver)) return 1;

    receiver->limbs[0] = val;
    receiver->n        = (val) ? 1 : 0;
    receiver->sign     = 1;
    return 0;
}
uint8_t __BIGINT_GET_I64__(int64_t val, bigInt *receiver) {
    assert(__BIGINT_MUTATIVE_SUBJECT_VALIDATE__(receiver));
    if (!__BIGINT_MUTATIVE_SUBJECT_VALIDATE__(receiver)) return 1;

    uint64_t abs_val = (val == INT64_MIN) ? 
        (uint64_t)(llabs(val + 1)) + 1 :
        (uint64_t)(llabs(val));
    receiver->limbs[0] = abs_val;
    receiver->n        = (val) ? 1 : 0;
    receiver->sign     = (val < 0) ? -1 : 1;
    return 0;
}
uint8_t __BIGINT_GET_LD__(long double x, bigInt *receiver) {}
uint8_t __BIGINT_GET_LD_SAFE__(long double x, bigInt *receiver) {}



//* =============================================== CONVERSIONS ============================================== */
/* --------- BigInt --> Primitive Types --------- */
uint64_t __BIGINT_TO_UI64__(const bigInt x) {
    if (!__BIGINT_STATE_VALIDATE__(x)) { errno = EINVAL; return -1; }
    uint64_t res = (x.n) ? x.limbs[0] : 0;
    return res;
}
int64_t __BIGINT_TO_I64__(const bigInt x) {
    if (!__BIGINT_STATE_VALIDATE__(x)) { errno = EINVAL; return INT_MIN; }
    int64_t res;
    uint64_t raw_u64 = (x.n) ? x.limbs[0] : 0;
    uint64_t abs_int64_min = (uint64_t)(llabs(INT64_MIN + 1)) + 1;
    if (raw_u64 > abs_int64_min && x.sign == -1) res = (int64_t)(raw_u64 & I64_MIN_BIT_MASK) * x.sign;
    else if (raw_u64 > INT64_MAX && x.sign == 1) res = (int64_t)(raw_u64 & I64_MAX_BIT_MASK) * x.sign;
    else res = ((int64_t)raw_u64) * x.sign;
    return res;
}
long double __BIGINT_TO_LD_(const bigInt x) {}
uint64_t __BIGINT_TO_UI64_SAFE__(const bigInt x) {
    if (!__BIGINT_VALIDATE__(x)) { errno = EINVAL; return -1; }
    if (x.sign == -1 || x.n > 1) { errno = ERANGE; return -1; }
    uint64_t res = (x.n) ? x.limbs[0] : 0;
    return res;
}
int64_t __BIGINT_TO_I64_SAFE__(const bigInt x) {
    if (!__BIGINT_VALIDATE__(x)) { errno = EINVAL; return INT_MIN; }
    if (x.n > 1) { errno = ERANGE; return INT_MIN; }
    uint64_t raw_u64 = (x.n) ? x.limbs[0] : 0;
    uint64_t abs_int64_min = (uint64_t)(llabs(INT64_MIN + 1)) + 1;
    if (raw_u64 > abs_int64_min && x.sign == -1) { errno = ERANGE; return INT_MIN; }
    if (raw_u64 > INT64_MAX && x.sign == 1) { errno = ERANGE; return INT_MIN; }
    int64_t res = ((int64_t)raw_u64) * x.sign;
    return res;
}
long double __BIGINT_TO_LD_SAFE_(const bigInt x) {}
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
        res.limbs[0] = (uint64_t)(llabs(x));
        res.n        = 1;
        res.sign     = (x < 0) ? -1 : 1;
    }
    return res;
}
bigInt __BIGINT_FROM_LD_(long double x) {}
bigInt __BIGINT_FROM_LD_SAFE__(long double x) {}


//* =========================================== BITWISE OPERATIONS =========================================== */



//* =============================================== COMPARISONS ============================================== */
/*
*
*

todo 1) Refactor Magnitude Comparisons
*/
int8_t __BIGINT_COMPARE_MAGNITUDE_UI64__(const bigInt *x, const uint64_t val) {
    if (x->n > 1) return 1;
    if (x->sign == -1) return -1;
    if (x->limbs[0] > val) return 1;
    else if (x->limbs[0] < val) return -1;
    return 0;
}
int8_t __BIGINT_COMPARE_MAGNITUDE__(const bigInt *a, const bigInt *b) {
    if (a->n != b->n) return (a->n > b->n) ? 1 : -1;
    for (size_t i = a->n - 1; i >= 0; --i) { // Loops from most-significant digit down to least-significant digit
        if (a->limbs[i] != b->limbs[i]) return (a->limbs[i] > b->limbs[i]) ? 1 : -1; 
        // Compare which one current most-significant digit is bigger
    }
    return 0;
}
/* --------------- Integer - I64 --------------- */
uint8_t __BIGINT_EQUAL_I64__(const bigInt x, const int64_t val) {
    if (!__BIGINT_VALIDATE__(x)) { errno = EINVAL; return -1; }
    if (x.n == 0) return (val) ? 0 : 1;
    int8_t val_sign = (val < 0) ? -1 : 1;
    if (val_sign != x.sign) return 0;
    if (x.n      > 1)       return 0;
    return x.limbs[0] == (uint64_t)(llabs(val));
}
uint8_t __BIGINT_LESS_I64__(const bigInt x, const int64_t val) { 
    if (!__BIGINT_VALIDATE__(x)) { errno = EINVAL; return -1; }
    if (x.n == 0) return (val > 0) ? 1 : 0;
    int8_t val_sign = (val < 0) ? -1 : 1;
    if (val_sign != x.sign) return (x.sign < val_sign);
    if (x.n      > 1)       return (x.sign == -1);
    if (x.limbs[0] > (uint64_t)llabs(val)) return (x.sign == -1);
    return (x.limbs[0] < (uint64_t)llabs(val)) && (x.sign == 1);
}
uint8_t __BIGINT_MORE_I64__(const bigInt x, const int64_t val) {
    if (!__BIGINT_VALIDATE__(x)) { errno = EINVAL; return -1; }
    if (x.n == 0) return (val < 0) ? 1 : 0;
    int8_t val_sign = (val < 0) ? -1 : 1;
    if (val_sign != x.sign) return (x.sign > val_sign);
    if (x.n      > 1)       return (x.sign == 1);
    if (x.limbs[0] < (uint64_t)llabs(val)) return (x.sign == -1);
    return (x.limbs[0] > (uint64_t)llabs(val)) && (x.sign == 1);
}
uint8_t __BIGINT_LESS_OR_EQUAL_I64__(const bigInt x, const int64_t val) {
    if (!__BIGINT_VALIDATE__(x)) { errno = EINVAL; return -1; }
    if (x.n == 0) return (val >= 0) ? 1 : 0;
    int8_t val_sign = (val < 0) ? -1 : 1;
    if (x.sign != val_sign) return (x.sign < val_sign);
    if (x.n    > 1)         return (x.sign == -1);
    // Case eg: 189 > 171  |  -189 < -171
    if (x.limbs[0] > (uint64_t)llabs(val)) return (x.sign == -1);
    return (x.sign == 1); // Case eg: 178 < 181  |   -178 > -181
}
uint8_t __BIGINT_MORE_OR_EQUALL_I64__(const bigInt x, const int64_t val) {
    if (!__BIGINT_VALIDATE__(x)) { errno = EINVAL; return -1; }
    if (x.n == 0) return (val <= 0) ? 1 : 0;
    int8_t val_sign = (val < 0) ? -1 : 1;
    if (x.sign != val_sign) return (x.sign > val_sign);
    if (x.n    > 1)         return (x.sign == 1);
    // Case eg: 189 > 171  |  -189 < -171
    if (x.limbs[0] > (uint64_t)llabs(val)) return (x.sign == 1);
    return (x.sign == -1); // Case eg: 178 < 181  |   -178 > -181
}
/* ---------- Unsigned Integer - UI64 ---------- */
uint8_t __BIGINT_EQUAL_UI64__(const bigInt x, const uint64_t val) {
    if (!__BIGINT_VALIDATE__(x)) { errno = EINVAL; return -1; }
    if (x.n == 0) return (val) ? 0 : 1;
    if (x.sign == -1) return 0;
    if (x.n    > 1) return 0;
    return (x.limbs[0] == val);
}
uint8_t __BIGINT_LESS_UI64__(const bigInt x, const uint64_t val) {
    if (!__BIGINT_VALIDATE__(x)) { errno = EINVAL; return -1; }
    if (x.n == 0) return (val) ? 1 : 0;
    if (x.sign == -1) return 1;
    if (x.n    > 1) return 0;
    return (x.limbs[0] < val);
}
uint8_t __BIGINT_MORE_UI64__(const bigInt x, const uint64_t val) {
    if (!__BIGINT_VALIDATE__(x)) { errno = EINVAL; return -1; }
    if (x.n == 0) return (val) ? 0 : 1;
    if (x.sign == -1) return 0;
    if (x.n    > 1) return 1;
    return (x.limbs[0] > val);
}
uint8_t __BIGINT_LESS_OR_EQUAL_UI64__(const bigInt x, const uint64_t val) {
    if (!__BIGINT_VALIDATE__(x)) { errno = EINVAL; return -1; }
    if (x.n == 0) return 1; // Always true, as every R+ numbers are always >= 0
    if (x.sign == -1) return 1;
    if (x.n    > 1) return 0;
    return (x.limbs[0] <= val);
}
uint8_t __BIGINT_MORE_OR_EQUALL_UI64__(const bigInt x, const uint64_t val) {
    if (!__BIGINT_VALIDATE__(x)) { errno = EINVAL; return -1; }
    if (x.n == 0) return (val) ? 0 : 1;
    if (x.sign == -1) return 0;
    if (x.n    > 1) return 1;
    return (x.limbs[0] >= val);
}
/* ------------------- BigInt ------------------ */
uint8_t __BIGINT_EQUAL__(const bigInt a, const bigInt b) {
    if (!__BIGINT_VALIDATE__(a) || !__BIGINT_VALIDATE__(b)) { errno = EINVAL; return -1; }
    if (!a.n) return (!b.n) ? 1 : 0;
    if (a.sign != b.sign) return 0;
    if (a.n    != b.n)    return 0;
    return memcmp(a.limbs, b.limbs, a.n * sizeof(uint64_t)) == 0;
}
uint8_t __BIGINT_LESS__(const bigInt a, const bigInt b) {
    if (!__BIGINT_VALIDATE__(a) || !__BIGINT_VALIDATE__(b)) { errno = EINVAL; return -1; }
    if (a.sign != b.sign) return (a.sign < b.sign);
    if (a.n    != b.n)    return (a.sign == 1) ? (a.n < b.n) : (a.n > b.n);
    return (a.sign == 1) ? 
                memcmp(a.limbs, b.limbs, a.n * sizeof(uint64_t)) < 0 :
                memcmp(a.limbs, b.limbs, a.n * sizeof(uint64_t)) > 0;
}
uint8_t __BIGINT_MORE__(const bigInt a, const bigInt b) {
    if (!__BIGINT_VALIDATE__(a) || !__BIGINT_VALIDATE__(b)) { errno = EINVAL; return -1; }
    if (a.sign != b.sign) return (a.sign > b.sign);
    if (a.n    != b.n)    return (a.sign == 1) ? (a.n > b.n) : (a.n < b.n);
    return (a.sign == 1) ? 
                memcmp(a.limbs, b.limbs, a.n * sizeof(uint64_t)) > 0 :
                memcmp(a.limbs, b.limbs, a.n * sizeof(uint64_t)) < 0;
}
uint8_t __BIGINT_LESS_OR_EQUAL__(const bigInt a, const bigInt b) {
    if (!__BIGINT_VALIDATE__(a) || !__BIGINT_VALIDATE__(b)) { errno = EINVAL; return -1; }
    if (a.sign != b.sign) return (a.sign < b.sign);
    if (a.n    != b.n)    return (a.sign == 1) ? (a.n < b.n) : (a.n > b.n);
    return (a.sign == 1) ? 
                memcmp(a.limbs, b.limbs, a.n * sizeof(uint64_t)) <= 0 :
                memcmp(a.limbs, b.limbs, a.n * sizeof(uint64_t)) >= 0;
}
uint8_t __BIGINT_MORE_OR_EQUAL__(const bigInt a, const bigInt b) {
    if (!__BIGINT_VALIDATE__(a) || !__BIGINT_VALIDATE__(b)) { errno = EINVAL; return -1; }
    if (a.sign != b.sign) return (a.sign > b.sign);
    if (a.n    != b.n)    return (a.sign == 1) ? (a.n > b.n) : (a.n < b.n);
    return (a.sign == 1) ? 
                memcmp(a.limbs, b.limbs, a.n * sizeof(uint64_t)) >= 0 :
                memcmp(a.limbs, b.limbs, a.n * sizeof(uint64_t)) <= 0;
}





//* ========================================= MAGNITUDE MATHEMATICA ========================================== */
/* -------------------- MAGNITUDED ARITHMETIC --------------------- */
void __BIGINT_MAGNITUDED_ADD__(bigInt *res, const bigInt *a, const bigInt *b) {
    size_t max = (a->n > b->n) ? a->n : b->n;
    // Set the minimum capacity of res to be 1 bigger
    // than the largest capacity between a & b ----> res->cap = max + 1
    uint8_t reserve_res = __BIGINT_RESERVE__(res, max + 1);
    assert(reserve_res);
    // if (reserve_res) abort();
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
    uint8_t reserve_res = __BIGINT_RESERVE__(res, a->n);
    assert(reserve_res);
    // if (reserve_res) abort();
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
    uint8_t reserve_res = __BIGINT_RESERVE__(res, x->n + 1);
    assert(reserve_res);
    // if (reserve_res) abort();
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
    uint8_t reserve_res = __BIGINT_RESERVE__(&quot, x->n+1); quot->n = x->n;
    assert(reserve_res);
    // if (reserve_res) abort();
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
uint8_t __BIGINT_MUT_MUL_UI64__(bigInt *x, uint64_t val) {
    assert(__BIGINT_PVALIDATE__(x));
    if (!__BIGINT_PVALIDATE__(x)) return 1;

    if (x->n == 0) return 0;
    if (val == 1);
    else if (!val) { 
        uint8_t op_res = __BIGINT_RESET__(x);
        if (unlikely(op_res)) return 1;
        else return 0;
    }
    else if (x->n == 1 && x->limbs[0] == 1) __BIGINT_MUT_COPY_UI64__(x, val);
    else {
        bigInt __TEMP_PROD__; __BIGINT_EMPTY_INIT__(&__TEMP_PROD__);
        __BIGINT_MAGNITUDED_MUL_UI64__(&__TEMP_PROD__, x, val);
        __BIGINT_MUT_COPY__(x, __TEMP_PROD__);
        __BIGINT_FREE__(&__TEMP_PROD__);
    }
    return 0;
}
uint8_t __BIGINT_MUT_DIV_UI64__(bigInt *x, uint64_t val) {}
uint8_t __BIGINT_MUT_MOD_UI64__(bigInt *x, uint64_t val) {}
uint8_t __BIGINT_MUT_MUL_I64__(bigInt *x, int64_t val) {
    assert(__BIGINT_PVALIDATE__(x));
    if (!__BIGINT_PVALIDATE__(x)) return 1;

    if (x->n == 0) return 0;
    if (!val) {
        uint8_t op_res = __BIGINT_RESET__(x);
        if (unlikely(op_res)) return 1;
    }
    else if (val == 1 || val == -1);
    else if (x->n == 1 && x->limbs[0] == 1) __BIGINT_MUT_COPY_I64__(x, val);
    else {
        uint64_t mag_val = (val == INT64_MIN) ? 
            (uint64_t)(llabs(val + 1)) + 1 :
            (uint64_t)(llabs(val));
        bigInt __TEMP_PROD__; __BIGINT_EMPTY_INIT__(&__TEMP_PROD__);
        __BIGINT_MAGNITUDED_MUL_UI64__(&__TEMP_PROD__, x, mag_val);
        __BIGINT_MUT_COPY__(x, __TEMP_PROD__);
        __BIGINT_FREE__(&__TEMP_PROD__);
    }
    x->sign *= (val < 0) ? -1 : 1;
    return 0;
}
uint8_t __BIGINT_MUT_DIV_I64__(bigInt *x, int64_t val) {}
uint8_t __BIGINT_MUT_MOD_I64__(bigInt *x, int64_t val) {}
uint8_t __BIGINT_MUT_ADD__(bigInt *x, const bigInt y) {}
uint8_t __BIGINT_MUT_SUB__(bigInt *x, const bigInt y) {}
uint8_t __BIGINT_MUT_MUL__(bigInt *x, const bigInt y) {
    assert(__BIGINT_PVALIDATE__(x) && __BIGINT_VALIDATE__(y));
    if (!__BIGINT_PVALIDATE__(x) && __BIGINT_VALIDATE__(y)) return 1;

    if (x->n == 0) return 0;
    if (!y.n) {
        uint8_t op_res = __BIGINT_RESET__(x);
        if (uinlikely(op_res)) return 1;
        return 0;
    }
    if (y.n == 1 && y.limbs[0] == 1);
    else if (x->n == 1 && x->limbs[0] == 1) __BIGINT_MUT_COPY__(x, y);
    else {
        bigInt __TEMP_PROD__; __BIGINT_EMPTY_INIT__(&__TEMP_PROD__);
        __BIGINT_MAGNITUDED_MUL__(&__TEMP_PROD__, x, &y);
        __BIGINT_MUT_COPY__(x, __TEMP_PROD__);
        __BIGINT_FREE__(&__TEMP_PROD__);
    }
    x->sign *= y.sign;
    return 0;
}
uint8_t __BIGINT_MUT_DIV__(bigInt *x, const bigInt y) {}
uint8_t __BIGINT_MUT_MOD__(bigInt *x, const bigInt y) {}
/* ------------------ FUNCTIONAL ARITHMETIC ------------------- */
bigInt __BIGINT_MUL_UI64__(const bigInt x, uint64_t val) {
    assert(__BIGINT_VALIDATE__(x));
    if (!__BIGINT_VALIDATE__(x)) return __BIGINT_ERROR_VALUE__();

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
bigInt __BIGINT_DIV_UI64__(const bigInt x, uint64_t val) {}
bigInt __BIGINT_MOD_UI64__(const bigInt x, uint64_t val) {}
bigInt __BIGINT_MUL_I64__(const bigInt x, int64_t val) {
    assert(__BIGINT_VALIDATE__(x));
    if (!__BIGINT_VALIDATE__(x)) return __BIGINT_ERROR_VALUE__();

    bigInt res;
    if (!x.n || !val) __BIGINT_EMPTY_INIT__(&res);
    else if (x.n == 1 && x.limbs[0] == 1) __BIGINT_I64_INIT__(&res, val);
    else if (llabs(val) == 1) __BIGINT_STANDARD_INIT__(&res, x);
    else {
        uint64_t mag_val = (val == INT64_MIN) ? 
            (uint64_t)(llabs(val + 1)) + 1 : 
            (uint64_t)(llabs(val));
        __BIGINT_EMPTY_INIT__(&res);
        __BIGINT_MAGNITUDED_MUL_UI64__(&res, &x, mag_val);
    }
    res.sign = x.sign * ((val < 0) ? -1 : 1);
    return res;
}
bigInt __BIGINT_DIV_I64__(const bigInt x, int64_t val) {}
bigInt __BIGINT_MOD_I64__(const bigInt x, int64_t val) {}
bigInt __BIGINT_ADD__(const bigInt x, const bigInt y) {}
bigInt __BIGINT_SUB__(const bigInt x, const bigInt y) {}
bigInt __BIGINT_MUL__(const bigInt x, const bigInt y) {
    assert(__BIGINT_VALIDATE__(x) && __BIGINT_VALIDATE__(y));
    if (!__BIGINT_VALIDATE__(x) || !__BIGINT_VALIDATE__(y)) return __BIGINT_ERROR_VALUE__();

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
bigInt __BIGINT_DIV__(const bigInt x, const bigInt y) {}
bigInt __BIGINT_MOD__(const bigInt x, const bigInt y) {}





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
uint8_t __BIGINT_MUT_MODULO_UI64__(bigInt *x, uint64_t modulus) {}
uint8_t __BIGINT_MUT_MODULO_I64__(bigInt *x, int64_t modulus) {}
uint8_t __BIGINT_MUT_MODULO__(bigInt *x, const bigInt modulus) {}
uint64_t __BIGINT_MODULO_UI64__(const bigInt x, uint64_t modulus) {}
int64_t __BIGINT_MODULO_I64__(const bigInt x, int64_t modulus) {}
bigInt __BIGINT_MODULO__(const bigInt x, const bigInt modulus) {}
/* ---------------- SMALL Modular Arithmetic --------------- */
uint8_t __BIGINT_MUT_MODADD_UI64__(bigInt *x, const bigInt y, uint64_t modulus) {}
uint8_t __BIGINT_MUT_MODSUB_UI64__(bigInt *x, const bigInt y, uint64_t modulus) {}
uint8_t __BIGINT_MUT_MODADD__(bigInt *x, const bigInt y, const bigInt modulus) {}
uint8_t __BIGINT_MUT_MODSUB__(bigInt *x, const bigInt y, const bigInt modulus) {}
uint64_t __BIGINT_MODADD_UI64__(const bigInt x, const bigInt y, uint64_t modulus) {}
uint64_t __BIGINT_MODSUB_UI64__(const bigInt x, const bigInt y, uint64_t modulus) {}
bigInt __BIGINT_MODADD__(const bigInt x, const bigInt y, const bigInt modulus) {}
bigInt __BIGINT_MODSUB__(const bigInt x, const bigInt y, const bigInt modulus) {}
/* ---------------- LARGE Modular Arithmetic --------------- */
uint8_t __BIGINT_MUT_MODMUL_UI64_UI64__(bigInt *x, uint64_t y, uint64_t modulus) {}
uint8_t __BIGINT_MUT_MODDIV_UI64_UI64__(bigInt *x, uint64_t y, uint64_t modulus) {}
uint8_t __BIGINT_MUT_MODMUL_BI_UI64__(bigInt *x, const bigInt y, uint64_t modulus) {}
uint8_t __BIGINT_MUT_MODDIV_BI_UI64__(bigInt *x, const bigInt y, uint64_t modulus) {}
uint8_t __BIGINT_MUT_MODMUL_UI64_BI__(bigInt *x, uint64_t y, const bigInt modulus) {}
uint8_t __BIGINT_MUT_MODDIV_UI64_BI__(bigInt *x, uint64_t y, const bigInt modulus) {}
uint8_t __BIGINT_MUT_MODMUL__(bigInt *x, const bigInt y, const bigInt modulus) {}
uint8_t __BIGINT_MUT_MODDIV__(bigInt *x, const bigInt y, const bigInt modulus) {}
uint64_t __BIGINT_MODMUL_UI64_UI64__(const bigInt x, uint64_t y, uint64_t modulus) {}
uint64_t __BIGINT_MODDIV_UI64_UI64__(const bigInt x, uint64_t y, uint64_t modulus) {}
uint64_t __BIGINT_MODMUL_BI_UI64__(const bigInt x, const bigInt y, uint64_t modulus) {}
uint64_t __BIGINT_MODDIV_BI_UI64__(const bigInt x, const bigInt y, uint64_t modulus) {}
bigInt __BIGINT_MODMUL_UI64_BI__(const bigInt x, uint64_t y, const bigInt modulus) {}
bigInt __BIGINT_MODDIV_UI64_BI__(const bigInt x, uint64_t y, const bigInt modulus) {}
bigInt __BIGINT_MODMUL__(const bigInt x, const bigInt y, const bigInt modulus) {}
bigInt __BIGINT_MODDIV__(const bigInt x, const bigInt y, const bigInt modulus) {}
/* ---------------------- Modular Algebraic ------------------ */
uint8_t __BIGINT_MUT_MODEXP_UI64__(bigInt *x, const bigInt y, uint64_t modulus) {}
uint8_t __BIGINT_MUT_MODSQR_UI64__(bigInt *x, uint64_t modulus) {}
uint8_t __BIGINT_MUT_MODINV_UI64__(bigInt *x, uint64_t modulus) {}
uint8_t __BIGINT_MUT_MODEXP__(bigInt *x, const bigInt y, const bigInt modulus) {}
uint8_t __BIGINT_MUT_MODSQR__(bigInt *x, const bigInt modulus) {}
uint8_t __BIGINT_MUT_MODINV__(bigInt *x, const bigInt modulus) {}
uint64_t __BIGINT_MODEXP_UI64__(const bigInt x, const bigInt y, uint64_t modulus) {}
uint64_t __BIGINT_MODSQR_UI64__(const bigInt x, uint64_t modulus) {}
uint64_t __BIGINT_MODINV_UI64__(const bigInt x, uint64_t modulus) {}
bigInt __BIGINT_MODEXP__(const bigInt x, const bigInt y, const bigInt modulus) {}
bigInt __BIGINT_MODSQR__(const bigInt x, const bigInt modulus) {}
bigInt __BIGINT_MODINV__(const bigInt x, const bigInt modulus) {}




//* ================================================= COPIES ================================================= */
/* -------------  Mutative SMALL Copies ------------- */
uint8_t __BIGINT_MUT_COPY_UI64__(bigInt *dst__, uint64_t source__) {
    if (!__BIGINT_MUTATIVE_SUBJECT_VALIDATE__(dst__)) return 1;
    __BIGINT_CANONICALIZE__(dst__);
    if (dst__->n == 0 && !source__) return 0;
    if (dst__->n == 1 && dst__->limbs[0] == source__) return 0;
    dst__->limbs[0] = source__;
    dst__->n        = source__ ? 1 : 0;
    dst__->sign     = 1;
    return 0;
}
uint8_t __BIGINT_MUT_COPY_DEEP_UI64__(bigInt *dst__, uint64_t source__) {
    if (dst__->limbs == NULL) return 1;
    __BIGINT_CANONICALIZE__(dst__);
    // Always reallocate and resize if dst__->cap is more than 1
    if (dst__->cap > 1) {
        uint64_t *__BUFFER_P = realloc(dst__->limbs, sizeof(uint64_t));
        if (__BUFFER_P == NULL) return 1;
        dst__->limbs = __BUFFER_P;
        dst__->cap     = 1;
    }
    if (dst__->n == 0 && !source__) return 0;
    if (dst__->n == 1 && dst__->limbs[0] == source__) return 0;
    dst__->limbs[0] = source__;
    dst__->n        = source__ ? 1 : 0;
    dst__->sign     = 1;
    return 0;
}
uint8_t __BIGINT_MUT_COPY_I64__(bigInt *dst__, int64_t source__) {
    if (!__BIGINT_MUTATIVE_SUBJECT_VALIDATE__(dst__)) return 1;
    __BIGINT_CANONICALIZE__(dst__);
    if (dst__->n == 0 && !source__) return 0;
    if (dst__->n == 1 && dst__->limbs[0] == (uint64_t)llabs(source__)) {
        dst__->sign = (source__ < 0) ? -1 : 1;
        return 0;
    }
    dst__->limbs[0] = (uint64_t)(llabs(source__));
    dst__->n        = source__ ? 1 : 0;
    dst__->sign     = (source__< 0 ? -1 : 1);
    return 0;
}
uint8_t __BIGINT_MUT_COPY_DEEP_I64__(bigInt *dst__, int64_t source__) {
    if (!__BIGINT_MUTATIVE_SUBJECT_VALIDATE__(dst__)) return 1;
    __BIGINT_CANONICALIZE__(dst__);
    // Always reallocate and resize if dst__->cap is more than 1
    if (dst__->cap > 1) {
        uint64_t *__BUFFER_P = realloc(dst__->limbs, sizeof(uint64_t));
        if (__BUFFER_P == NULL) return 1;
        dst__->limbs = __BUFFER_P;
        dst__->cap     = 1;
    }
    if (dst__->n == 0 && !source__) return 0;
    if (dst__->n == 1 && dst__->limbs[0] == (uint64_t)llabs(source__)) {
        dst__->sign = (source__ < 0) ? -1 : 1;
        return 0;
    }
    dst__->limbs[0] = (uint64_t)(llabs(source__));
    dst__->n        = source__ ? 1 : 0;
    dst__->sign     = (source__< 0 ? -1 : 1);
    return 0;
}
/* -------------  Mutative LARGE Copies ------------- */
uint8_t __BIGINT_MUT_COPY_LD__(bigInt *dst__, long double source__) {}
uint8_t __BIGINT_MUT_COPY_DEEP_LD__(bigInt *dst__, long double source__) {}
uint8_t __BIGINT_MUT_COPY_OVER_LD__(bigInt *dst__, long double source__) {}
uint8_t __BIGINT_MUT_COPY_TRUNCOVER_LD__(bigInt *dst__, long double source__) {}
uint8_t __BIGINT_MUT_COPY__(bigInt *dst__, bigInt source__) {
    if (!__BIGINT_STATE_VALIDATE__(source__)) return 1;
    if (!__BIGINT_MUTATIVE_SUBJECT_VALIDATE__(dst__)) return 1;
    __BIGINT_CANONICALIZE__(dst__); // Enforce contracts, ESPECAILLY Contract 3
    /* Fast Paths */
    // Since they're equal, and due to Contract 3
    //  ------> They're not subjected to resizing if these cases are true
    if (dst__->n == 0 && source__.n == 0) return 0;
    if (dst__->n == source__.n && !memcmp(dst__->limbs, source__.limbs, source__.n)) {
        dst__->sign = source__.sign;
        return 0;
    }
    /* Standard Route */
    if (dst__->cap < source__.n) {
        uint8_t res = __BIGINT_RESERVE__(dst__, source__.n);
        if (res == 1) return 1; // If reservation contains error, return error code
    }
    memcpy(dst__->limbs, source__.limbs, source__.n);
    dst__->n    = source__.n;
    dst__->sign = source__.sign;
    return 0;
}
uint8_t __BIGINT_MUT_COPY_DEEP__(bigInt *dst__, bigInt source__) {
    if (!__BIGINT_STATE_VALIDATE__(source__)) return 1;
    if (!__BIGINT_MUTATIVE_SUBJECT_VALIDATE__(dst__)) return 1;
    __BIGINT_CANONICALIZE__(dst__); // Enforce contracts, ESPECAILLY Contract 3
    /* Reallocation and resize */
    if (dst__->cap != source__.n) {
        size_t size_to_change = source__.n;
        if (source__.n == 0) size_to_change = 1;
        uint8_t res = __BIGINT_RESIZE__(dst__, size_to_change);
        if (res) return 1;
    }
    /* Fast Paths */
    // The equal fast path (dst__ != 0 && source__ != 0) is not here since
    // Reallocation and Resizing may tamper with the size metadata,
    //  -----> Tampering with the validity of memcmp()
    if (dst__->n == 0 && source__.n == 0) return 0;

    /* Standard Path */
    memcpy(dst__->limbs, source__.limbs, source__.n * sizeof(uint64_t));
    dst__->n    = source__.n;
    dst__->sign = source__.sign;
    return 0;
}
uint8_t __BIGINT_MUT_COPY_OVER__(bigInt *dst__, bigInt source__) {
    if (!__BIGINT_STATE_VALIDATE__(source__)) return 1;
    if (!__BIGINT_MUTATIVE_SUBJECT_VALIDATE__(dst__)) return 1;
    __BIGINT_CANONICALIZE__(dst__); // Enforce contracts, ESPECAILLY Contract 3
    /* Fast Paths */
    // Since they're equal, and due to Contract 3
    //  ------> They're not subjected to errors if these cases are true
    if (dst__->n == 0 && source__.n == 0) return 0;
    if (dst__->n == source__.n && !memcmp(dst__->limbs, source__.limbs, source__.n * sizeof(uint64_t))) {
        dst__->sign = source__.sign;
        return 0;
    }
    /* Standard Route */
    if (dst__->cap < source__.n) return 1;
    memcpy(dst__->limbs, source__.limbs, source__.n * sizeof(uint64_t));
    dst__->n    = source__.n;
    dst__->sign = source__.sign;
    return 0;
}
uint8_t __BIGINT_MUT_COPY_TRUNCOVER__(bigInt *dst__, bigInt source__) {
    if (!__BIGINT_STATE_VALIDATE__(source__)) return 1;
    if (!__BIGINT_MUTATIVE_SUBJECT_VALIDATE__(dst__)) return 1;
    __BIGINT_CANONICALIZE__(dst__); // Enforce contracts, ESPECAILLY Contract 3
    /* Fast Paths */
    // Since they're equal, and due to Contract 3
    //  ------> They're not subjected to truncation if these cases are true
    if (dst__->n == 0 && source__.n == 0) return 0;
    if (dst__->n == source__.n && !memcmp(dst__->limbs, source__.limbs, source__.n * sizeof(uint64_t))) {
        dst__->sign = source__.sign;
        return 0;
    }
    /* Standard Route */
    size_t operation_range = source__.n;
    if (dst__->cap < source__.n) operation_range = dst__->cap; // Truncation (reducing operation range)
    memcpy(dst__->limbs, source__.limbs, operation_range * sizeof(uint64_t));
    dst__->n    = operation_range;
    dst__->sign = source__.sign;
    return 0;
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
        dst__.limbs[0] = (uint64_t)(llabs(source__));
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
    if (!__BIGINT_VALIDATE__(source__)) return __BIGINT_ERROR_VALUE__();
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
    if (!__BIGINT_STATE_VALIDATE__(source__)) return __BIGINT_ERROR_VALUE__();
    bigInt dst__;
    if (source__.n == 0) {
        __BIGINT_EMPTY_INIT__(&dst__);
    }
    else {
        __BIGINT_LIMBS_INIT__(&dst__, source__.n);
        memcpy(dst__.limbs, source__.limbs, source__.n * sizeof(uint64_t));
        dst__.n = source__.n;
    }
    dst__.sign = source__.sign;
    return dst__;
}
bigInt __BIGINT_COPY_OVER__(const bigInt source__, size_t output_cap) {
    if (!__BIGINT_VALIDATE__(source__)) return __BIGINT_ERROR_VALUE__();
    if (output_cap < source__.n) return __BIGINT_ERROR_VALUE__();
    bigInt dst__; __BIGINT_LIMBS_INIT__(&dst__, output_cap);
    memcpy(dst__.limbs, source__.limbs, source__.n * sizeof(uint64_t));
    dst__.n     = source__.n;
    dst__.sign  = source__.sign;
    return dst__;
}
bigInt __BIGINT_COPY_TRUNCOVER__(const bigInt source__, size_t output_cap) {
    if (!__BIGINT_VALIDATE__(source__)) return __BIGINT_ERROR_VALUE__();
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




//todo ========================================= GENERAL UTILITIES =========================================== */
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
uint8_t __BIGINT_RESIZE__(bigInt *x, size_t k) { //* Exact Capacity resize
    if (!__BIGINT_INTERNAL_SVALID__(x) || !k) return 1;
    uint64_t *__BUFFER_P = realloc(x->limbs, k * sizeof(uint64_t));
    if (__BUFFER_P == NULL) return 1;
    x->limbs = __BUFFER_P;
    x->cap   = k;
    if (x->n < x->cap) x->n = x->cap;
    return 0;
}
uint8_t __BIGINT_RESERVE__(bigInt *x, size_t k) { //* Minimum Capacity
    if (!__BIGINT_INTERNAL_SVALID__(x)) return 1;
    if (x->cap >= k) return 0;
    size_t new_cap = x->cap;
    while (new_cap < k) new_cap *= 2; /* Capacity doubles instead of incrementation, 
                                         ---> Ensure less reallocation ---> Enhanced performance */
    uint64_t *__BUFFER_P = realloc(x->limbs, new_cap * sizeof(uint64_t));
    if (__BUFFER_P == NULL) return 1;
    x->limbs = __BUFFER_P;
    x->cap   = new_cap;
    return 0;
}
uint8_t __BIGINT_SHRINK__(bigInt *x, size_t k) { //* Maximum Capacity
    if (!__BIGINT_INTERNAL_PVALID__(x) || !k) return 1;
    if (x->cap <= k) return 0;
    uint64_t *__BUFFER_P = realloc(x->limbs, k * sizeof(uint64_t));
    if (__BUFFER_P == NULL) return 1;
    x->limbs = __BUFFER_P;
    x->cap   = k;
    if (x->n < x->cap) x->n = x->cap;
    return 0;
}
uint8_t __BIGINT_RESET__(bigInt *x) {
    if (!__BIGINT_INTERNAL_PVALID__(x)) return 1;
    if (x->n >= 1) x->limbs[0] = 0;
    x->n    = 0;
    x->sign = 1;
    return 0;
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



//? ----------------------------------------    --   TESTING LAB    --   ----------------------------------------- */
int main(void) {

    return 0;
}