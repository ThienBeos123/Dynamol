#ifdef __cplusplus
extern "C" {
#endif

#include "../system/include.h"
#include "../system/numeric_config.h"
#include "../system/compiler.h"

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