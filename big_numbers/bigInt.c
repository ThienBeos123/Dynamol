#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <errno.h>

#include "bigNums.h"
#include "bigNums_func.h"
#include "../calculation_algorithms/calculation.h"
#include "../internal_utils/util.h"

    /* NOTE: +) For any function of type uint8_t, 
    *           it is to be treated like a void function that handles errors (0 = success, 1 = error)  
    */


//todo ============================================ INTRODUCTION ============================================= */
/* Attribute Explanation:
*   +) sign     (uint8_t)       : Stores the sign (negative or positive)
*   +) limbs    (*uint64_t)     : Pointer to each limb that holds part of the bigInt number
*   +) n        (size_t)        : Number of currently used limbs (used to determine the bigInt value and in arithmetic)
*   +) cap      (size_t)        : Essentially the bigInt object/number's maximum capacity 
*
/* Terms Explanation:
*   +) Mutative Model: Changes an existing bigInt object/variable value in place (Eg: x += 10; )
*   +) Functional Model: -) Creates a new variable with the value of the expression (Eg: int x = 5 + 10; ) 
*                        -) This transfer ownership of the allocated limbs to the caller, 
                            forcing a manual deletion of the object (See more detailed explanation below on Functional Arithmetic)
*/




//todo ====================================== CONSTRUCTORS & DESTRUCTOR ====================================== */
uint8_t __BIGINT_EMPTY_INIT__(bigInt *x) {
    uint64_t *P_BUFFER__ = malloc(sizeof(uint64_t));
    if (P_BUFFER__ == NULL) return 1;
    x->limbs = P_BUFFER__;
    x->cap   = 1;
    x->n     = 0;
    x->sign  = 1;
}
uint8_t __BIGINT_FREE__(bigInt *x) {
    if (x->limbs == NULL) return 1;
    free(x->limbs);
    x->limbs = NULL;              
    x->n     = 0;
    x->cap   = 0;
    x->sign  = 1; 
    return 0;
}
uint8_t __BIGINT_LIMBS_INIT__(bigInt *x, size_t n) {
    x->limbs = calloc(n, sizeof(uint64_t)); // Allocate n spaces of 64 bits for n limbs in the heap
    if (!x->limbs) return 1;                // ----> CALLOC() (provide safety by zero-initializing each element)
    x->cap   = n; // A capacity of n (n spaces in the heap)
    x->n     = 0; // Currently using no limb
    x->sign  = 1; return 0;
}
uint8_t __BIGINT_STANDARD_INIT__(bigInt *x, const bigInt y) {}
uint8_t __BIGINT_UI64_INIT__(bigInt *x, uint64_t in) {
    if (!in) { __BIGINT_EMPTY_INIT__(x); return 0; }
    x->limbs = malloc(sizeof(uint64_t)); // Initialized a block with 1, 64 bit element in the heap
    if (!x->limbs) return 1;             // with uninitialized values ---> MALLOC()
    x->limbs[0] = in; // ---> Initialized the value of that block as the input
    x->n        = 1;
    x->cap      = 1;
    x->sign     = 1; return 0;
}
uint8_t __BIGINT_I64_INIT__(bigInt *x, int64_t in) {
    if (!in) { __BIGINT_EMPTY_INIT__(x); return 0; }
    x->limbs = malloc(sizeof(uint64_t)); // Initialized a block with 1, 64 bit element in the heap
    if (!x->limbs) return 1;             // with uninitialized values ---> MALLOC()
    x->limbs[0] = (in < 0) ? -in : in; // ---> Initialized the value of that block as the input
    x->n        = 1;
    x->cap      = 1;
    x->sign     = (in < 0) ? -1 : 1; return 0;
}
uint8_t __BIGINT_LD_INIT__(bigInt *x, long double in) {}




