#include "../header/num_theory.h"
#include "../../big_numbers/bigNums.h"

/* GCD - GREATEST COMMON DENOMINATOR */
void __BIGINT_STEIN_GCD__(bigInt *res, const bigInt *a, const bigInt *b) {}
void __BIGINT_LEHMER_GCD__(bigInt *res, const bigInt *a, const bigInt *b) {}

/* MODULAR ARITHMETIC */
void __BIGINT_CLASSICAL_REDUCT__(bigInt *res, const bigInt *a, const bigInt *b) {}
void __BIGINT_MONTGOMERY_REDUCT__(bigInt *res, const bigInt *a, const bigInt *b) {}

/* PRIMALITY TESTING */
void __BIGINT_MILLER_RABIN__(bigInt *res, const bigInt *a, const bigInt *b) {}
void __BIGINT_AKS__(bigInt *res, const bigInt *a, const bigInt *b) {}
