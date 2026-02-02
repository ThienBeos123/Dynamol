#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>

#include "../Headers/div.h"
#include "../../big_numbers/bigNums.h"
#include "../../internal_utils/util.h"

/* SIMPLE - INTERMEDIATE ALGORITHMS */
void __BIGINT_SCHOOLBOOK__(bigInt *res, const bigInt *a, const bigInt *b) {
    __BIGINT_INTERNAL_ENSCAP__(res, a->n + b->n);
    memset(res->limbs, 0, (a->n + b->n) * sizeof(uint64_t)); // Set every bytes to 0 in res, basically CALLOC() without the MALLOC()
    // Implementing schoolbook multiplication, treating each limb like a digit
    // -----> Inner loop access each limb of b and multiplying by 1 limb of a before going to the next a's limb
    for (size_t i = 0; i < a->n; ++i) {
        uint64_t carry = 0;
        for (size_t j = 0; j < b->n; ++j) {
            uint64_t low, high;
            __MUL_UI64__(a->limbs[i], b->limbs[j], &low, &high);
            // Stored and calculated kinda in a staircase pattern seen in the sums of Schoolbook
            uint64_t sum =      res->limbs[i + j] /* Potential data from last iteration */ 
                            +   low /* Lower half (64 bit) */ 
                            +   carry /* Carry from the last iteration */;

            carry =     high /* The remaining half (64 bit) */ 
                    +   (sum < low) /* Overflow from adding the lower half */ 
                    +   (sum < carry) /* Overflow from adding the last iteration's carry */;

            res->limbs[i + j] = sum; // Apply the new sum
        }
        res->limbs[i + b->n] += carry; // Add the remaining carry to the largest, 
                                       // most significant limb of the current sum
    }
    res->n = a->n + b->n;
}
void __BIGINT_KARATSUBA__(bigInt *res, const bigInt *a, const bigInt *b) {}
void __BIGINT_TOOM_3__(bigInt *res, const bigInt *a, const bigInt *b) {}
void __BIGINT_SSA__(bigInt *res, const bigInt *a, const bigInt *b) {}

void __BIGINT_MUL_DISPATCH__(bigInt *res, const bigInt *a, const bigInt *b) {

}