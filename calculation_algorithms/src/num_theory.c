#include "../header/num_theory.h"

/* GCD - GREATEST COMMON DIVISOR */
uint64_t __BIGINT_EUCLID__(uint64_t u, uint64_t v) {
    uint64_t remainder = (u < v) ? u : v;
    uint64_t dividend = (u >= v) ? u : v;
    uint64_t old_remainder;
    while (remainder) {
        old_remainder = remainder;
        remainder = dividend % remainder;
        dividend = old_remainder;
    }
    return dividend;
}
void __BIGINT_STEIN__(bigInt *res, const bigInt *u, const bigInt *v) {
    
}
void __BIGINT_LEHMER__(bigInt *res, const bigInt *u, const bigInt *v) {}
void __BIGINT_HALF__(bigInt *res, const bigInt *u, const bigInt *v) {}


/* Primality Testing */
bool __BIGINT_TRIAL_DIV__(const bigInt *x) {}
bool __BIGINT_MILLER_RABIN__(const bigInt *x) {}
bool __BIGINT_BPSW_MIX__(const bigInt *x) {}
bool __BIGINT_ECPP__(const bigInt *x) {}
