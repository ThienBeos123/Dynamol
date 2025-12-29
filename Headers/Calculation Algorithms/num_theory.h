#include "../Type Headers/bigNums.h"
#include "../Type Headers/dynamicNums.h"
#include "../Type Headers/staticNums.h"

/* --------------------------------------------------- BIGNUMS --------------------------------------------------- */
void __BIGINT_EUCLIDEAN_GCD__(bigInt *res, const bigInt *a, const bigInt *b);
void __BIGINT_STEIN_GCD__(bigInt *res, const bigInt *a, const bigInt *b);
void __BIGINT_LEHMER_GCD__(bigInt *res, const bigInt *a, const bigInt *b);

void __BIGINT_CLASSICAL_REDUCT__(bigInt *res, const bigInt *a, const bigInt *b);
void __BIGINT_MONTGOMERY_REDUCT__(bigInt *res, const bigInt *a, const bigInt *b);

void __BIGINT_MILLER_RABIN__(bigInt *res, const bigInt *a, const bigInt *b);
void __BIGINT_AKS__(bigInt *res, const bigInt *a, const bigInt *b);


/* ------------------------------------------------- DYNAMICNUMS ------------------------------------------------- */




/* ------------------------------------------------- STATICNUMS -------------------------------------------------- */



