#ifdef __cplusplus
extern "C" {
#endif

#include "../system/include.h"
#include "../system/numeric_config.h"
#include "../system/compiler.h"


//todo ========================================= ALGORITHMS THRESHOLD ==========================================
/* Multiplication */
#define SCHOOLBOOK          64
#define KARATSUBA           256
#define TOOM                1024
#define SSA                 1024

/* Division + Euclidean Modulo */
#define KNUTH               64
#define BARETT              512
#define NEWTON              512

/* GCD - Greatest Common Divisor */
#define STEIN               512
#define LEHMER              2048
#define HALF                2048

/* Modular Inverse */
#define EXTENDED_EUCLID     256
#define BINARY_XGCD         2048
#define HALF-BINARY_XGCD    2048

/* Primality Testing */
#define DETERMINISTIC_MR    64
#define MILLER-RABIN        1024
#define MR_BPSW             4096
#define ECPP                4096    



//todo =========================================== TYPE DEFINITION =============================================
typedef struct __424947696E7465676572__ {
    int8_t sign;                    bigint_limb *limbs; 
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