//todo ============================================= ASSIGNMENTS ============================================= */
/* --------- BigInt --> Primitive Types --------- */
uint8_t __BIGINT_SET_UI64__(const bigInt x, uint64_t *receiver) {}
uint8_t __BIGINT_SET_I64__(const bigInt x, int64_t *receiver) {}
uint8_t __BIGINT_SET_LD__(const bigInt x, long double *receiver) {}
uint8_t __BIGINT_SET_UI64_SAFE__(const bigInt x, uint64_t *receiver) {}
uint8_t __BIGINT_SET_I64_SAFE__(const bigInt x, int64_t *receiver) {}
uint8_t __BIGINT_SET_LD_SAFE__(const bigInt x, long double *receiver) {}
/* --------- Primitive Types --> BigInt --------- */
uint8_t __BIGINT_GET_UI64__(uint64_t x, bigInt *receiver) {}
uint8_t __BIGINT_GET_I64__(int64_t x, bigInt *receiver) {}
uint8_t __BIGINT_GET_LD__(long double x, bigInt *receiver) {}
uint8_t __BIGINT_GET_LD_SAFE__(long double x, bigInt *receiver) {}



//todo ============================================= CONVERSIONS ============================================= */
/* --------- BigInt --> Primitive Types --------- */
uint64_t __BIGINT_TO_UI64__(const bigInt x) {}
int64_t __BIGINT_TO_I64__(const bigInt x) {}
long double __BIGINT_TO_LD_(const bigInt x) {}
uint64_t __BIGINT_TO_UI64_SAFE__(const bigInt x) {}
int64_t __BIGINT_TO_I64_SAFE__(const bigInt x) {}
long double __BIGINT_TO_LD_SAFE_(const bigInt x) {}
/* --------- Primitive Types --> BigInt --------- */
bigInt __BIGINT_FROM_UI64__(uint64_t x) {}
bigInt __BIGINT_FROM_I64__(int64_t x) {}
bigInt __BIGINT_FROM_LD_(long double x) {}
bigInt __BIGINT_FROM_LD_SAFE__(long double x) {}



