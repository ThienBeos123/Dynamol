#ifndef DNML_POW_ROOT_H
#define DNML_POW_ROOT_H


#ifdef __cplusplus
extern "C" {
#endif



#include "../../system/include.h"
#include "../../system/compiler.h"
#include "../../sconfigs/arena.h"
#include "../../sconfigs/numeric_config.h"

#include "../../intrinsics/intrinsics.h"
#include "../../intrinsics/limb_math.h"
#include "../../internal_utils/util.h"
#include "../../big_numbers/bigNums.h"

#include "div.h"
#include "mul.h"

/* --------------------------------------------------- BIGNUMS --------------------------------------------------- */
void __BIGINT_BINARY_EXP__(bigInt *res, const bigInt *a, const bigInt *b);
void __BIGINT_SLIDING_WINDOW_EXP__(bigInt *res, const bigInt *a, const bigInt *b);
void __BIGINT_NEWTON_RAPHSON_SQRT__(bigInt *res, const bigInt *a, const bigInt *b);
void __BIGINT_NEWTON_NROOT__(bigInt *res, const bigInt *a, const bigInt *b);


/* ------------------------------------------------- DYNAMICNUMS ------------------------------------------------- */



/* ------------------------------------------------- STATICNUMS -------------------------------------------------- */


#ifdef __cplusplus
}
#endif


#endif
