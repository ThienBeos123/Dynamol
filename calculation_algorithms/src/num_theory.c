#include "../header/num_theory.h"

/* GCD - GREATEST COMMON DENOMINATOR */
void __BIGINT_EUCLID__(bigInt *res, const bigInt *u, const bigInt *v) {}
void __BIGINT_STEIN__(bigInt *res, const bigInt *u, const bigInt *v) {}
void __BIGINT_LEHMER__(bigInt *res, const bigInt *u, const bigInt *v) {}


/* Primality Testing */
bool __BIGINT_TRIAL_DIV__(const bigInt *x) {}
bool __BIGINT_MILLER_RABIN__(const bigInt *x) {}
bool __BIGINT_BPSW_MIX__(const bigInt *x) {}
bool __BIGINT_ECPP__(const bigInt *x) {}
