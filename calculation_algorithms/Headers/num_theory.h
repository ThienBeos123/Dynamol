#include "../../system/include.h"
#include "../../intrinsics/intrinsics.h"
#include "../../intrinsics/limb_math.h"
#include "../../internal_utils/util.h"
#include "../../big_numbers/bigNums.h"

/* --------------------------------------------------- BIGNUMS --------------------------------------------------- */
void __BIGINT_STEIN_GCD__(bigInt *res, const bigInt *a, const bigInt *b);
void __BIGINT_LEHMER_GCD__(bigInt *res, const bigInt *a, const bigInt *b);

void __BIGINT_CLASSICAL_REDUCT__(bigInt *res, const bigInt *a, const bigInt *b);
void __BIGINT_MONTGOMERY_REDUCT__(bigInt *res, const bigInt *a, const bigInt *b);

void __BIGINT_MILLER_RABIN__(bigInt *res, const bigInt *a, const bigInt *b);
void __BIGINT_AKS__(bigInt *res, const bigInt *a, const bigInt *b);


/* ------------------------------------------------- DYNAMICNUMS ------------------------------------------------- */




/* ------------------------------------------------- STATICNUMS -------------------------------------------------- */