//todo ============================================= COMPARISONS ============================================= */
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
    return x.limbs[0] == llabs(val);
}
uint8_t __BIGINT_LESS_I64__(const bigInt x, const int64_t val) { 
    if (!__BIGINT_VALIDATE__(x)) { errno = EINVAL; return -1; }
    if (x.n == 0) return (val > 0) ? 1 : 0;
    int8_t val_sign = (val < 0) ? -1 : 1;
    if (val_sign != x.sign) return (x.sign < val_sign);
    if (x.n      > 1)       return (x.sign == -1);
    if (x.limbs[0] > llabs(val)) return (x.sign == -1);
    return (x.limbs[0] < llabs(val)) && (x.sign == 1);
}
uint8_t __BIGINT_MORE_I64__(const bigInt x, const int64_t val) {
    if (!__BIGINT_VALIDATE__(x)) { errno = EINVAL; return -1; }
    if (x.n == 0) return (val < 0) ? 1 : 0;
    int8_t val_sign = (val < 0) ? -1 : 1;
    if (val_sign != x.sign) return (x.sign > val_sign);
    if (x.n      > 1)       return (x.sign == 1);
    if (x.limbs[0] < llabs(val)) return (x.sign == -1);
    return (x.limbs[0] > llabs(val)) && (x.sign == 1);
}
uint8_t __BIGINT_LESS_OR_EQUAL_I64__(const bigInt x, const int64_t val) {
    if (!__BIGINT_VALIDATE__(x)) { errno = EINVAL; return -1; }
    if (x.n == 0) return (val >= 0) ? 1 : 0;
    int8_t val_sign = (val < 0) ? -1 : 1;
    if (x.sign != val_sign) return (x.sign < val_sign);
    if (x.n    > 1)         return (x.sign == -1);
    if (x.limbs[0] > llabs(val)) return (x.sign == -1); // Case eg: 189 > 171  |  -189 < -171
    return (x.sign == 1); // Case eg: 178 < 181  |   -178 > -181
}
uint8_t __BIGINT_MORE_OR_EQUALL_I64__(const bigInt x, const int64_t val) {
    if (!__BIGINT_VALIDATE__(x)) { errno = EINVAL; return -1; }
    if (x.n == 0) return (val <= 0) ? 1 : 0;
    int8_t val_sign = (val < 0) ? -1 : 1;
    if (x.sign != val_sign) return (x.sign > val_sign);
    if (x.n    > 1)         return (x.sign == 1);
    if (x.limbs[0] > llabs(val)) return (x.sign == 1); // Case eg: 189 > 171  |  -189 < -171
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
/* ------------ Floating Point - LD ------------ */
uint8_t __BIGINT_EQUAL_LD__(const bigInt x, const long double val) {}
uint8_t __BIGINT_LESS_LD__(const bigInt x, const long double val) {}
uint8_t __BIGINT_MORE_LD__(const bigInt x, const long double val) {}
uint8_t __BIGINT_LESS_OR_EQUAL_LD__(const bigInt x, const long double val) {}
uint8_t __BIGINT_MORE_OR_EQUALL_LD__(const bigInt x, const long double val) {}
/* ------------------- BigInt ------------------ */
uint8_t __BIGINT_EQUAL__(const bigInt a, const bigInt b) {}
uint8_t __BIGINT_LESS__(const bigInt a, const bigInt b) {}
uint8_t __BIGINT_MORE__(const bigInt a, const bigInt b) {}
uint8_t __BIGINT_LESS_OR_EQUAL__(const bigInt a, const bigInt b) {}
uint8_t __BIGINT_MORE_OR_EQUAL__(const bigInt a, const bigInt b) {}





//todo ======================================= MAGNITUDE MATHEMATICA ========================================= */
/* -------------------- MAGNITUDED ARITHMETIC --------------------- */
void __BIGINT_MAGNITUDED_ADD_UI64__(bigInt *res, const bigInt *x, const uint64_t val) {
    __BIGINT_ENSURE_CAPACITY(res, x->n + 1); // Ensure at least enough capacity even when carry is propagated to the end
    uint64_t carry = val; size_t i = 0;
    // Add when there is still carry to be propagated
    for (; (carry && i < x->n); ++i) {
        res->limbs[i] = __ADD_UI64__(x->limbs[i], carry, &carry); /* Initially:  Add val on first iteration, (carry = val)
                                                                     Afterwards: Add carry from the previous limb to the next limb */ }
    // Copy the remaining limbs of x (if the carry didn't propagate to the very end)
    for (; i < x->n; ++i) { res->limbs[i] = x->limbs[i]; }
                 res->n               = x->n;
    if (carry) { res->limbs[res->n++] = carry; } // Handle remaining carry if it propagated to the end
}
void __BIGINT_MAGNITUDED_SUB_UI64__(bigInt *res, const bigInt *x, const uint64_t val) {
    /* Design choice of __BIGINT_MAGNITUDED_SUB_UI64__():
    *       +) One critical design choice that was made is the decision to abort()
    *          magnitude subtraction on the occurence of underflowing (|x| < |y|)
    *       +) This is because when we subtract magnitudes, we also expect a magnitude value
    *       ------> |x| - |y| = |result|
    *       +) Underflow happens when |x| < |y| ----> |x| - |y| = -negative_value
    *       -----> Violate the contract by returning a negative value 
    *              that is illegal for an absolute value */

    __BIGINT_ENSURE_CAPACITY__(res, x->n); res->n = x->n;
    uint64_t borrow = val; size_t i = 0;
    for (; (borrow && i < x->n); ++i) {
        res->limbs[i] = __SUB_UI64__(x->limbs[i], borrow, &borrow); /* First iteration: Subtract val (borrow = val)
                                                                       Next iterations: Subtract borrow from last limb */ }
    assert(!borrow); // Check for Magnitude Underflow
    for (; i < x->n; ++i) { res->limbs[i] = x->limbs[i]; }
}
void __BIGINT_MAGNITUDED_MUL_UI64__(bigInt *res, const bigInt *x, const uint64_t val) {
    // Since the divisor size is small (n <= 1), we implement inline schoolbook multiplication
    __BIGINT_ENSURE_CAPACITY__(res, x->n + 1);
    uint64_t carry = 0;
    for (size_t i = 0; i < x->n; ++i) {
        uint64_t low, high;
        __MUL_UI64__(x->limbs[i], val, &low, &high);
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
    __BIGINT_ENSURE_CAPACITY__(&quot, x->n+1); quot->n = x->n;
    uint64_t remainder = 0;
    for (size_t i = x->n; i-- > 0;) {
       __DIV_HELPER_UI64__(remainder, 0, val, &quot->limbs[i], &remainder);
    }
    *rem = remainder;
    __BIGINT_NORMALIZE__(quot);
}
void __BIGINT_MAGNITUDED_ADD__(bigInt *res, const bigInt *a, const bigInt *b) {
    size_t max = (a->n > b->n) ? a->n : b->n;
    __BIGINT_ENSURE_CAPACITY__(res, max + 1); /* Set the minimum capacity of res to be 1 bigger
                                                 than the largest capacity between a & b ----> res->cap = max + 1 */
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
    __BIGINT_ENSURE_CAPACITY__(res, a->n);
    uint64_t borrow = 0;
    for (size_t i = 0; i < a->n; ++i) {
        uint64_t y = (i < b->n) ? b->limbs[i] : 0;
        res->limbs[i] = __SUB_UI64__(a->limbs[i], y, &borrow); // Do single-limb subtraction with borrow ---> Stores the borrow
    }
    res->n = a->n;
}
void __BIGINT_MAGNITUDED_MUL__(bigInt *res, const bigInt *a, const bigInt *b) {
    if (max(a->n, b->n) < SCHOOLBOOK)       __BIGINT_SCHOOLBOOK__(res, a, b);
    else                                    __BIGINT_KARATSUBA__(res, a, b);

    /* FULLY FINISHED BRANCHES FOR PERFORMANCE OPTIMIZATION */
    // if (max(a->n, b->n) < SCHOOLBOOK)       __BIGINT_SCHOOLBOOK__(res, a, b);
    // else if (max(a->n, b->n) < KARATSUBA)   __BIGINT_KARATSUBA__(res, a, b);
    // else if (max(a->n, b->n) < TOOM)        __BIGINT_TOOM_3__(res, a, b);
    // else                                    __BIGINT_SSA__(res, a, b);
}
void __BIGINT_MAGNITUDED_DIVMOD__(bigInt *quot, bigInt *rem, const bigInt *a, const bigInt *b) {
    assert(b->n == 0);
    if (b->n < KNUTH_D)         __BIGINT_KNUTH_D__(a, b, quot, rem);
    else                        __BIGINT_NEWTON_RECIPROCAL__(a, b, quot, rem);

    /* FULLY FINISHED BRANCHES FOR PERFORMANCE OPTIMIZATION */
    // if (b->n < KNUTH_D)         __BIGINT_KNUTH_D__(a, b, quot, rem);
    // else if (b->n < NEWTON)     __BIGINT_NEWTON_RECIPROCAL__(a, b, quot, rem);
    // else                        __BIGINT_NEWTON_FFT(a, b, quot, rem);
}
/* --------------- MAGNITUDED CORE NUMBER-THEORETIC ---------------- */
uint64_t ___GCD_UI64___() {}
void __BIGINT_MAGNITUDED_GCD_UI64__() {}
void __BIGINT_MAGNITUDED_LCM_UI64__() {}
void __BIGINT_MAGNITUDED_EUCMOD_UI64__() {}
void __BIGINT_MAGNITUDED_GCD__() {}
void __BIGINT_MAGNITUDED_LCM__() {}
void __BIGINT_MAGNITUDED_EUCMOD__() {}
void __BIGINT_MAGNITUDED_MODSQR__() {}
void __BIGINT_MAGNITUDED_MODINV__() {}
/* ----------------- MAGNITUDED MODULAR-ARITHMETIC ------------------ */
void __BIGINT_MAGNITUDED_MODADD_UI64__() {}
void __BIGINT_MAGNITUDED_MODSUB_UI64__() {}
void __BIGINT_MAGNITUDED_MODMUL_UI64__() {}
void __BIGINT_MAGNITUDED_MODDIV_UI64__() {}
void __BIGINT_MAGNITUDED_MODEXP_UI64__() {}
void __BIGINT_MAGNITUDED_MODADD__() {}
void __BIGINT_MAGNITUDED_MODSUB__() {}
void __BIGINT_MAGNITUDED_MODMUL__() {}
void __BIGINT_MAGNITUDED_MODDIV__() {}
void __BIGINT_MAGNITUDED_MODEXP__() {}





//todo ========================================== SIGNED ARITHMETIC ========================================== */
/* ------------------- MUTATIVE SMALL ARITHMETIC -------------------- */
uint8_t __BIGINT_MUT_ADD_UI64__(bigInt *x, const uint64_t val) {}
uint8_t __BIGINT_MUT_SUB_UI64__(bigInt *x, const uint64_t val) {}
uint8_t __BIGINT_MUT_MUL_UI64__(bigInt *x, const uint64_t val) {}
uint8_t __BIGINT_MUT_DIV_UI64__(bigInt *x, const uint64_t val) {}
uint8_t __BIGINT_MUT_MOD_UI64__(bigInt *x, const uint64_t val) {}
uint8_t __BIGINT_MUT_ADD_I64__(bigInt *x, const int64_t val) {}
uint8_t __BIGINT_MUT_SUB_I64__(bigInt *x, const int64_t val) {}
uint8_t __BIGINT_MUT_MUL_I64__(bigInt *x, const int64_t val) {}
uint8_t __BIGINT_MUT_DIV_I64__(bigInt *x, int64_t val) {}
uint8_t __BIGINT_MUT_MOD_I64__(bigInt *x, int64_t val) {}
/* ------------------- MUTATIVE BIG ARITHMETIC -------------------- */
uint8_t __BIGINT_MUT_ADD_LD__(bigInt *x, const long double val) {}
uint8_t __BIGINT_MUT_SUB_LD__(bigInt *x, const long double val) {}
uint8_t __BIGINT_MUT_MUL_LD__(bigInt *x, const long double val) {}
uint8_t __BIGINT_MUT_DIV_LD__(bigInt *x, const long double val) {}
uint8_t __BIGINT_MUT_MOD_LD__(bigInt *x, const long double val) {}
uint8_t __BIGINT_MUT_ADD__(bigInt *x, const bigInt *y) {}
uint8_t __BIGINT_MUT_SUB__(bigInt *x, const bigInt *y) {}
uint8_t __BIGINT_MUT_MUL__(bigInt *x, const bigInt *y) {}
uint8_t __BIGINT_MUT_DIV__(bigInt *x, const bigInt *y) {}
uint8_t __BIGINT_MUT_MOD__(bigInt *x, const bigInt *y) {}
/* ------------------ FUNCTIONAL SMALL ARITHMETIC ------------------- */
bigInt __BIGINT_ADD_UI64__(const bigInt *x, const uint64_t val) {}
bigInt __BIGINT_SUB_UI64__(const bigInt *x, const uint64_t val) {}
bigInt __BIGINT_MUL_UI64__(const bigInt *x, const uint64_t val) {}
bigInt __BIGINT_DIV_UI64__(const bigInt *x, const uint64_t val) {}
bigInt __BIGINT_MOD_UI64__(const bigInt *x, const uint64_t val) {}
bigInt __BIGINT_ADD_I64__(const bigInt *x, const int64_t val) {}
bigInt __BIGINT_SUB_I64__(const bigInt *x, const int64_t val) {}
bigInt __BIGINT_MUL_I64__(const bigInt *x, const int64_t val) {}
bigInt __BIGINT_DIV_I64__(const bigInt *x, const int64_t val) {}
bigInt __BIGINT_MOD_I64__(const bigInt *x, const int64_t val) {}
/* ------------------- FUNCTIONAL BIG ARITHMETIC -------------------- */
bigInt __BIGINT_ADD_LD__(const bigInt *x, const long double val) {}
bigInt __BIGINT_SUB_LD__(const bigInt *x, const long double val) {}
bigInt __BIGINT_MUL_LD__(const bigInt *x, const long double val) {}
bigInt __BIGINT_DIV_LD__(const bigInt *x, const long double val) {}
bigInt __BIGINT_MOD_LD__(const bigInt *x, const long double val) {}
bigInt __BIGINT_ADD__(const bigInt *x, const bigInt *y) {}
bigInt __BIGINT_SUB__(const bigInt *x, const bigInt *y) {}
bigInt __BIGINT_MUL__(const bigInt *x, const bigInt *y) {}
bigInt __BIGINT_DIV__(const bigInt *x, const bigInt *y) {}
bigInt __BIGINT_MOD__(const bigInt *x, const bigInt *y) {}





//todo ======================================== SIGNED NUMBER THEORY ========================================= */





//todo =============================================== COPIES ================================================ */
/* -------------  Mutative SMALL Copies ------------- */
uint8_t __BIGINT_MUT_COPY_UI64__(bigInt *dst__, uint64_t source__) {}
uint8_t __BIGINT_MUT_COPY_DEEP_UI64__(bigInt *dst__, uint64_t source__) {}
uint8_t __BIGINT_MUT_COPY_I64__(bigInt *dst__, int64_t source__) {}
uint8_t __BIGINT_MUT_COPY_DEEP_I64__(bigInt *dst__, int64_t source__) {}
/* -------------  Mutative LARGE Copies ------------- */
uint8_t __BIGINT_MUT_COPY_LD__(bigInt *dst__, long double source__) {}
uint8_t __BIGINT_MUT_COPY_DEEP_LD__(bigInt *dst__, long double source__) {}
uint8_t __BIGINT_MUT_COPY_OVER_LD__(bigInt *dst__, long double source__) {}
uint8_t __BIGINT_MUT_COPY_TRUNCOVER_LD__(bigInt *dst__, long double source__) {}
uint8_t __BIGINT_MUT_COPY__(bigInt *dst__, bigInt source__) {}
uint8_t __BIGINT_MUT_COPY_DEEP__(bigInt *dst__, bigInt source__) {}
uint8_t __BIGINT_MUT_COPY_OVER__(bigInt *dst__, bigInt source__) {}
uint8_t __BIGINT_MUT_COPY_TRUNCOVER__(bigInt *dst__, bigInt source__) {}
/* -------------  Functional SMALL Copies ------------- */
bigInt __BIGINT_COPY_UI64__(bigInt *dst__, uint64_t source__) {}
bigInt __BIGINT_COPY_DEEP_UI64__(bigInt *dst__, uint64_t source__) {}
bigInt __BIGINT_COPY_I64__(bigInt *dst__, int64_t source__) {}
bigInt __BIGINT_COPY_DEEP_I64__(bigInt *dst__, int64_t source__) {}
/* -------------  Functional LARGE Copies ------------- */
bigInt __BIGINT_COPY_LD__(bigInt *dst__, long double source__) {}
bigInt __BIGINT_COPY_DEEP_LD__(bigInt *dst__, long double source__) {}
bigInt __BIGINT_COPY_OVER_LD__(bigInt *dst__, long double source__) {}
bigInt __BIGINT_COPY_TRUNCOVER_LD__(bigInt *dst__, long double source__) {}
bigInt __BIGINT_COPY__(bigInt *dst__, bigInt source__) {}
bigInt __BIGINT_COPY_DEEP__(bigInt *dst__, bigInt source__) {}
bigInt __BIGINT_COPY_OVER__(bigInt *dst__, bigInt source__) {}
bigInt __BIGINT_COPY_TRUNCOVER__(bigInt *dst__, bigInt source__) {}




//todo ========================================= GENERAL UTILITIES =========================================== */
inline void __BIGINT_CANONICALIZE__(bigInt *x) {

}
void __BIGINT_NORMALIZE__(bigInt *x) {
    while (x->n > 0 && x->limbs[x->n - 1] == 0) --x->n; // Delete trailing/leading zeros
    if (x->n == 0) x->sign = 1; // Guarantees 0, not -0
}
uint8_t __BIGINT_RESERVE__(bigInt *x, size_t k) {
    if (x->cap >= k) return 0;
    size_t new_cap = (x->cap == 0) ? 1 : x->cap;
    while (new_cap < k) new_cap *= 2; /* Capacity doubles instead of incrementation, 
                                         ---> Ensure less reallocation ---> Enhanced performance */
    uint64_t *__BUFFER_P = realloc(x->limbs, new_cap * sizeof(uint64_t));
    if (__BUFFER_P == NULL) return 1;
    x->limbs = __BUFFER_P;
    x->cap   = new_cap;
    return 0;
}
uint8_t __BIGINT_SHRINK__(bigInt *x, size_t k) {}
uint8_t __BIGINT_RESET__(bigInt *x) {
    if (!__BIGINT_INTERNAL_PVALID__(x)) return 1;
    if (x->n >= 1) x->limbs[0] = 0;
    x->n    = 0;
    x->sign = 1;
    return 0;
}
inline uint8_t __BIGINT_STATE_VALIDTAE__(bigInt x) {
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





//? ----------------------------------------    --   TESTING LAB    --   ----------------------------------------- */
int main(void) {

    return 0;
}