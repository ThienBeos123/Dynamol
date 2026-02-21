#ifndef __DNML_BIGNUMS_H
#define __DNML_BIGNUMS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../system/include.h"
#include "../system/compiler.h"
#include "../sconfigs/settings.h"
#include "../sconfigs/numeric_config.h"
#include "../sconfigs/dnml_status.h"
#include "../sconfigs/arena.h"

//todo =========================================== TYPE DEFINITION =============================================
#define BIGINT_LIMBS_BITS 64
#define DASI_OVERFLOW_THRES10 1844677407370955161 // 1,844,674,407,370,955,161 (UINT64_MAX / 10)
typedef uint64_t limb_t;

typedef struct __424947696E7465676572__ {
    size_t n; /* Used limbs */      size_t cap; /* Maximum limbs */
    limb_t *limbs;                  int8_t sign;
} bigInt;

typedef struct __424947666C6F6174__ {
    bigInt mantissa; uint32_t exponent; 
    // RAM Space needed to calculate the maxmimum exponent value as a power of 2 (2^(2^32 - 1))
    // ------> Approximately 550MB
    // ------> Doable, but extremely heavy for computers
} bigFloat;


#ifdef __cplusplus
}
#endif

#endif
