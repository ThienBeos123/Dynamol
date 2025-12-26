#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <string.h>

#include "../Headers/singleLimb.h"
#include "../Headers/Big Data Types/bigInt.h"
#include "../Headers/Big Data Types/bigInt.hpp"

    /* NOTE: +) For any function of type uint8_t, 
    *           it is to be treated like a void function that handles errors (0 = success, 1 = error)  
    */


/* --------------------------------------    --   INTEGER TYPES    --   ----------------------------------------- */
/* Attribute Explanation:
*   +) sign     (uint8_t)       : Stores the sign (negative or positive)
*   +) limbs    (*uint64_t)     : Pointer to each limb that holds part of the bigInt number
*   +) n        (size_t)        : Number of currently used limbs (used to determine the bigInt value and in arithmetic)
*   +) cap      (size_t)        : Essentially the bigInt object/number's maximum capacity 
*
/* Terms Explanation:
*   +) Mutative Model: Changes an existing bigInt object/variable value in place (Eg: x += 10; )
*   +) Functional Model: Creates a new variable with the value of the expression (Eg: int x = 5 + 10; ) 
*/
/* ----------------- CONSTRUCTORS & DESCTRUCTORS --------------- */
void __BIGINT_EMPTY_INIT__(bigInt *x) { 
    x->limbs = NULL;              
    x->n     = 0;
    x->cap   = 0;
    x->sign  = 1; 
}
void __BIGINT_FREE__(bigInt *x) {
    free(x->limbs);
    x->limbs = NULL;              
    x->n     = 0;
    x->cap   = 0;
    x->sign  = 1; 
}
uint8_t __BIGINT_LIMBS_INIT__(bigInt *x, size_t n) {
    x->limbs = calloc(n, sizeof(uint64_t)); // Allocate n spaces of 64 bits for n limbs in the heap
    if (!x->limbs) return 1;                // ----> CALLOC() (provide safety by zero-initializing each element)
    x->cap   = n; // A capacity of n (n spaces in the heap)
    x->n     = 0; // Currently using no limb
    x->sign  = 1; return 0;
}
uint8_t __BIGINT_STANDARD_INIT__(bigInt *x, const char *str) {
    __BIGINT_EMPTY_INIT__(x);
    if (!str || *str == '\0') return 1;
    uint8_t __temp_sign__ = 1;
    if (*str == '-') { __temp_sign__ = -1; ++str; }
    else if (*str == '+') ++str;
    for (; *str; ++str) {
        if (*str < '0' || *str > '9') return 1;  // Error-case, char not a number
        __BIGINT_SPECIAL_MUL_UI64__(x, 10); 
        __BIGINT_SPECIAL_ADD_UI64__(x, *str - '0');
    }
    x->sign = (x->n == 0) ? 1 : __temp_sign__; return 0; // Success
}
uint8_t __BIGINT_UNSIGNED_PRIMITVE_INIT__(bigInt *x, uint64_t in) {
    if (!in) { __BIGINT_EMPTY_INIT__(x); return 0; }
    x->limbs = malloc(sizeof(uint64_t)); // Initialized a block with 1, 64 bit element in the heap
    if (!x->limbs) return 1;             // with uninitialized values ---> MALLOC()
    x->limbs[0] = in; // ---> Initialized the value of that block as the input
    x->n        = 1;
    x->cap      = 1;
    x->sign     = 1; return 0;
}
uint8_t __BIGINT_SIGNED_PRIMITIVE_INIT__(bigInt *x, int64_t in) {
    if (!in) { __BIGINT_EMPTY_INIT__(x); return 0; }
    x->limbs = malloc(sizeof(uint64_t)); // Initialized a block with 1, 64 bit element in the heap
    if (!x->limbs) return 1;             // with uninitialized values ---> MALLOC()
    x->limbs[0] = (in < 0) ? -in : in; // ---> Initialized the value of that block as the input
    x->n        = 1;
    x->cap      = 1;
    x->sign     = (in < 0) ? -1 : 1; return 0;
}
uint8_t __BIGINT_FLOAT_INIT__(bigInt *x, long double in) {

}

