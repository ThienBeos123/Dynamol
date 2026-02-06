#include "limb_math.h"

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ */
/* DIVISION & MODULO
*   +) This section is only include a small helper that is dedicated to calculating
*      the expression (a2 + a1) / b1 and (a2 + a1) % b1
*   +) A generalize helper that implements partial Knuth Algorithm D will not be implemented,
*      as it will be way too hard to emulate 128 bits integers, and simultaneously trying to
*      estimate the quotient on two seperate low and high bits.
*/
#if HAVE_UINT128
void __DIV_HELPER_UI64__(uint64_t high, uint64_t low, uint64_t div, 
                                       uint64_t *qhat, uint64_t *rhat) {
    unsigned __int128 __INTERMEDIATE__ = ((unsigned __int128)high << BITS_IN_UINT64_T) | low;
    qhat = (uint64_t)(__INTERMEDIATE__ / div);
    rhat = (uint64_t)(__INTERMEDIATE__ % div);
}
#else
void __DIV_HELPER_UI64__(uint64_t high, uint64_t low, uint64_t div, 
                                       uint64_t *qhat, uint64_t *rhat) {
    uint32_t a;
}
#endif

/* ----------------------------------------    --   TESTING LAB    --   ----------------------------------------- */
/* Commented out to avoid duplicate main when linking with bigInt.c
int main(int argc, char *argv[], char *envp[]) {

    return 0;
}
*/