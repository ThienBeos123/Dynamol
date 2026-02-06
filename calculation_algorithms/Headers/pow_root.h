#include "../../system/include.h"
#include "../../intrinsics/intrinsics.h"
#include "../../intrinsics/limb_math.h"
#include "../../internal_utils/util.h"
#include "../../big_numbers/bigNums.h"

/* --------------------------------------------------- BIGNUMS --------------------------------------------------- */
void __BIGINT_BINARY_EXP__(bigInt *res, const bigInt *a, const bigInt *b);
void __BIGINT_SLIDING_WINDOW_EXP__(bigInt *res, const bigInt *a, const bigInt *b);
void __BIGINT_NEWTON_RAPHSON_SQRT__(bigInt *res, const bigInt *a, const bigInt *b);
void __BIGINT_NEWTON_NROOT__(bigInt *res, const bigInt *a, const bigInt *b);

void __BIGINT_FFT_EXP__(bigInt *res, const bigInt *a, const bigInt *b);
void __BIGINT_FFT_NEWTON__(bigInt *res, const bigInt *a, const bigInt *b);


/* ------------------------------------------------- DYNAMICNUMS ------------------------------------------------- */



/* ------------------------------------------------- STATICNUMS -------------------------------------------------- */