/* ------------------------ CONVERSIONS ------------------------ */




/* -------------------- BITWISE OPERATIONS --------------------- */



/* ------------------------ COMPARISONS ------------------------ */
int8_t __BIGINT_COMPARE_MAGNITUDE_UI64__(const bigInt *x, const uint64_t val) {
    if (x->n > 1) return 1;
    if (x->limbs[0] > val) return 1;
    else if (x->limbs[0] < val) return -1;
    return 0;
}
uint8_t __BIGINT_EQUAL_UI64__(const bigInt *x, const int64_t val) {
    int8_t val_sign = (val < 0) ? -1 : 1;
    if (val_sign    != x->sign) return 0;
    if (x->n        >  1)       return 0;
    if (x->limbs[0] != val)     return 0;
    return 1;
}
uint8_t __BIGINT_LESS_UI64__(const bigInt *x, const int64_t val) { 
    int8_t val_sign = (val < 0) ? -1 : 1;
    if (val_sign    != x->sign) return (x->sign < val_sign);
    if (x->n        > 1)        return 0;
    return (x->limbs[0] < val) | (x->sign < 0);
}
uint8_t __BIGINT_MORE_UI64__(const bigInt *x, const int64_t val) {
    int8_t val_sign = (val < 0) ? -1 : 1;
    if (val_sign    != x->sign) return (x->sign > val_sign);
    if (x->n        > 1)        return 1;
    return (x->limbs[0] > val) | (x->sign > 0);
}
uint8_t __BIGINT_LESS_OR_EQUAL_UI64__(const bigInt *x, const int64_t val) {
    return (__BIGINT_EQUAL_UI64__(x, val) || __BIGINT_LESS_UI64__(x, val));
}
uint8_t __BIGINT_MORE_OR_EQUALL_UI64__(const bigInt *x, const int64_t val) {
    return (__BIGINT_EQUAL_UI64__(x, val) || __BIGINT_MORE_UI64__(x, val));
}
int8_t __BIGINT_COMPARE_MAGNITUDE__(const bigInt *a, const bigInt *b) {
    if (a->n != b->n) return (a->n > b->n) ? 1 : -1;
    for (size_t i = a->n - 1; i >= 0; --i) { // Loops from most-significant digit down to least-significant digit
        if (a->limbs[i] != b->limbs[i]) return (a->limbs[i] > b->limbs[i]) ? 1 : -1; 
        // Compare which one current most-significant digit is bigger
    }
    return 0;
}
uint8_t __BIGINT_EQUAL__(const bigInt *a, const bigInt *b) {
    if (a->sign != b->sign) return 0;
    if (a->n    != b->n)    return 0;
    for (int i = 0; i < a->n; ++i) if (a->limbs[i] != b->limbs[i]) return 0;
    return 1;
}
uint8_t __BIGINT_LESS__(const bigInt *a, const bigInt *b) {
    if (a->sign != b->sign) return a->sign < b->sign;
    if (a->n    != b->n)    return a->n < b->n;
    for (int i = a->n - 1; i >= 0; --i) {
        if (a->limbs[i] != b->limbs[i]) return (a->limbs[i] < b->limbs[i]) ^ (a->sign < 0);
    }
    return 0;
}
uint8_t __BIGINT_MORE__(const bigInt *a, const bigInt *b) {
    if (a->sign != b->sign) return a->sign  > b->sign;
    if (a->n    != b->n)    return a->n     > b->n;
    for (int i = a->n - 1; i >= 0; --i) {
        if (a->limbs[i] != b->limbs[i]) return (a->limbs[i] > b->limbs[i]) ^ (a->sign > 0);
    }
    return 0;
}
uint8_t __BIGINT_LESS_OR_EQUAL__(const bigInt *a, const bigInt *b) { 
    return (__BIGINT_EQUAL__(a, b) || __BIGINT_LESS__(a, b)); 
}
uint8_t __BIGINT_MORE_OR_EQUAL__(const bigInt *a, const bigInt *b) { 
    return (__BIGINT_EQUAL__(a, b) || __BIGINT_MORE__(a, b)); 
}

