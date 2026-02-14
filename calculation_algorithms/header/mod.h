#ifndef DNML_MOD_H
#define DNML_MOD_H



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

void __BIGINT_BARETT__(const bigInt *a, const bigInt *b, bigInt *rem);
void __BIGINT_MONT_REDC__(const bigInt *a, const bigInt *b, bigInt *rem);


#ifdef __cplusplus
}
#endif


#endif