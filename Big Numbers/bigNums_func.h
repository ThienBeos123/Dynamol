#ifdef __cplusplus
extern "C" {
#endif

#include "bigNums.h"

/* ------------------------------------    --   INTEGER FUNCTIONS    --   --------------------------------------- */
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
/* ------------- CONSTRUCTORS & DESCTRUCTORS -------------- */
void __BIGINT_EMPTY_INIT__(bigInt *__bigInteger); // Default Constructor
void __BIGINT_FREE__(bigInt *x); // Destructor
uint8_t __BIGINT_LIMBS_INIT__(bigInt *__bigInteger, size_t __fixed_size); // Allocate __fixed_size space, No value initialization
uint8_t __BIGINT_STANDARD_INIT__(bigInt *__bigInteger, const char *__numeric_string); // Initialize from string ( >2^64 )
uint8_t __BIGINT_UNSIGNED_PRIMITVE_INIT__(bigInt *__bigInteger, uint64_t __unsigned_int); // Initialize from primitive uint ( 0 <= x <=2^64)
uint8_t __BIGINT_SIGNED_PRIMITIVE_INIT__(bigInt *__bigInteger, int64_t __signed_int); // Initialize from primitive int (-2^63 <= x <= 2^63 - 1 )
uint8_t __BIGINT_FLOAT_INIT__(bigInt *__bigInteger, long double __float );

#define bigInt_free             __BIGINT_FREE__
#define bigInt_init(x, ...) \
    _Generic((__VA_ARGS__) /* __VA_ARGS__: Takes a variety of types */,     \
        /* Signed, INTEGER Initialization */                                \
        char:                       __BIGINT_SIGNED_PRIMITIVE_INIT__,       \
        int:                        __BIGINT_SIGNED_PRIMITIVE_INIT__,       \
        long:                       __BIGINT_SIGNED_PRIMITIVE_INIT__,       \
        long long:                  __BIGINT_SIGNED_PRIMITIVE_INIT__,       \
        int8_t:                     __BIGINT_SIGNED_PRIMITIVE_INIT__,       \
        int16_t:                    __BIGINT_SIGNED_PRIMITIVE_INIT__,       \
        int32_t:                    __BIGINT_SIGNED_PRIMITIVE_INIT__,       \
        int64_t:                    __BIGINT_SIGNED_PRIMITIVE_INIT__,       \ 
                                                                            \
        /* Unsigned, INTEGER Initialization */                              \
        unsigned char:              __BIGINT_UNSIGNED_PRIMITVE_INIT__,      \
        unsigned int:               __BIGINT_UNSIGNED_PRIMITVE_INIT__,      \
        unsigned long:              __BIGINT_UNSIGNED_PRIMITVE_INIT__,      \
        unsigned long long:         __BIGINT_UNSIGNED_PRIMITVE_INIT__,      \
        uint8_t:                    __BIGINT_UNSIGNED_PRIMITVE_INIT__,      \
        uint16_t:                   __BIGINT_UNSIGNED_PRIMITVE_INIT__,      \
        uint32_t:                   __BIGINT_UNSIGNED_PRIMITVE_INIT__,      \
        uint64_t:                   __BIGINT_UNSIGNED_PRIMITVE_INIT__,      \
                                                                            \
        /* Float Initialization */                                          \
        float:                      __BIGINT_FLOAT_INIT__,                  \
        double:                     __BIGINT_FLOAT_INIT__,                  \
        long double:                __BIGINT_FLOAT_INIT__,                  \
                                                                            \
        /* String Initialization */                                         \
        char *:                     __BIGINT_STANDARD_INIT__,               \
        const char *:               __BIGINT_STANDARD_INIT__,               \
                                                                            \
        /* Empty Initialization */                                          \
        default:                    __BIGINT_EMPTY_INIT__                   \
    )(x, ##__VA_ARGS__) /* ##__VA_ARGS__: __VA_ARGS__ + accepts no input */


/* ------------------------ CONVERSIONS ------------------------ */




/* -------------------- BITWISE OPERATIONS --------------------- */




/* ------------------------ COMPARISONS ------------------------ */
int8_t __BIGINT_COMPARE_MAGNITUDE_UI64__(const bigInt *__x__, const uint64_t __I64VAL);
uint8_t __BIGINT_EQUAL_UI64__(const bigInt *__x__, const int64_t __I64VAL);
uint8_t __BIGINT_LESS_UI64__(const bigInt *__x__, const int64_t __I64VAL);
uint8_t __BIGINT_MORE_UI64__(const bigInt *__x__, const int64_t __I64VAL);
uint8_t __BIGINT_LESS_OR_EQUAL_UI64__(const bigInt *__x__, const int64_t __I64VAL);
uint8_t __BIGINT_MORE_OR_EQUALL_UI64__(const bigInt *__x__, const int64_t __I64VAL);
int8_t __BIGINT_COMPARE_MAGNITUDE__(const bigInt *__a__, const bigInt *__b__);
uint8_t __BIGINT_EQUAL__(const bigInt *__a__, const bigInt *__b__);
uint8_t __BIGINT_LESS__(const bigInt *__a__, const bigInt *__b__);
uint8_t __BIGINT_MORE__(const bigInt *__a__, const bigInt *__b__);
uint8_t __BIGINT_LESS_OR_EQUAL__(const bigInt *__a__, const bigInt *__b__);
uint8_t __BIGINT_MORE_OR_EQUAL__(const bigInt *__a__, const bigInt *__b__);

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
void __BIGINT_MAGNITUDED_ADD_UI64__(bigInt *__outputted_bigInteger, const bigInt *__x__, const uint64_t __I64VAL);
void __BIGINT_MAGNITUDED_SUB_UI64__(bigInt *__outputted_bigInteger, const bigInt *__x__, const uint64_t __I64VAL);
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

/* -------------------- SIGNED ARITHMETIC --------------------- */
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
/* ------------------- MUTATIVE ARITHMETIC -------------------- */
void __BIGINT_MUT_ADD_UI64__(bigInt *__x__, int64_t __I64VAL);
void __BIGINT_MUT_SUB_UI64__(bigInt *__x__, int64_t __I64VAL);
void __BIGINT_MUT_MUL_UI64__(bigInt *__x__, int64_t __I64VAL);
void __BIGINT_MUT_DIV_UI64__(bigInt *__x__, int64_t __I64VAL);
void __BIGINT_MUT_MOD_UI64__(bigInt *__x__, int64_t __I64VAL);
void __BIGINT_MUT_ADD__(bigInt *__x__, const bigInt *__y__);
void __BIGINT_MUT_SUB__(bigInt *__x__, const bigInt *__y__);
void __BIGINT_MUT_MUL__(bigInt *__x__, const bigInt *__y__);
void __BIGINT_MUT_DIV__(bigInt *__x__, const bigInt *__y__);
void __BIGINT_MUT_MOD__(bigInt *__x__, const bigInt *__y__);

/* ------------------ FUNCTIONAL ARITHMETIC ------------------- */
bigInt __BIGIN_INT_ADD_UI64__(const bigInt *__x__, int64_t __I64VAL);
bigInt __BIGIN_INT_SUB_UI64__(const bigInt *__x__, int64_t __I64VAL);
bigInt __BIGIN_INT_MUL_UI64__(const bigInt *__x__, int64_t __I64VAL);
bigInt __BIGIN_INT_DIV_UI64__(const bigInt *__x__, int64_t __I64VAL);
bigInt __BIGIN_INT_MOD_UI64__(const bigInt *__x__, int64_t __I64VAL);
bigInt __BIGINT_ADD__(const bigInt *__x__, const bigInt *__y__);
bigInt __BIGINT_SUB__(const bigInt *__x__, const bigInt *__y__);
bigInt __BIGINT_MUL__(const bigInt *__x__, const bigInt *__y__);
bigInt __BIGINT_DIV__(const bigInt *__x__, const bigInt *__y__);
bigInt __BIGINT_MOD__(const bigInt *__x__, const bigInt *__y__);

/* ------------------------- COPIES --------------------------- */
/* MUTATIVE API MODEL */
/* Used case example (Primitive Analogy) 
*   +) int a = 123456789;
*   +) int b = 5;
*   +) a = b;  ------> Mutates a into b's value -----> Mutation */
uint8_t __BIGINT_MUT_COPY__(const bigInt *source__, bigInt *copycat__); // Copies exactly (used limbs, capacity, value, ...)
uint8_t __BIGINT_MUT_COPY_OVER__(const bigInt *source__, bigInt *copycat__); // Overwrite the current available capacity, only copies the value if enough capacity
uint8_t __BIGINT_MUT_COPY_TRUNCOVER__(const bigInt *source__, bigInt *copycat__); // Overwrite the current available capacity, truncates if not enough capacity
/* FUNCTIONAL API MODEL */
/* Used case example (Primitive Analogy) 
*   +) int a = 123456789;
*   +) int c = a; ------> Declaring new variable with value a 
*                 ------> Return a value to be copied into  ------> Functional */
bigInt __BIGINT_COPY__(const bigInt *source__);
bigInt __BIGINT_COPY_OVER__(const bigInt *source__);
bigInt __BIGINT_COPY_TRUNCOVER__(const bigInt *source__);

#define bigInt_mut_copy             __BIGINT_MUT_COPY__
#define bigInt_mut_copy_over        __BIGINT_MUT_COPY_OVER__
#define bigInt_mut_copy_truncover   __BIGINT_MUT_COPY_TRUNCOVER__
#define bigInt_copy                 __BIGINT_COPY__
#define bigInt_copy_over            __BIGINT_COPY_OVER__
#define bigInt_copy_truncover       __BIGINT_COPY_TRUNCOVER__

/* -------------------- GENERAL UTILITIES --------------------- */
void __BIGINT_NORMALIZE__(bigInt *__bigInteger); // Normalize (no trailing 0s + guanratees 0 no -0)
void __BIGINT_ENSURE_CAPACITY__(bigInt *__bigInteger, size_t __MIN_CAP__);
void __BIGINT_RESET__(bigInt *__bigInteger);

#define bigInt_normalize        __BIGINT_NORMALIZE__
#define bigInt_ensureCapacity   __BIGINT_ENSURE_CAPACITY__
#define bigInt_reset            __BIGINT_RESET__

/* ------------------------ SPECIAL UTILITIES --------------------- */
/* o) Note: This category is reserved only for function used for:
*    +) Specific, one-time use, internal tasks
*    +) Other usages
*  o) It is also not defined/shortcutted
*  ----> +) This category of functions is not to be used on the surface-level interface
*           as it design DOES NOT follow the structure of the other functions
*        +) They're not aliased 
*
*/
void __BIGINT_SPECIAL_ADD_UI64__(bigInt *__x__, uint64_t __I64VAL);
void __BIGINT_SPECIAL_MUL_UI64__(bigInt *__x__, uint64_t __I64VAL);

#ifdef __cplusplus
}
#endif