/* ------------------ MAGNITUDE ARITHMETIC -------------------- */
/* - IMPORTANT NOTE / CLARIFICATION
*       +) This arithmetic layer exclusively handles pure,
*          magnituded arithmetic (|a| with |b|)
*       +) This function assumes the average, normal case, 
*          and ignores special cases (handled by their signed counterparts)
*       +) Therefore, it assumes inputs to be automatically:
*           o) non-negative
*           o) non-zero or value coincide with a fast-path
*           o) normalized
*           o) no invalid/illegal operations (eg: division by 0)
*           o) the output integer (res) is capable of storing the output
*       -----> +) These functions are not aliased in bigInt.h with a more user-friendly name, 
*                 as they are never meant to be used on the surface level
*       +) Note: 
*           These functions also follows the "Functional" model API, 
*           which means it copies the final output, and pastes it into a different, newly created bigInt
*
*       -----> WARNING!: 
*               THESE FUNCTION (IF EVER USED ON THE SURFACE) ARE TO BE USED WITH 
*               THE UTMOST CAUTION TO PREVENT FATAL ERRORS OR MEMORY LEAKS 
*/
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
    __BIGINT_NORMALIZE__(res);
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
    __BIGINT_NORMALIZE__(res); // Might leave leading/trailing zeros after subtraction 
                               //  ---> Decrease amount of used limbs (n) to avoid arithmetic errors
}
void __BIGINT_MAGNITUDED_MUL_UI64__(bigInt *res, const bigInt *x, const uint64_t val) {
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
    __BIGINT_NORMALIZE__(res);
}
void __BIGINT_MAGNITUDED_DIV_UI64__(bigInt *res, const bigInt *x, const uint64_t val) {
    assert(val != 0); // Checks for invalid operation ( x / 0 )
    uint64_t remainder = 0;
    for (size_t i = x->n; i-- > 0;) {
        res->limbs[i] = __DIV_HELPER_128_64__(remainder << sizeof(uint64_t) * 8, x->limbs[i], 
                                              val, &remainder);
    }
    __BIGINT_NORMALIZE__(x);
}
void __BIGINT_MAGNITUDED_MOD_UI64__(bigInt *res, const bigInt *x, const uint64_t val) {
    __BIGINT_NORMALIZE__(x);
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
    __BIGINT_NORMALIZE__(res);
}
void __BIGINT_MAGNITUDED_SUB__(bigInt *res, const bigInt *a, const bigInt *b) {
    __BIGINT_ENSURE_CAPACITY__(res, a->n);
    uint64_t borrow = 0;
    for (size_t i = 0; i < a->n; ++i) {
        uint64_t y = (i < b->n) ? b->limbs[i] : 0;
        res->limbs[i] = __SUB_UI64__(a->limbs[i], y, &borrow); // Do single-limb subtraction with borrow ---> Stores the borrow
    }
    res->n = a->n;
    __BIGINT_NORMALIZE__(res); // Subtraction may leave trailing/leading 0s ---> Normalize
}
void __BIGINT_MAGNITUDED_MUL__(bigInt *res, const bigInt *a, const bigInt *b) {
    __BIGINT_ENSURE_CAPACITY__(res, a->n + b->n);
    memset(res->limbs, 0, (a->n + b->n) * sizeof(uint64_t)); // Set every bytes to 0 in res, basically CALLOC() without the MALLOC()
    // Implementing schoolbook multiplication, treating each limb like a digit
    // -----> Inner loop access each limb of b and multiplying by 1 limb of a before going to the next a's limb
    for (size_t i = 0; i < a->n; ++i) {
        uint64_t carry = 0;
        for (size_t j = 0; j < b->n; ++j) {
            uint64_t low, high;
            __MUL_UI64__(a->limbs[i], b->limbs[j], &low, &high);
            // Stored and calculated kinda in a staircase pattern seen in the sums of Schoolbook
            uint64_t sum =      res->limbs[i + j] /* Potential data from last iteration */ 
                            +   low /* Lower half (64 bit) */ 
                            +   carry /* Carry from the last iteration */;

            carry =     high /* The remaining half (64 bit) */ 
                    +   (sum < low) /* Overflow from adding the lower half */ 
                    +   (sum < carry) /* Overflow from adding the last iteration's carry */;

            res->limbs[i + j] = sum; // Apply the new sum
        }
        res->limbs[i + b->n] += carry; // Add the remaining carry to the largest, 
                                       // most significant limb of the current sum
    }
    res->n = a->n + b->n;
    __BIGINT_NORMALIZE__(res);
}
void __BIGINT_MAGNITUDED_DIV__(bigInt *res, const bigInt *a, const bigInt *b) {
    __BIGINT_NORMALIZE__(res);
}
void __BIGINT_MAGNITUDED_MOD__(bigInt *res, const bigInt *a, const bigInt *b) {
    __BIGINT_NORMALIZE__(res);
}

