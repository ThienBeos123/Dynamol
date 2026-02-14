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
#include "mod.h"

//todo ============================== GREATEST COMMON DIVISORS ============================== todo//
void __BIGINT_EUCLID__(bigInt *res, const bigInt *u, const bigInt *v);
void __BIGINT_STEIN__(bigInt *res, const bigInt *u, const bigInt *v);
void __BIGINT_LEHMER__(bigInt *res, const bigInt *u, const bigInt *v);




//todo ================================== PRIMALITY TESTING ================================== todo//
bool __BIGINT_TRIAL_DIV__(const bigInt *x);
bool __BIGINT_MILLER_RABIN__(const bigInt *x);
bool __BIGINT_BPSW_MIX__(const bigInt *x);
bool __BIGINT_ECPP__(const bigInt *x);


#ifdef __cplusplus
}
#endif




#endif