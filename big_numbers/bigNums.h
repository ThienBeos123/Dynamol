#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <limits.h>
#include <stdlib.h>

/* Essential Details */
#define LIMB_BITS uint64_t
#define LIMB_MASK UINT64_MAX

#define BITS_IN_UINT64_T ((sizeof(uint64_t)) * CHAR_BIT)

/* Compiler/System specific implementations */
#ifdef __SIZEOF_INT128__
#define HAVE_UINT128 1
#else
#define HAVE_UINT128 0
#endif

/* Multiplication Threshold */
#define SCHOOLBOOK  32      // n <  32
#define KARATSUBA   256     // n <  256
#define TOOM        1024    // n <  1024
#define FFT         1024    // n >= 1024 (Threshold only needed in special conditionals)

/* Division Threshold */
#define KNUTH_D     64      // n <  64
#define NEWTON      1024    // n < 1024 (Threshold only needed in special conditionals)

typedef struct __424947696E7465676572__ {
    int8_t sign;                    LIMB_BITS *limbs; 
    size_t n; /* Used limbs */      size_t cap; /* Maximum limbs */
} bigInt;

typedef struct __424947666C6F6174__ {
    bigInt mantissa; uint32_t exponent; 
    // RAM Space needed to calculate the maxmimum exponent value as a power of 2 (2^(2^32 - 1))
    // ------> Approximately 550MB
    // ------> Doable, but extremely heavy for computers
} bigFloat;

typedef struct {
    
} bigRational;


#ifdef __cplusplus
}
#endif