/* ------------------- MUTATIVE ARITHMETIC -------------------- */
void __BIGINT_MUT_ADD_UI64__(bigInt *x, int64_t val) {
    int8_t val_sign = (val < 0) ? -1 : 1;
    if (x->sign == val_sign) {
        if (x->n == 0 && val == 0) __BIGINT_RESET__(x);
        else { bigInt __TEMP_BUFF__; 
               __BIGINT_EMPTY_INIT__(&__TEMP_BUFF__);
               __BIGINT_MAGNITUDED_ADD_UI64__(&__TEMP_BUFF__, x, val);
               __BIGINT_COPY_INTO__(&__TEMP_BUFF__, x); }
    }
    else {
        int8_t __compare_res__ = __BIGINT_COMPARE_MAGNITUDE_UI64__(x, val);
        if (__compare_res__ == 0) __BIGINT_RESET__(x);
        else if ( __compare_res__ > 0 ) { bigInt __TEMP_BUFF__; 
                                          __BIGINT_EMPTY_INIT__(&__TEMP_BUFF__);
                                          __BIGINT_MAGNITUDED_SUB_UI64__(&__TEMP_BUFF__, x, val);
                                          __BIGINT_COPY_INTO__(&__TEMP_BUFF__, x); }
    }
}
void __BIGINT_MUT_SUB_UI64__(bigInt *x, int64_t val) {}
void __BIGINT_MUT_MUL_UI64__(bigInt *x, int64_t val) {}
void __BIGINT_MUT_DIV_UI64__(bigInt *x, int64_t val) {}
void __BIGINT_MUT_MOD_UI64__(bigInt *x, int64_t val) {}

