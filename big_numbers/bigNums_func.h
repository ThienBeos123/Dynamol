#ifdef __cplusplus
extern "C" {
#endif

#include "../system/include.h"
#include "../intrinsics/intrinsics.h"
#include "../intrinsics/limb_math.h"
#include "../calculation_algorithms/calculation.h"
#include "../internal_utils/util.h"
#include "bigNums.h"

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
uint8_t __BIGINT_EMPTY_INIT__(bigInt *__bigInteger); // Default Constructor
uint8_t __BIGINT_FREE__(bigInt *x); // Destructor
uint8_t __BIGINT_LIMBS_INIT__(bigInt *__bigInteger, size_t __fixed_size); // Allocate __fixed_size space, No value initialization
uint8_t __BIGINT_STANDARD_INIT__(bigInt *__bigInteger, const bigInt __numeric_string); // Initialize from string ( >2^64 )
uint8_t __BIGINT_UI64_INIT__(bigInt *__bigInteger, uint64_t __unsigned_int); // Initialize from primitive uint ( 0 <= x <=2^64)
uint8_t __BIGINT_I64_INIT__(bigInt *__bigInteger, int64_t __signed_int); // Initialize from primitive int (-2^63 <= x <= 2^63 - 1 )
uint8_t __BIGINT_LD_INIT__(bigInt *__bigInteger, long double __float );

#define bigInt_free             __BIGINT_FREE__
#define bigInt_init(x, ...) \
    _Generic((__VA_ARGS__) /* __VA_ARGS__: Takes a variety of types */,     \
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
    )(x, ##__VA_ARGS__) /* ##__VA_ARGS__: __VA_ARGS__ + accepts no input */


//* ------------------------ ASSIGNMENTS ------------------------ */
/* --------- BigInt --> Primitive Types --------- */
uint8_t __BIGINT_SET_UI64__(const bigInt x, uint64_t *receiver);
uint8_t __BIGINT_SET_I64__(const bigInt x, int64_t *receiver);
uint8_t __BIGINT_SET_LD__(const bigInt x, long double *receiver);
uint8_t __BIGINT_SET_UI64_SAFE__(const bigInt x, uint64_t *receiver);
uint8_t __BIGINT_SET_I64_SAFE__(const bigInt x, int64_t *receiver);
uint8_t __BIGINT_SET_LD_SAFE__(const bigInt x, long double *receiver);
/* --------- Primitive Types --> BigInt --------- */
uint8_t __BIGINT_GET_UI64__(uint64_t x, bigInt *receiver);
uint8_t __BIGINT_GET_I64__(int64_t x, bigInt *receiver);
uint8_t __BIGINT_GET_LD__(long double x, bigInt *receiver);
uint8_t __BIGINT_GET_LD_SAFE__(long double x, bigInt *receiver);


//* ------------------------ CONVERSIONS ------------------------ */
/* --------- BigInt --> Primitive Types --------- */
uint64_t __BIGINT_TO_UI64__(const bigInt x);
int64_t __BIGINT_TO_I64__(const bigInt x);
long double __BIGINT_TO_LD_(const bigInt x);
int64_t __BIGINT_TO_I64_SAFE__(const bigInt x);
long double __BIGINT_TO_LD_SAFE_(const bigInt x);
/* --------- Primitive Types --> BigInt --------- */
bigInt __BIGINT_FROM_UI64__(uint64_t x);
bigInt __BIGINT_FROM_I64__(int64_t x);
bigInt __BIGINT_FROM_LD_(long double x);
bigInt __BIGINT_FROM_LD_SAFE__(long double x);


/* -------------------- BITWISE OPERATIONS --------------------- */




/* ------------------------ COMPARISONS ------------------------ */
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
uint8_t __BIGINT_MUT_MUL_UI64__(bigInt *x, uint64_t val);
uint8_t __BIGINT_MUT_DIV_UI64__(bigInt *x, uint64_t val);
uint8_t __BIGINT_MUT_MOD_UI64__(bigInt *x, uint64_t val);
uint8_t __BIGINT_MUT_MUL_I64__(bigInt *x, int64_t val);
uint8_t __BIGINT_MUT_DIV_I64__(bigInt *x, int64_t val);
uint8_t __BIGINT_MUT_MOD_I64__(bigInt *x, int64_t val);
uint8_t __BIGINT_MUT_ADD__(bigInt *x, const bigInt y);
uint8_t __BIGINT_MUT_SUB__(bigInt *x, const bigInt y);
uint8_t __BIGINT_MUT_MUL__(bigInt *x, const bigInt y);
uint8_t __BIGINT_MUT_DIV__(bigInt *x, const bigInt y);
uint8_t __BIGINT_MUT_MOD__(bigInt *x, const bigInt y);
/* ------------------ Functional Arithmetic ------------------- */
bigInt __BIGINT_MUL_UI64__(const bigInt x, uint64_t val);
bigInt __BIGINT_DIV_UI64__(const bigInt x, uint64_t val);
uint64_t __BIGINT_MOD_UI64__(const bigInt x, uint64_t val);
bigInt __BIGINT_MUL_I64__(const bigInt x, int64_t val);
bigInt __BIGINT_DIV_I64__(const bigInt x, int64_t val);
int64_t __BIGINT_MOD_I64__(const bigInt x, int64_t val);
bigInt __BIGINT_ADD__(const bigInt x, const bigInt y);
bigInt __BIGINT_SUB__(const bigInt x, const bigInt y);
bigInt __BIGINT_MUL__(const bigInt x, const bigInt y);
bigInt __BIGINT_DIV__(const bigInt x, const bigInt y);
bigInt __BIGINT_MOD__(const bigInt x, const bigInt y);


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
uint8_t __BIGINT_MUT_MODULO_UI64__(bigInt *x, uint64_t modulus);
uint8_t __BIGINT_MUT_MODULO_I64__(bigInt *x, int64_t modulus);
uint8_t __BIGINT_MUT_MODULO__(bigInt *x, const bigInt modulus);
uint64_t __BIGINT_MODULO_UI64__(const bigInt x, uint64_t modulus);
int64_t __BIGINT_MODULO_I64__(const bigInt x, int64_t modulus);
bigInt __BIGINT_MODULO__(const bigInt x, const bigInt modulus);
/* ---------------- SMALL Modular Arithmetic --------------- */
uint8_t __BIGINT_MUT_MODADD_UI64__(bigInt *x, const bigInt y, uint64_t modulus);
uint8_t __BIGINT_MUT_MODSUB_UI64__(bigInt *x, const bigInt y, uint64_t modulus);
uint8_t __BIGINT_MUT_MODADD__(bigInt *x, const bigInt y, const bigInt modulus);
uint8_t __BIGINT_MUT_MODSUB__(bigInt *x, const bigInt y, const bigInt modulus);
uint64_t __BIGINT_MODADD_UI64__(const bigInt x, const bigInt y, uint64_t modulus);
uint64_t __BIGINT_MODSUB_UI64__(const bigInt x, const bigInt y, uint64_t modulus);
bigInt __BIGINT_MODADD__(const bigInt x, const bigInt y, const bigInt modulus);
bigInt __BIGINT_MODSUB__(const bigInt x, const bigInt y, const bigInt modulus);
/* ---------------- LARGE Modular Arithmetic --------------- */
uint8_t __BIGINT_MUT_MODMUL_UI64_UI64__(bigInt *x, uint64_t y, uint64_t modulus);
uint8_t __BIGINT_MUT_MODDIV_UI64_UI64__(bigInt *x, uint64_t y, uint64_t modulus);
uint8_t __BIGINT_MUT_MODMUL_BI_UI64__(bigInt *x, const bigInt y, uint64_t modulus);
uint8_t __BIGINT_MUT_MODDIV_BI_UI64__(bigInt *x, const bigInt y, uint64_t modulus);
uint8_t __BIGINT_MUT_MODMUL_UI64_BI__(bigInt *x, uint64_t y, const bigInt modulus);
uint8_t __BIGINT_MUT_MODDIV_UI64_BI__(bigInt *x, uint64_t y, const bigInt modulus);
uint8_t __BIGINT_MUT_MODMUL__(bigInt *x, const bigInt y, const bigInt modulus);
uint8_t __BIGINT_MUT_MODDIV__(bigInt *x, const bigInt y, const bigInt modulus);
uint64_t __BIGINT_MODMUL_UI64_UI64__(const bigInt x, uint64_t y, uint64_t modulus);
uint64_t __BIGINT_MODDIV_UI64_UI64__(const bigInt x, uint64_t y, uint64_t modulus);
uint64_t __BIGINT_MODMUL_BI_UI64__(const bigInt x, const bigInt y, uint64_t modulus);
uint64_t __BIGINT_MODDIV_BI_UI64__(const bigInt x, const bigInt y, uint64_t modulus);
bigInt __BIGINT_MODMUL_UI64_BI__(const bigInt x, uint64_t y, const bigInt modulus);
bigInt __BIGINT_MODDIV_UI64_BI__(const bigInt x, uint64_t y, const bigInt modulus);
bigInt __BIGINT_MODMUL__(const bigInt x, const bigInt y, const bigInt modulus);
bigInt __BIGINT_MODDIV__(const bigInt x, const bigInt y, const bigInt modulus);
/* ---------------------- Modular Algebraic ------------------ */
uint8_t __BIGINT_MUT_MODEXP_UI64__(bigInt *x, const bigInt y, uint64_t modulus);
uint8_t __BIGINT_MUT_MODSQR_UI64__(bigInt *x, uint64_t modulus);
uint8_t __BIGINT_MUT_MODINV_UI64__(bigInt *x, uint64_t modulus);
uint8_t __BIGINT_MUT_MODEXP__(bigInt *x, const bigInt y, const bigInt modulus);
uint8_t __BIGINT_MUT_MODSQR__(bigInt *x, const bigInt modulus);
uint8_t __BIGINT_MUT_MODINV__(bigInt *x, const bigInt modulus);
uint64_t __BIGINT_MODEXP_UI64__(const bigInt x, const bigInt y, uint64_t modulus);
uint64_t __BIGINT_MODSQR_UI64__(const bigInt x, uint64_t modulus);
uint64_t __BIGINT_MODINV_UI64__(const bigInt x, uint64_t modulus);
bigInt __BIGINT_MODEXP__(const bigInt x, const bigInt y, const bigInt modulus);
bigInt __BIGINT_MODSQR__(const bigInt x, const bigInt modulus);
bigInt __BIGINT_MODINV__(const bigInt x, const bigInt modulus);

//* ------------------------- COPIES --------------------------- */
/* -------------  Mutative SMALL Copies ------------- */
uint8_t __BIGINT_MUT_COPY_UI64__(bigInt *dst__, uint64_t source__);
uint8_t __BIGINT_MUT_COPY_DEEP_UI64__(bigInt *dst__, uint64_t source__);
uint8_t __BIGINT_MUT_COPY_I64__(bigInt *dst__, int64_t source__);
uint8_t __BIGINT_MUT_COPY_DEEP_I64__(bigInt *dst__, int64_t source__);
/* -------------  Mutative LARGE Copies ------------- */
uint8_t __BIGINT_MUT_COPY_LD__(bigInt *dst__, long double source__);
uint8_t __BIGINT_MUT_COPY_DEEP_LD__(bigInt *dst__, long double source__);
uint8_t __BIGINT_MUT_COPY_OVER_LD__(bigInt *dst__, long double source__);
uint8_t __BIGINT_MUT_COPY_TRUNCOVER_LD__(bigInt *dst__, long double source__);
uint8_t __BIGINT_MUT_COPY__(bigInt *dst__, bigInt source__);
uint8_t __BIGINT_MUT_COPY_DEEP__(bigInt *dst__, bigInt source__);
uint8_t __BIGINT_MUT_COPY_OVER__(bigInt *dst__, bigInt source__);
uint8_t __BIGINT_MUT_COPY_TRUNCOVER__(bigInt *dst__, bigInt source__);
/* -------------  Functional SMALL Copies ------------- */
bigInt __BIGINT_COPY_UI64__(uint64_t source__);
bigInt __BIGINT_COPY_I64__(int64_t source__);
/* -------------  Functional LARGE Copies ------------- */
bigInt __BIGINT_COPY_LD__(long double source__);
bigInt __BIGINT_COPY_OVER_LD__(long double source__, size_t output_cap);
bigInt __BIGINT_COPY_TRUNCOVER_LD__(long double source__, size_t output_cap);
bigInt __BIGINT_COPY__(const bigInt source__);
bigInt __BIGINT_COPY_DEEP__(const bigInt source__);
bigInt __BIGINT_COPY_OVER__(const bigInt source__, size_t output_cap);
bigInt __BIGINT_COPY_TRUNCOVER__(const bigInt source__, size_t output_cap);

/* -------------------- GENERAL UTILITIES --------------------- */
inline void __BIGINT_CANONICALIZE__(bigInt *x);
void __BIGINT_NORMALIZE__(bigInt *x);
uint8_t __BIGINT_RESIZE__(bigInt *x, size_t k);
uint8_t __BIGINT_RESERVE__(bigInt *x, size_t k);
uint8_t __BIGINT_SHRINK__(bigInt *x, size_t k);
uint8_t __BIGINT_RESET__(bigInt *x);
static inline uint8_t __BIGINT_MUTATIVE_SUBJECT_VALIDATE__(bigInt *x);
static inline uint8_t __BIGINT_STATE_VALIDATE__(bigInt x);
inline uint8_t __BIGINT_VALIDATE__(bigInt x);
inline uint8_t __BIGINT_PVALIDATE__(bigInt *x);

#define bigInt_canonicalize     __BIGINT_CANONICALIZE__
#define bigInt_normalize        __BIGINT_NORMALIZE__
#define bigInt_reserve          __BIGINT_RESERVE__
#define bigInt_reset            __BIGINT_RESET__

#ifdef __cplusplus
}
#endif

