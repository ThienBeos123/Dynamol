#ifndef DNML_DIV_H
#define DNML_DIV_H



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

/* --------------------------------------------------- BIGNUMS --------------------------------------------------- */
void __BIGINT_SHORT_DIVISION__(const bigInt *a, const bigInt *b, bigInt *quot, bigInt *rem);
void __BIGINT_KNUTH_D__(const bigInt *a, const bigInt *b, bigInt *quot, bigInt *rem);
void __BIGINT_NEWTON_RECIPROCAL__(const bigInt *a, const bigInt *b, bigInt *quot, bigInt *rem);
void __BIGINT_DIVMOD_DISPATCH__(const bigInt *a, const bigInt *b, bigInt *quot, bigInt *rem);



/* ------------------------------------------------- DYNAMICNUMS ------------------------------------------------- */




/* -------------------------------------------------- STATICNUMS ------------------------------------------------- */


#ifdef __cplusplus
}
#endif


#endif