/* ------------------ FUNCTIONAL ARITHMETIC ------------------- */
void __BIGINT_ADD__(bigInt *res, const bigInt *x, const bigInt *y) {
    if (x->sign == y->sign) { // Same sign
        if (x->n == 0 && y->n == 0) __BIGINT_EMPTY_INIT__(res);
        else { __BIGINT_MAGNITUDED_ADD__(res, x, y); res->sign = x->sign; }
        /* Two scenarios: +x + +y ----> x + y
        *                 -x + -y ----> -x - y ----> -|x + y| */
    } 
    else { // Different signs
        int8_t __compare_res__ = __BIGINT_COMPARE_MAGNITUDE__(x, y);
        if (__compare_res__ == 0) __BIGINT_EMPTY_INIT__(res); // __BIGINT_EMPTY_INIT__ instead of __BIGINT_WIPE__ because we're creating a new bigInt
        else if(__compare_res__ > 0) { __BIGINT_MAGNITUDED_SUB__(res, x, y); res->sign = x->sign; }
        else { __BIGINT_MAGNITUDED_SUB__(res, y, x); res->sign = y->sign; }
    }
}
void __BIGINT_SUB__(bigInt *res, const bigInt *x, const bigInt *y) {
    bigInt neg = *y; // Copy only
    neg.sign   = -y->sign;
    __BIGINT_ADD__(x, &neg, res);   /* Function not really neccesary, 
                                       could've manually negate y when calling ADD */
}
void __BIGINT_MUL__(bigInt *res, const bigInt *x, const bigInt *y) {
    if (x->n == 0 || y->n == 0) { __BIGINT_EMPTY_INIT__(res); return; }
    __BIGINT_MAGNITUDED_MUL__(res, x, y); res->sign = x->sign * y->sign;
}
void __BIGINT_DIV__(bigInt *res, const bigInt *x, const bigInt *y) {
}
void __BIGINT_MOD__(bigInt *res, const bigInt *x, const bigInt *y) {

}

/* ------------------------- UTILITIES ------------------------- */
// o- Exact Copy on the byte level (Same value + Capacity)
void __BIGINT_NORMALIZE__(bigInt *x) {
    while (x->n > 0 && x->limbs[x->n - 1] == 0) --x->n; // Delete trailing/leading zeros
    if (x->n == 0) x->sign = 1; // Guarantees 0, not -0
}
void __BIGINT_ENSURE_CAPACITY__(bigInt *x, size_t k) {
    if (x->cap >= k) return;
    size_t new_cap = (x->cap == 0) ? 1 : x->cap;
    while (new_cap < k) new_cap *= 2; /* Capacity doubles instead of incrementation, 
                                         ---> Ensure less reallocation ---> Enhanced performance */
    uint64_t *__BUFFER_P = realloc(x->limbs, new_cap * sizeof(uint64_t));
    assert(!__BUFFER_P); // Check if __BUFFER_P is empty, if yes, abort()
    x->limbs = __BUFFER_P;
    x->cap   = new_cap;
}
void __BIGINT_RESET__(bigInt *x) {
    /* Different from __BIGINT_EMPTY_INIT__ because:
    *    +) Keeps *limbs pointing to its block of space
    *    +) Keeps the current capacity
    *       -------> The block stills exists, just resseted back to 0
    *       -------> Preferrably used over __BIGINT_EMPTY_INIT__ 
    *                in Mutative arithmetics to saves time + Reusable Capacity  */
   if (x->limbs && x->n > 0) memset(x->limbs, 0, x->n * sizeof(uint64_t));
    x->n    = 0;
    x->sign = 1;
}
uint8_t __BIGINT_COPY__(const bigInt *source__, bigInt *copycat__) {
    if (copycat__ == source__) return 0; // Same input ---> Unneccesary, fail X
    __BIGINT_FREE__(copycat__); // Free the space inside copycat__ for copying
    if (source__->n == 0) { copycat__->limbs = NULL;
                            copycat__->n     = 0;
                            copycat__->cap   = 0; 
                            copycat__->sign  = source__->sign; return 0; } // Empty initializing if copycat__ copies a blank value
    copycat__->limbs = malloc(source__->cap * sizeof(uint64_t)); // Initialize a block with equal elements as source__ of size 64 bits
    if (!copycat__->limbs) { copycat__->n = 0; return 1; } // Handle allocating an empty block for limbs
    for (size_t i = 0; i < source__->cap; ++i) copycat__->limbs[i] = source__->limbs[i];
                                               copycat__->n        = source__->n;
                                               copycat__->cap      = source__->cap;
                                               copycat__->sign     = source__->sign; return 0;
    // ---> Initialize/Copy values from source__
    // ---> Hence the usage of MALLOC() (Unintialized array saves time ---> Initialized later anyways)
}
// o- Copy value, Keep Capacity (Fails when value > capacity)
uint8_t __BIGINT_COPY_INTO__(const bigInt *source__, bigInt *copycat__) {
    if (copycat__ == source__) return 0;
    if (source__->n == 0) { copycat__->n     = 0;
                            copycat__->sign  = source__->sign; return 0; }
    if (source__->n > copycat__->cap) return 1;
    for (size_t i = 0; i < source__->n; ++i) copycat__->limbs[i] = source__->limbs[i];
                                             copycat__->n        = source__->n;
                                             copycat__->sign     = source__->sign; return 0;
}
// o- Copy value, Keep Capacity (Truncate value by n - cap limbs ---> DOWNCASTING)
uint8_t __BIGINT_COPY_TRUNCINTO__(const bigInt *source__, bigInt *copycat__) {
    if (copycat__ == source__) return 0;
    if (source__->n == 0) { copycat__->n     = 0;
                            copycat__->sign  = source__->sign; return 0; }
    if (source__->n > copycat__->cap) {
        for (size_t i = 0; i < copycat__->n; ++i) copycat__->limbs[i] = source__->limbs[i];
                                                  copycat__->n        = copycat__->cap;
                                                  copycat__->sign     = source__->sign; return 0;
    }
    for (size_t i = 0; i < source__->n; ++i) copycat__->limbs[i] = source__->limbs[i];
                                             copycat__->n        = source__->n;
                                             copycat__->sign     = source__->sign; return 0;
}
// Shortcut for the above functions
bigInt __BIGINT_SHORT_COPY__(const bigInt *source__) {
    bigInt copycat__ = {0}; __BIGINT_COPY__(source__, &copycat__);
    return copycat__;
}
bigInt __BIGINT_SHORT_COPY_INTO__(const bigInt *source__) {
    bigInt copycat__ = {0}; __BIGINT_COPY_INTO__(source__, &copycat__);
    return copycat__;
}
bigInt __BIGINT_SHORT_TRUNCINTO__(const bigInt *source__) {
    bigInt copycat__ = {0}; __BIGINT_COPY_TRUNCINTO__(source__, &copycat__);
    return copycat__;
}

