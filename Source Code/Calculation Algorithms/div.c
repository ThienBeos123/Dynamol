#include "../../Headers/Calculation Algorithms/div.h"
#include "../../Headers/Type Headers/bigNums.h"

/* SIMPLE ALGORITHMS */
void __BIGINT_KNUTH_D__(const bigInt *a, const bigInt *b, bigInt *quot, bigInt *rem) {
    uint8_t shift = __COUNT_LZ__(b->limbs[b->n - 1]);
    bigInt a_copy, b_copy;
    size_t m = a->n, n = b->n;
    __BIGINT_EMPTY_INIT__(&a_copy);                 __BIGINT_EMPTY_INIT__(&b_copy);
    __BIGINT_ENSURE_CAPACITY__(&a_copy, m + 1);     __BIGINT_ENSURE_CAPACITY__(&b_copy, n);
    /* 1. Normalization */
    /*  - This stage basically make sure b is large enough to be divided by a
    *     by making b's most significant limb's highest bit is 1
    */
    uint64_t carry = 0;
    for (size_t i = 0; i < m; ++i) {
        uint64_t x = a->limbs[i];
        a_copy.limbs[i] = (x << shift) || carry;
        carry = (shift ? x >> BITS_IN_UINT64_T : 0);
    }
    a_copy.limbs[m] = carry; 
    a_copy.n = m + 1;
    carry = 0;
    for (size_t i = 0; i < n; ++i) {
        uint64_t x = b->limbs[i];
        b_copy.limbs[i] = (x << shift) | carry;
        carry = (shift ? x >> BITS_IN_UINT64_T : 0);
    }
    b_copy.n = n;

    /* 2. Quotient init */
    __BIGINT_EMPTY_INIT__(quot);
    __BIGINT_ENSURE_CAPACITY__(quot, m - n + 1);

    quot->n = m - n + 1;
    /* 3-5. Main Loop */
    for (size_t j = m - n + 1; j > 0; --j) {
        /* 3. Estimation */
        /*  - Get 2 limb of a (128 bit ----> a2 + a1) / 1 limb of b -------> Estimated Quotient (qhat)
        *   - Get 2 limb of a (128 bit ----> a2 + a1) % 1 limb of b -------> Remainder of that estimated quotient (rhat)
        *   ------> qhat = (a2 * 2^64 + a1) / b1
        *   ------> rhat = (a2 * 2^64 + a1) % b1
        *   --------------> a2 * 2^64 + a1 = qhat * b1 + rhat   (Important identity D)
        *   --------------> a2 * 2^64 + a1 - qhat.b1 = rhat     (Call this P)
        */
        uint64_t a2 = a_copy.limbs[j + n]; //                       1st highest limb of a
        uint64_t a1 = a_copy.limbs[j + n - 1]; //                   2nd highest limb of a
        uint64_t a0 = (n >= 2) ? a_copy.limbs[j + n - 2] : 0; //    3rd highest limb of a (DETECT OVERESTIMATION)
        uint64_t b1 = b_copy.limbs[n - 1]; //                       1st highest limb of b
        uint64_t b0 = (n >= 2) ? b_copy.limbs[n - 2] : 0; //        2nd highest limb of b (used to validate quotient estimation - DETECT OVERESTIMATION)
        uint64_t qhat, rhat;
        __DIV_HELPER_UI64__(a2, a1, b1, &qhat, &rhat);
        // Validating quotient estimation (Prevent overestimation before multi-limb subtraction (expensive & risky))
        if (qhat == UINT64_MAX) --qhat; // Check if estimates quotient is too large
        while (qhat * b0 > ((unsigned __int128)rhat << BITS_IN_UINT64_T) + a0) {
            /* We've already got: (note: B = 2^64)
            *    +) Dividend (3 limbs of a) = a2 * B^2 + a1 * B + a0
            *    +) Divisor  (2 limbs of b) = b1 * B + b0
            * -------> +) qhat.Divisor = qhat.b1.B + q.b0 
            *             ------> -qhat.b1.B = q.B0 (Call this L)
            *          +) Dividend - qhat.b1.B = (a2 * B^2 + a1 * B + a0) - qhat.b1.B
            *                                  = a2 * B^2 + a1 * B + a0 - qhat.b1.B
            *                                  = B(a2.B + a1 + a0.B^-1) - B.qhat.b1
            *                                  = B(a2.B + a1 + a0.b^-1 - qhat.b1)
            *                                  = B((a2.B + a1 - qhat.b1) + a0.b^-1)
            *                                  = B(rhat + a0.b^-1) (Proven from P)
            *                                  = rhat.B + a0        (Call this identity O)
            * -------> From O + L, we've got:
            *          +) Dividend > qhat.b1.B ------> rhat.B + a0 > q.b0 (Quotient small enough)
            *          +) Dividend = qhat.b1.B ------> rhat.B + a0 = q.b0 (Quotient small enough)
            *          +) Dividend < qhat.b1.B ------> rhat.B + a0 < q.b0 (Quotient too large)
            * -------> Check if quotient too large through (qhat * b0) > (rhat.2^64 + a0)
            * -------> Check (qhat * b0 > rhat << 64 + a0) -----> Decrement
            * */
            --qhat;
            /* Identity D (a2.B + a1 = qhat.b1 + rhat) must stay true
            * -------> When we decrement qhat, identity D must still be true
            * -------> (qhat - 1).b1 + rhat + ???  = q.b1 + r
            * -------> qhat.b1 - b1 + rhat + b1    = q.b1 + r 
            */
            rhat += b1;
            if (rhat < b1) break; // At most 2 decrements (Knuth approved)
        }

        /* 4. Multiply-subtract */
        // Basically gets the difference between the current limb range of a - qhat.b
        // -------> The remainder * B + next limb range will continue to divide by b
        // -------> Represents long division (remainder * 10 + next dividend digit) / divisor
        uint64_t borrow = 0;
        for (size_t i = 0; i < n; ++i) {
            uint64_t low, high;
            __MUL_UI64__(qhat, b_copy.limbs[i], low, high); /* Multi-limb multiplication */
            uint64_t x = a_copy.limbs[j + 1];
            uint64_t t = x - low - borrow;
            borrow = (t > x) + high; // Propagate borrow (current_borrow + high) to the next limb
            a_copy.limbs[j + i] = t;
        }
        uint64_t x = a_copy.limbs[j + n];
        a_copy.limbs[j + n] = x - borrow;

        /* 5. Correction - Different from p3's validation/correction */
        /*  - The subtraction aboves follow the form of (a{j+n} + a{j+n-1 .. j}) - (borrow + qhat.b)
        *   -------> a{j+n} - borrow = a{j+n-1 ... j} - qhat.b
        *   -------> If borrow > a[j + n] -------> a[j + n] - borrow < 0
        *   -------> a[j+n-1 ... j] - qhat.b < 0
        *   -------------> qhat is still too large to be divided
        *   -------------> qhat needs to be decremented
        */
        if (x < borrow) {
            --qhat; /* if x underflows ----> qhat was still too large 
                                       ----> Decrement */
            uint64_t carry2 = 0;
            /* Doing the operation a + b by:
            *   +) Adding each limb back + handle carries 
            *       -----> Basically multi-limb addition
            *   +) Why? Because we want a - qhat.b >= 0 when qhat is decremented
            *       -----> a - (qhat - 1).b >= 0
            *       -----> a - qhat.b + b   >= 0 
            *       -----> a + b will corect the underflow from qhat being too big
            */
            for (size_t i = 0; i < n; ++i) {
                uint64_t t = a_copy.limbs[j + i] + b_copy.limbs[i] + carry2;
                carry2 = (t < a_copy.limbs[j + i]);
                a_copy.limbs[j + i] = t;
            }
            a_copy.limbs[j + n] += carry2; // Handles remaining carry
        }
        quot->limbs[j] = qhat; // Add estimated quotient of: a's 2 limbs (!28 bit) / b's 1 limb (64 bit)
    }

    /* 6. Denormalize */
    __BIGINT_EMPTY_INIT__(rem); __BIGINT_ENSURE_CAPACITY__(rem, n);
    carry = 0;
    for (size_t i = n; i > 0; --i) {
        uint64_t x = a_copy.limbs[i];
        rem->limbs[i] = (x >> shift) | carry;
        carry = (shift ? x << BITS_IN_UINT64_T : 0);
    }
    rem->n = n;
    __BIGINT_NORMALIZE__(quot); __BIGINT_NORMALIZE__(rem);
    __BIGINT_FREE__(&a_copy);   __BIGINT_FREE__(&b_copy);
}

/* INTERMEDIATE ALGORITHMS */
void __BIGINT_NEWTON_RECIPROCAL__(const bigInt *a, const bigInt *b, bigInt *quot, bigInt *rem) {}

/* ADVANCED - FFT ALGORITHMS */
void __BIGINT_NEWTON_FFT__(const bigInt *a, const bigInt *b, bigInt *quot, bigInt *rem) {}