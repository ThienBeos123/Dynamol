#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <limits.h>
#include "bigInt.h"

typedef struct {
    /* The sign is stored inside the mantissa to 
        avoid conflict between bigInt's sign and bigFloat's sign */
    bigInt mantissa; int64_t exponent;
} bigFloat;

/* -------------------------------------    --   FLOAT TYPES    --   ---------------------------------------- */
/* ------------- CONSTRUCTORS & DESCTRUCTORS -------------- */
void __BIGFLOAT_EMPTY_INIT__(bigFloat *__bigFloat);
void __BIGfLOAT_FREE__(bigFloat *__bigFloat);
uint8_t __BIGFLOAT_NATIVE_INIT__(bigFloat *__bigFloat, uint64_t __primitive_val);

#ifdef __cplusplus
}
#endif