/* ------------------------ SPECIAL UTILITIES --------------------- */
/* Note: This category is reserved only for function used for:
*    +) Specific, one-time use, internal tasks
*    +) Reducing bottlenecks and boilerplate compared to similar functions
*    +) Etcetera
*  ----> This category of functions is not to be used on the surface-level interface
*        as it design DOES NOT follow the structure of the other functions
*/
void __BIGINT_SPECIAL_ADD_UI64__(bigInt *x, uint64_t val) { // Mutative, Magnitude Addition (reduce bottleneck & boilerplate)
    if (val == 0) return;
    uint64_t carry = val;
    for (size_t i = 0; (carry & i < x->n) ; ++i) { x->limbs[i] = __ADD_UI64__(x->limbs[i], carry, &carry); }
    if (carry) { __BIGINT_ENSURE_CAPACITY__(x, x->n + 1);
                 x->limbs[x->n++] = carry; }
    __BIGINT_NORMALIZE(x);
}
void __BIGINT_SPECIAL_MUL_UI64__(bigInt *x, uint64_t val) { // Mutative, Magnitude Multiplication (reduce bottleneck & boilerplate)
    if (val == 0) { __BIGINT_RESET__(x); return; } // x * 0 = 0 ---> Reset back to 0
    if (val == 1) { return; } // x * 1 = x ----> Stays the same, no operation ---> Sames time and space
    __BIGINT_ENSURE_CAPACITY__(x, x->n + 1);
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
    __BIGINT_NORMALIZE__(x);
}


/* ----------------------------------------    --   TESTING LAB    --   ----------------------------------------- */
int main(int argc, char *argv[], char *envp[]) {

    return 0;
}