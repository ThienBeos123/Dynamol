#include "../system/include.h"
#include "../intrinsics/intrinsics.h"
#include "../intrinsics/limb_math.h"
#include "../internal_utils/util.h"
#include "../../big_numbers/bigNums.h"

/* --------------------------------------------------- BIGNUMS --------------------------------------------------- */
void __BIGINT_SCHOOLBOOK__(bigInt *res, const bigInt *a, const bigInt *b);
void __BIGINT_KARATSUBA__(bigInt *res, const bigInt *a, const bigInt *b);
void __BIGINT_TOOM_3__(bigInt *res, const bigInt *a, const bigInt *b);
void __BIGINT_SSA__(bigInt *res, const bigInt *a, const bigInt *b);

void __BIGINT_MUL_DISPATCH__(bigInt *res, const bigInt *a, const bigInt *b);


/* ------------------------------------------------- DYNAMICNUMS ------------------------------------------------- */




/* ------------------------------------------------- STATICNUMS -------------------------------------------------- */



