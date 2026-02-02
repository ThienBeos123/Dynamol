#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <limits.h>

#include "../../big_numbers/bigNums.h"

/* --------------------------------------------------- BIGNUMS --------------------------------------------------- */
void __BIGINT_KNUTH_D__(const bigInt *a, const bigInt *b, bigInt *quot, bigInt *rem);
void __BIGINT_NEWTON_RECIPROCAL__(const bigInt *a, const bigInt *b, bigInt *quot, bigInt *rem);
void __BIGINT_DIVMOD_DISPATCH__(const bigInt *a, const bigInt *b, bigInt *quot, bigInt *rem);



/* ------------------------------------------------- DYNAMICNUMS ------------------------------------------------- */




/* -------------------------------------------------- STATICNUMS ------------------------------------------------- */


#ifdef __cplusplus
}
#endif
