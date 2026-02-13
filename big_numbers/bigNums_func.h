#ifndef DNML_BIGNUM_FUNC
#define DNML_BIGNUM_FUNC





#ifdef __cplusplus
extern "C" {
#endif

#include "../system/include.h"
#include "../sconfigs/settings.h"
#include "../sconfigs/arena.h"
#include "../sconfigs/dnml_status.h"

#include "../intrinsics/intrinsics.h"
#include "../intrinsics/limb_math.h"
#include "../calculation_algorithms/calculation.h"
#include "../internal_utils/util.h"
#include "bigNums.h"

// Providing for
#include "big_numbers.h"

/* ------------------------------------    --   INTEGER FUNCTIONS    --   --------------------------------------- */
/* Attribute Explanation:
*   +) sign     (uint8_t)       : Stores the sign (negative or positive)
*   +) limbs    (*uint64_t)     : Pointer to each limb that holds part of the bigInt number
*   +) n        (size_t)        : Number of currently used limbs (used to determine the bigInt value and in arithmetic)
*   +) cap      (size_t)        : Essentially the bigInt object/number's maximum capacity 
*/

/* Terms Explanation:
*   +) Mutative Model: Changes an existing bigInt object/variable value in place (Eg: x += 10; )
*   +) Functional Model: -) Creates a new variable with the value of the expression (Eg: int x = 5 + 10; ) 
*                        -) This transfer ownership of the allocated limbs to the caller, 
                            forcing a manual deletion of the object (See more detailed explanation below on Functional Arithmetic)
*/
/* ------------- CONSTRUCTORS & DESCTRUCTORS -------------- */
void __BIGINT_EMPTY_INIT__(bigInt *__bigInteger); // Default Constructor
void __BIGINT_FREE__(bigInt *x); // Destructor
void __BIGINT_LIMBS_INIT__(bigInt *__bigInteger, size_t __fixed_size);
void __BIGINT_STANDARD_INIT__(bigInt *__bigInteger, const bigInt __preBigInt);
void __BIGINT_UI64_INIT__(bigInt *__bigInteger, uint64_t __unsigned_int);
void __BIGINT_I64_INIT__(bigInt *__bigInteger, int64_t __signed_int);
void __BIGINT_LD_INIT__(bigInt *__bigInteger, long double __float );

#define bigInt_free __BIGINT_FREE__
#define bigInt_init(initializing, initializer) \
    _Generic((initializer)                                                  \
        /* Signed, INTEGER Initialization */                                \
        char:                       __BIGINT_I64_INIT__,                    \
        int:                        __BIGINT_I64_INIT__,                    \
        long:                       __BIGINT_I64_INIT__,                    \
        long long:                  __BIGINT_I64_INIT__,                    \
        int8_t:                     __BIGINT_I64_INIT__,                    \
        int16_t:                    __BIGINT_I64_INIT__,                    \
        int32_t:                    __BIGINT_I64_INIT__,                    \
        int64_t:                    __BIGINT_I64_INIT__,                    \
                                                                            \
        /* Unsigned, INTEGER Initialization */                              \
        unsigned char:              __BIGINT_UI64_INIT__,                   \
        unsigned int:               __BIGINT_UI64_INIT__,                   \
        unsigned long:              __BIGINT_UI64_INIT__,                   \
        unsigned long long:         __BIGINT_UI64_INIT__,                   \
        uint8_t:                    __BIGINT_UI64_INIT__,                   \
        uint16_t:                   __BIGINT_UI64_INIT__,                   \
        uint32_t:                   __BIGINT_UI64_INIT__,                   \
        uint64_t:                   __BIGINT_UI64_INIT__,                   \
                                                                            \
        /* Float Initialization */                                          \
        float:                      __BIGINT_LD_INIT__,                     \
        double:                     __BIGINT_LD_INIT__,                     \
        long double:                __BIGINT_LD_INIT__,                     \
                                                                            \
        /* BigInt Initialization */                                         \
        bigInt                     __BIGINT_STANDARD_INIT__,                \
        size_t                     __BIGINT_LIMBS_INIT__,                   \
                                                                            \
        /* Empty Initialization */                                          \
        default:                    __BIGINT_EMPTY_INIT__                   \
)(initializing, initializer);


//* ------------------------ ASSIGNMENTS ------------------------ */
void __BIGINT_SET_BIGINT__(const bigInt x, bigInt *receiver);
void __BIGINT_SET_BIGINT_SAFE__(const bigInt x, bigInt *receiver);
/* --------- BigInt --> Primitive Types --------- */
void __BIGINT_SET_UI64__(const bigInt x, uint64_t *receiver);
void __BIGINT_SET_I64__(const bigInt x, int64_t *receiver);
void __BIGINT_SET_LD__(const bigInt x, long double *receiver);
bigint_status __BIGINT_SET_UI64_SAFE__(const bigInt x, uint64_t *receiver);
bigint_status __BIGINT_SET_I64_SAFE__(const bigInt x, int64_t *receiver);
bigint_status __BIGINT_SET_LD_SAFE__(const bigInt x, long double *receiver);
/* --------- Primitive Types --> BigInt --------- */
void __BIGINT_GET_UI64__(uint64_t x, bigInt *receiver);
void __BIGINT_GET_I64__(int64_t x, bigInt *receiver);
void __BIGINT_GET_LD__(long double x, bigInt *receiver);
bigint_status __BIGINT_GET_LD_SAFE__(long double x, bigInt *receiver);

#define bigInt_set(receiver, giver) \
    _Generic((receiver)                                         \
        /* Signed, INTEGER Variants */                          \
        char*:                       __BIGINT_SET_I64__,        \
        int*:                        __BIGINT_SET_I64__,        \
        long*:                       __BIGINT_SET_I64__,        \
        long long*:                  __BIGINT_SET_I64__,        \
        int8_t*:                     __BIGINT_SET_I64__,        \
        int16_t*:                    __BIGINT_SET_I64__,        \
        int32_t*:                    __BIGINT_SET_I64__,        \
        int64_t*:                    __BIGINT_SET_I64__,        \
                                                                \
        /* Unsigned, INTEGER Variants */                        \
        unsigned char*:              __BIGINT_SET_UI64__,       \
        unsigned int*:               __BIGINT_SET_UI64__,       \
        unsigned long*:              __BIGINT_SET_UI64__,       \
        unsigned long long*:         __BIGINT_SET_UI64__,       \
        uint8_t*:                    __BIGINT_SET_UI64__,       \
        uint16_t*:                   __BIGINT_SET_UI64__,       \
        uint32_t*:                   __BIGINT_SET_UI64__,       \
        uint64_t*:                   __BIGINT_SET_UI64__,       \
                                                                \
        /* Floating Point Variants */                           \
        float*:                      __BIGINT_SET_LD__,         \
        double*:                     __BIGINT_SET_LD__,         \
        long double*:                __BIGINT_SET_LD__,         \
                                                                \
        /* Normal/BigInt Variant */                             \
        bigInt*                     __BIGINT_SET_BIGINT__       \
)(receiver, giver) /* Don't accept no input */


//* ------------------------ CONVERSIONS ------------------------ */
/* --------- BigInt --> Primitive Types --------- */
uint64_t __BIGINT_TO_UI64__(const bigInt x);
int64_t __BIGINT_TO_I64__(const bigInt x);
long double __BIGINT_TO_LD_(const bigInt x);
uint64_t __BIGINT_TO_UI64_SAFE__(const bigInt x, bigint_status *err);
int64_t __BIGINT_TO_I64_SAFE__(const bigInt x, bigint_status *err);
long double __BIGINT_TO_LD_SAFE_(const bigInt x, bigint_status *err);
/* --------- Primitive Types --> BigInt --------- */
bigInt __BIGINT_FROM_UI64__(uint64_t x);
bigInt __BIGINT_FROM_I64__(int64_t x);
bigInt __BIGINT_FROM_LD_(long double x);
bigInt __BIGINT_FROM_LD_SAFE__(long double x);



//* -------------------- BITWISE OPERATIONS --------------------- */
bigInt __BIGINT_NOT__(const bigInt x);
bigInt __BIGINT_RSHIFT__(const bigInt x, size_t k);
bigInt __BIGINT_LSHIFT__(const bigInt x, size_t k);
void __BIGINT_MUT_RSHIFT__(bigInt *x, size_t k);
void __BIGINT_MUT_LSHIFT__(bigInt *x, size_t k);
/* ------------- Mutative, Fixed-width ------------- */
void __BIGINT_MUT_AND_UI64__  (bigInt *x, uint64_t y);
void __BIGINT_MUT_NAND_UI64__ (bigInt *x, uint64_t y);
void __BIGINT_MUT_OR_UI64__   (bigInt *x, uint64_t y);
void __BIGINT_MUT_NOR_UI64__  (bigInt *x, uint64_t y);
void __BIGINT_MUT_XOR_UI64__  (bigInt *x, uint64_t y);
void __BIGINT_MUT_XNOR_UI64__ (bigInt *x, uint64_t y);
void __BIGINT_MUT_AND__  (bigInt *x, const bigInt y);
void __BIGINT_MUT_NAND__ (bigInt *x, const bigInt y);
void __BIGINT_MUT_OR__   (bigInt *x, const bigInt y);
void __BIGINT_MUT_NOR__  (bigInt *x, const bigInt y);
void __BIGINT_MUT_XOR__  (bigInt *x, const bigInt y);
void __BIGINT_MUT_XNOR__ (bigInt *x, const bigInt y);
/* ------------- Mutative, Explicit-width ------------- */
void __BIGINT_MUT_EX_AND_UI64__  (bigInt *x, uint64_t val, size_t range);
void __BIGINT_MUT_EX_NAND_UI64__ (bigInt *x, uint64_t val, size_t range);
void __BIGINT_MUT_EX_OR_UI64__   (bigInt *x, uint64_t val, size_t range);
void __BIGINT_MUT_EX_NOR_UI64__  (bigInt *x, uint64_t val, size_t range);
void __BIGINT_MUT_EX_XOR_UI64__  (bigInt *x, uint64_t val, size_t range);
void __BIGINT_MUT_EX_XNOR_UI64__ (bigInt *x, uint64_t val, size_t range);
void __BIGINT_MUT_EX_AND_I64__  (bigInt *x, int64_t val, size_t range);
void __BIGINT_MUT_EX_NAND_I64__ (bigInt *x, int64_t val, size_t range);
void __BIGINT_MUT_EX_OR_I64__   (bigInt *x, int64_t val, size_t range);
void __BIGINT_MUT_EX_NOR_I64__  (bigInt *x, int64_t val, size_t range);
void __BIGINT_MUT_EX_XOR_I64__  (bigInt *x, int64_t val, size_t range);
void __BIGINT_MUT_EX_XNOR_I64__ (bigInt *x, int64_t val, size_t range);
void __BIGINT_MUT_EX_AND__   (bigInt *x, const bigInt y, size_t range);
void __BIGINT_MUT_EX_NAND__  (bigInt *x, const bigInt y, size_t range);
void __BIGINT_MUT_EX_OR__    (bigInt *x, const bigInt y, size_t range);
void __BIGINT_MUT_EX_NOR__   (bigInt *x, const bigInt y, size_t range);
void __BIGINT_MUT_EX_XOR__   (bigInt *x, const bigInt y, size_t range);
void __BIGINT_MUT_EX_XNOR__  (bigInt *x, const bigInt y, size_t range);
/* ------------- Functional, Fixed-width ------------- */
bigInt __BIGINT_AND_UI64__  (const bigInt x, uint64_t val);
bigInt __BIGINT_NAND_UI64__ (const bigInt x, uint64_t val);
bigInt __BIGINT_OR_UI64__   (const bigInt x, uint64_t val);
bigInt __BIGINT_NOR_UI64__  (const bigInt x, uint64_t val);
bigInt __BIGINT_XOR_UI64__  (const bigInt x, uint64_t val);
bigInt __BIGINT_XNOR_UI64__ (const bigInt x, uint64_t val);
bigInt __BIGINT_AND__   (const bigInt x, const bigInt y);
bigInt __BIGINT_NAND__  (const bigInt x, const bigInt y);
bigInt __BIGINT_OR__    (const bigInt x, const bigInt y);
bigInt __BIGINT_NOR__   (const bigInt x, const bigInt y);
bigInt __BIGINT_XOR__   (const bigInt x, const bigInt y);
bigInt __BIGINT_XNOR__  (const bigInt x, const bigInt y);
/* ------------- Functional, Explicit-width ------------- */
bigInt __BIGINT_EX_AND_UI64__  (const bigInt x, uint64_t val, size_t width);
bigInt __BIGINT_EX_NAND_UI64__ (const bigInt x, uint64_t val, size_t width);
bigInt __BIGINT_EX_OR_UI64__   (const bigInt x, uint64_t val, size_t width);
bigInt __BIGINT_EX_NOR_UI64__  (const bigInt x, uint64_t val, size_t width);
bigInt __BIGINT_EX_XOR_UI64__  (const bigInt x, uint64_t val, size_t width);
bigInt __BIGINT_EX_XNOR_UI64__ (const bigInt x, uint64_t val, size_t width);
bigInt __BIGINT_EX_AND_I64__  (const bigInt x, int64_t val, size_t width);
bigInt __BIGINT_EX_NAND_I64__ (const bigInt x, int64_t val, size_t width);
bigInt __BIGINT_EX_OR_I64__   (const bigInt x, int64_t val, size_t width);
bigInt __BIGINT_EX_NOR_I64__  (const bigInt x, int64_t val, size_t width);
bigInt __BIGINT_EX_XOR_I64__  (const bigInt x, int64_t val, size_t width);
bigInt __BIGINT_EX_XNOR_I64__ (const bigInt x, int64_t val, size_t width);
bigInt __BIGINT_EX_AND__   (const bigInt x, const bigInt y, size_t width);
bigInt __BIGINT_EX_NAND__  (const bigInt x, const bigInt y, size_t width);
bigInt __BIGINT_EX_OR__    (const bigInt x, const bigInt y, size_t width);
bigInt __BIGINT_EX_NOR__   (const bigInt x, const bigInt y, size_t width);
bigInt __BIGINT_EX_XOR__   (const bigInt x, const bigInt y, size_t width);
bigInt __BIGINT_EX_XNOR__  (const bigInt x, const bigInt y, size_t width);



//* ------------------------ COMPARISONS ------------------------ */
int8_t __BIGINT_COMPARE_MAGNITUDE_UI64__(const bigInt *x, const uint64_t val);
int8_t __BIGINT_COMPARE_MAGNITUDE__(const bigInt *a, const bigInt *b);
/* --------------- Integer - I64 --------------- */
uint8_t __BIGINT_EQUAL_I64__(const bigInt x, const int64_t val);
uint8_t __BIGINT_LESS_I64__(const bigInt x, const int64_t val);
uint8_t __BIGINT_MORE_I64__(const bigInt x, const int64_t val);
uint8_t __BIGINT_LESS_OR_EQUAL_I64__(const bigInt x, const int64_t val);
uint8_t __BIGINT_MORE_OR_EQUALL_I64__(const bigInt x, const int64_t val);
/* ---------- Unsigned Integer - UI64 ---------- */
uint8_t __BIGINT_EQUAL_UI64__(const bigInt x, const uint64_t val);
uint8_t __BIGINT_LESS_UI64__(const bigInt x, const uint64_t val);
uint8_t __BIGINT_MORE_UI64__(const bigInt x, const uint64_t val);
uint8_t __BIGINT_LESS_OR_EQUAL_UI64__(const bigInt x, const uint64_t val);
uint8_t __BIGINT_MORE_OR_EQUALL_UI64__(const bigInt x, const uint64_t val);
/* ------------------- BigInt ------------------ */
uint8_t __BIGINT_EQUAL__(const bigInt a, const bigInt b);
uint8_t __BIGINT_LESS__(const bigInt a, const bigInt b);
uint8_t __BIGINT_MORE__(const bigInt a, const bigInt b);
uint8_t __BIGINT_LESS_OR_EQUAL__(const bigInt a, const bigInt b);
uint8_t __BIGINT_MORE_OR_EQUAL__(const bigInt a, const bigInt b);



//* ------------------ MAGNITUDE MATHEMATICA -------------------- */
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
/* ------- Magnitude Arithmetic ------- */
void __BIGINT_MAGNITUDED_MUL_UI64__(bigInt *__outputted_bigInteger, const bigInt *__x__, const uint64_t __I64VAL);
void __BIGINT_MAGNITUDED_DIVMOD_UI64__
    (bigInt *__outputted_bigInteger_quotient, 
    uint64_t *__outputted_I64_remainder, 
    const bigInt *__x__, const uint64_t val);
void __BIGINT_MAGNITUDED_ADD__(bigInt *__outputted_bigInteger, const bigInt *__a__, const bigInt *__b__);
void __BIGINT_MAGNITUDED_SUB__(bigInt *__outputted_bigInteger, const bigInt *__a__, const bigInt *__b__);
void __BIGINT_MAGNITUDED_MUL__(bigInt *__outputted_bigInteger, const bigInt *__a__, const bigInt *__b__);
void __BIGINT_MAGNITUDED_DIVMOD__
    (bigInt *__outputted_bigInteger_quotient, 
    bigInt *__outputted_bigInteger_remainder, 
    const bigInt *__a__, const bigInt *__b__);
/* ------- Magnituded Core Number-Theoretic ------- */
uint64_t ___GCD_UI64___(uint64_t a, uint64_t b);
void __BIGINT_MAGNITUDED_GCD_UI64__(uint64_t *res, const bigInt *a, uint64_t val);
void __BIGINT_MAGNITUDED_LCM_UI64__(bigInt *res, const bigInt *a, uint64_t val);
void __BIGINT_MAGNITUDED_GCD__(bigInt *res, const bigInt *a, const bigInt *b);
void __BIGINT_MAGNITUDED_LCM__(bigInt *res, const bigInt *a, const bigInt *b);
void __BIGINT_MAGNITUDED_EUCMOD_UI64__(uint64_t *res, const bigInt *a, uint64_t modulus);
void __BIGINT_MAGNITUDED_EUCMOD__(bigInt *res, const bigInt *a, const bigInt *modulus);
void __BIGINT_MAGNITUDED_PRIMATEST__(const bigInt *x);
/* ------- Magnituded Modular Arithmetic ------- */
void __BIGINT_MAGNITUDED_MODADD__(bigInt *res, const bigInt *a, const bigInt *b, const bigInt *mod);
void __BIGINT_MAGNITUDED_MODSUB__(bigInt *res, const bigInt *a, const bigInt *b, const bigInt *mod);
void __BIGINT_MAGNITUDED_MODMUL__(bigInt *res, const bigInt *a, const bigInt *b, const bigInt *mod);
void __BIGINT_MAGNITUDED_MODDIV__(bigInt *res, const bigInt *a, const bigInt *b, const bigInt *mod);
void __BIGINT_MAGNITUDED_MODEXP__(bigInt *res, const bigInt *a, const bigInt *b, const bigInt *mod);
void __BIGINT_MAGNITUDED_MODSQR__(bigInt *res, const bigInt *a, const bigInt *b, const bigInt *mod);
void __BIGINT_MAGNITUDED_MODINV__(bigInt *res, const bigInt *a, const bigInt *b, const bigInt *mod);



//* -------------------- SIGNED ARITHMETIC --------------------- */
/*  - These arithmetic functions handles:
*       +) Special/Edge cases
*       +) Fast Paths
*       +) Normalization
*       +) Illegal Operation
*   - They are designed to improve performance by implementing fast paths, 
*     decrease boilerplate, and provide safe, public, surface-level interface for bigInt operations
*   - These function are included in two different sections below:
*       +) MUTATIVE ARITHMETIC      ---> In-place mutation of a variable                (Eg: x += 1     )
*       +) FUNCTIONAL ARITHMETIC    ---> Return a new copy of a value to be asigned     (Eg: x  = 1 + 2;)
*/
/* ------------------- Mutative Arithmetic -------------------- */
void __BIGINT_MUT_MUL_UI64__(bigInt *x, uint64_t val);
bigint_status __BIGINT_MUT_DIV_UI64__(bigInt *x, uint64_t val);
bigint_status __BIGINT_MUT_MOD_UI64__(bigInt *x, uint64_t val);
void __BIGINT_MUT_MUL_I64__(bigInt *x, int64_t val);
bigint_status __BIGINT_MUT_DIV_I64__(bigInt *x, int64_t val);
bigint_status __BIGINT_MUT_MOD_I64__(bigInt *x, int64_t val);
void __BIGINT_MUT_ADD__(bigInt *x, const bigInt y);
void __BIGINT_MUT_SUB__(bigInt *x, const bigInt y);
void __BIGINT_MUT_MUL__(bigInt *x, const bigInt y);
bigint_status __BIGINT_MUT_DIV__(bigInt *x, const bigInt y);
bigint_status __BIGINT_MUT_MOD__(bigInt *x, const bigInt y);
/* ------------------ Functional Arithmetic ------------------- */
bigInt __BIGINT_MUL_UI64__(const bigInt x, uint64_t val);
bigInt __BIGINT_DIV_UI64__(const bigInt x, uint64_t val, bigint_status *err);
bigInt __BIGINT_MOD_UI64__(const bigInt x, uint64_t val, bigint_status *err);
bigInt __BIGINT_MUL_I64__(const bigInt x, int64_t val);
bigInt __BIGINT_DIV_I64__(const bigInt x, int64_t val, bigint_status *err);
bigInt __BIGINT_MOD_I64__(const bigInt x, int64_t val, bigint_status *err);
bigInt __BIGINT_ADD__(const bigInt x, const bigInt y);
bigInt __BIGINT_SUB__(const bigInt x, const bigInt y);
bigInt __BIGINT_MUL__(const bigInt x, const bigInt y);
bigInt __BIGINT_DIV__(const bigInt x, const bigInt y, bigint_status *err);
bigInt __BIGINT_MOD__(const bigInt x, const bigInt y, bigint_status *err);


//* -------------------- SIGNED NUMBER-THEORETIC --------------------- */
/* -------------- Pure Number Theoretic -------------- */
uint64_t __BIGINT_GCD_UI64__(const bigInt x, uint64_t val);
int64_t __BIGINT_GCD_I64__(const bigInt x, int64_t val);
bigInt __BIGINT_GCD__(const bigInt x, const bigInt y);
bigInt __BIGINT_LCM_UI64__(const bigInt x, uint64_t val);
bigInt __BIGINT_LCM_I64__(const bigInt x, int64_t val);
bigInt __BIGINT_LCM__(const bigInt x, const bigInt y);
uint8_t __BIGINT_IS_PRIME__(const bigInt x);
/* ---------------- Modular Reduction ---------------- */
void __BIGINT_MUT_MODULO_UI64__(bigInt *x, uint64_t modulus);
void __BIGINT_MUT_MODULO_I64__(bigInt *x, int64_t modulus);
void __BIGINT_MUT_MODULO__(bigInt *x, const bigInt modulus);
uint64_t __BIGINT_MODULO_UI64__(const bigInt x, uint64_t modulus);
int64_t __BIGINT_MODULO_I64__(const bigInt x, int64_t modulus);
bigInt __BIGINT_MODULO__(const bigInt x, const bigInt modulus);
/* ---------------- SMALL Modular Arithmetic --------------- */
void __BIGINT_MUT_MODADD_UI64__(bigInt *x, const bigInt y, uint64_t modulus);
void __BIGINT_MUT_MODSUB_UI64__(bigInt *x, const bigInt y, uint64_t modulus);
void __BIGINT_MUT_MODADD__(bigInt *x, const bigInt y, const bigInt modulus);
void __BIGINT_MUT_MODSUB__(bigInt *x, const bigInt y, const bigInt modulus);
uint64_t __BIGINT_MODADD_UI64__(const bigInt x, const bigInt y, uint64_t modulus);
uint64_t __BIGINT_MODSUB_UI64__(const bigInt x, const bigInt y, uint64_t modulus);
bigInt __BIGINT_MODADD__(const bigInt x, const bigInt y, const bigInt modulus);
bigInt __BIGINT_MODSUB__(const bigInt x, const bigInt y, const bigInt modulus);
/* ---------------- LARGE Modular Arithmetic --------------- */
void __BIGINT_MUT_MODMUL_UI64_UI64__(bigInt *x, uint64_t y, uint64_t modulus);
void __BIGINT_MUT_MODDIV_UI64_UI64__(bigInt *x, uint64_t y, uint64_t modulus);
void __BIGINT_MUT_MODMUL_BI_UI64__(bigInt *x, const bigInt y, uint64_t modulus);
void __BIGINT_MUT_MODDIV_BI_UI64__(bigInt *x, const bigInt y, uint64_t modulus);
void __BIGINT_MUT_MODMUL_UI64_BI__(bigInt *x, uint64_t y, const bigInt modulus);
void __BIGINT_MUT_MODDIV_UI64_BI__(bigInt *x, uint64_t y, const bigInt modulus);
void __BIGINT_MUT_MODMUL__(bigInt *x, const bigInt y, const bigInt modulus);
void __BIGINT_MUT_MODDIV__(bigInt *x, const bigInt y, const bigInt modulus);
uint64_t __BIGINT_MODMUL_UI64_UI64__(const bigInt x, uint64_t y, uint64_t modulus);
uint64_t __BIGINT_MODDIV_UI64_UI64__(const bigInt x, uint64_t y, uint64_t modulus);
uint64_t __BIGINT_MODMUL_BI_UI64__(const bigInt x, const bigInt y, uint64_t modulus);
uint64_t __BIGINT_MODDIV_BI_UI64__(const bigInt x, const bigInt y, uint64_t modulus);
bigInt __BIGINT_MODMUL_UI64_BI__(const bigInt x, uint64_t y, const bigInt modulus);
bigInt __BIGINT_MODDIV_UI64_BI__(const bigInt x, uint64_t y, const bigInt modulus);
bigInt __BIGINT_MODMUL__(const bigInt x, const bigInt y, const bigInt modulus);
bigInt __BIGINT_MODDIV__(const bigInt x, const bigInt y, const bigInt modulus);
/* ---------------------- Modular Algebraic ------------------ */
void __BIGINT_MUT_MODEXP_UI64__(bigInt *x, const bigInt y, uint64_t modulus);
void __BIGINT_MUT_MODSQR_UI64__(bigInt *x, uint64_t modulus);
void __BIGINT_MUT_MODINV_UI64__(bigInt *x, uint64_t modulus);
void __BIGINT_MUT_MODEXP__(bigInt *x, const bigInt y, const bigInt modulus);
void __BIGINT_MUT_MODSQR__(bigInt *x, const bigInt modulus);
void __BIGINT_MUT_MODINV__(bigInt *x, const bigInt modulus);
uint64_t __BIGINT_MODEXP_UI64__(const bigInt x, const bigInt y, uint64_t modulus);
uint64_t __BIGINT_MODSQR_UI64__(const bigInt x, uint64_t modulus);
uint64_t __BIGINT_MODINV_UI64__(const bigInt x, uint64_t modulus);
bigInt __BIGINT_MODEXP__(const bigInt x, const bigInt y, const bigInt modulus);
bigInt __BIGINT_MODSQR__(const bigInt x, const bigInt modulus);
bigInt __BIGINT_MODINV__(const bigInt x, const bigInt modulus);

//* ------------------------- COPIES --------------------------- */
/* -------------  Mutative SMALL Copies ------------- */
void __BIGINT_MUT_COPY_UI64__(bigInt *dst__, uint64_t source__);
void __BIGINT_MUT_COPY_DEEP_UI64__(bigInt *dst__, uint64_t source__);
void __BIGINT_MUT_COPY_I64__(bigInt *dst__, int64_t source__);
void __BIGINT_MUT_COPY_DEEP_I64__(bigInt *dst__, int64_t source__);
/* -------------  Mutative LARGE Copies ------------- */
void __BIGINT_MUT_COPY_LD__(bigInt *dst__, long double source__);
void __BIGINT_MUT_COPY_DEEP_LD__(bigInt *dst__, long double source__);
void __BIGINT_MUT_COPY_OVER_LD__(bigInt *dst__, long double source__);
void __BIGINT_MUT_COPY_TRUNCOVER_LD__(bigInt *dst__, long double source__);
void __BIGINT_MUT_COPY__(bigInt *dst__, bigInt source__);
void __BIGINT_MUT_COPY_DEEP__(bigInt *dst__, bigInt source__);
void __BIGINT_MUT_COPY_OVER__(bigInt *dst__, bigInt source__);
void __BIGINT_MUT_COPY_TRUNCOVER__(bigInt *dst__, bigInt source__);
/* -------------  Functional SMALL Copies ------------- */
bigInt __BIGINT_COPY_UI64__(uint64_t source__);
bigInt __BIGINT_COPY_I64__(int64_t source__);
/* -------------  Functional LARGE Copies ------------- */
bigInt __BIGINT_COPY_LD__(long double source__);
bigInt __BIGINT_COPY_OVER_LD__(long double source__, size_t output_cap);
bigInt __BIGINT_COPY_TRUNCOVER_LD__(long double source__, size_t output_cap);
bigInt __BIGINT_COPY__(const bigInt source__);
bigInt __BIGINT_COPY_DEEP__(const bigInt source__);
bigInt __BIGINT_COPY_OVER__(const bigInt source__, size_t output_cap, bigint_status *err);
bigInt __BIGINT_COPY_TRUNCOVER__(const bigInt source__, size_t output_cap);

/* -------------------- GENERAL UTILITIES --------------------- */
inline void __BIGINT_CANONICALIZE__(bigInt *x);
void __BIGINT_NORMALIZE__(bigInt *x);
void __BIGINT_RESIZE__(bigInt *x, size_t k);
void __BIGINT_RESERVE__(bigInt *x, size_t k);
void __BIGINT_SHRINK__(bigInt *x, size_t k);
void __BIGINT_RESET__(bigInt *x);
static inline uint8_t __BIGINT_MUTATIVE_SUBJECT_VALIDATE__(bigInt *x);
static inline uint8_t __BIGINT_STATE_VALIDATE__(bigInt x);
inline uint8_t __BIGINT_VALIDATE__(bigInt x);
inline uint8_t __BIGINT_PVALIDATE__(bigInt *x);

#define bigInt_canonicalize     __BIGINT_CANONICALIZE__
#define bigInt_normalize        __BIGINT_NORMALIZE__
#define bigInt_resize           __BIGINT_RESIZE__
#define bigInt_reserve          __BIGINT_RESERVE__
#define bigInt_shrink           __BIGINT_SHRINK__
#define bigInt_reset            __BIGINT_RESET__

#ifdef __cplusplus
}
#endif




#endif
