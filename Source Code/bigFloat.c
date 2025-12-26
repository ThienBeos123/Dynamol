#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <string.h>

#include "../Headers/singleLimb.h"
#include "../Headers/Big Data Types/bigFloat.h"
#include "../Headers/Big Data Types/bigFloat.hpp"

/* ----------------------------------------    --   FLOAT TYPES    --   ------------------------------------------ */
/* ------------ CONSTRUCTORS & DESCTRUCTORS ------------- */
void __BIGFLOAT_EMPTY_INIT__(bigFloat *x) { 
    __BIGINT_EMPTY_INIT__(&x->mantissa); 
    x->exponent = 0;
}
void __BIGfLOAT_FREE__(bigFloat *x) {
    __BIGINT_FREE__(&x->mantissa);
    x->exponent = 0;
}
uint8_t __BIGFLOAT_NATIVE_INIT__(bigFloat *x, uint64_t in) {
    if (!__BIGINT_NATIVE_INIT__(&x->mantissa, in)) return 0;
    x->exponent = 0; return 1;
}



/* ----------------------------------------    --   TESTING LAB    --   ----------------------------------------- */
int main(int argc, char *argv[], char *envp[])
{
    return 0;
}