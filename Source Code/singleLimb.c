#include <stdio.h>
#include <inttypes.h>
#include <assert.h>
#include <limits.h>
#include "../Headers/singleLimb.h"

/* ADDITION + SUBTRACTION
*  +) Uses the principles of Elementary Addition and Subtraction (using Carries & Borrows)
*     to apply addition across multiple limbs in types of bigInt.c and bigFloat.c
*  ----> Treating each limb as if its a digit inside a number
*  ----> Example (idk if needed):                                   
*   1) Addition                                                     
*           ..                                                      
*            99                                                     
*       +    99                                                     
*          ------                                                   
*           198                                                     
*                                                                   
*       +) First pair: 9 + 9 = 18 --> overflowed by sum             
*                                 ---> carry                        
*      +) Second pair: (9 + 1) + 9                                  
*                    = 10 + 9 = 19 ---> Overflowed by sum & carry   
*                                  ---> Carry                       
*      +) Third pair: (0 + 1) + 0 = 1                               
*/
static inline uint64_t __ADD_UI64__(uint64_t a, uint64_t b, uint64_t *carry) {
    // Limb's sum
    uint64_t sum           = a + b;
    uint64_t sum_overflow  = (sum < a); /* When overflowed, unsigned ints do a modulo to reverse back
    // Limb's Carry                        -----> If overflow --> sum < a ---> return true (1)        */
    sum                      += *carry;
    uint64_t carry_overflow  = (sum < *carry); /* Same as above, if sum overflowed after adding the carry 
                                                  from the last limb, return true (1) */
    *carry = sum_overflow | carry_overflow; // If both or either overflowed ---> Carry to the next limb (carry 1)
    return sum;
}
static inline uint64_t __SUB_UI64__(uint64_t a, uint64_t b, uint64_t *borrow) {
    // Limb's difference
    uint64_t diff = a - b;
    uint64_t b1   = (a < b); /* When underflow, unsigned ints interpreted the bits as 
                                really big numbers due to the trailing 1s of the sign 
                                -----> If underflow --> a < b  ----> return true (1)  */
    diff -= *borrow;
    uint64_t b2   = (diff > a); /* Same as above, if difference overflowed after subtracting the carry
                                   from the last limb, return true (1) */
    *borrow = b1 | b2; // If both or either underflowed ---> Borrow to/from the next limb
    return diff;
}

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ */
/* MULTIPLICATION
*   +) I'm not really acknowledgable of other countries multiplication algorithm compared to Vietnam's
*      but I'll try my best to explain the multiplcation method I've implemented
*   +) Example:
*               23                                          a_high        a_low
*       x       64                                  x       b_high        b_low
*       ------------                    ------------------------------------------
*               12                                                 a_low * b_low        ----> first_mul ----> lower bits
*     +         80      --------->      +                 a_high * b_low            ---------> second_mul |_ Cross term ---> mid
*     +        180                      +        b_high * a_low           -------------------> third_mul  |
*     +       1200                      + b_high * a_high       ---------------------------> fourth_mul ---> higher bits
*       ------------
*             1472
*
*   +) Partial Product Form: - a = a_high * 2^32 + a_low * 2^0
*                            - b = b_high * 2^32 + b_low + 2^0
*   -----> a * b = (2^32 * a_high + a_low) * (2^32 * b_high + b_low)
*                = 2^64.(a_high * b_high) + --------------------------------> High
*                  2^32.(a_low * b_high + a_high * b_low) + ----------------> Mid
*                  2^0,(a_low * b_low) -------------------------------------> Low
*/
#if HAVE_UINT128
// Simpler multiplication using __uint128_t ----> System/Compiler Dependent - GNU/Clang/NVidia HPC/etc
static inline void __MUL_UI64__(uint64_t a, uint64_t b, uint64_t *low_res, uint64_t *high_res) {
    unsigned __int128 res = (unsigned __int128)a * b;
    *low_res = (uint64_t)res; *high_res = (uint64_t)(res >> 64);
}
#else
// More complex, portable multiplication ----> System/Compiler Independent
static inline void __MUL_UI64__(uint64_t a, uint64_t b, uint64_t *low_res, uint64_t *high_res) {
    // Seperate a and b into 2 different halves
    uint64_t a_low = a & 0xFFFFFFFF;        uint64_t b_low = b & 0xFFFFFFFF; // Extract the 32 lower bits
    uint64_t a_high = a >> 32;              uint64_t b_high = b >> 32; // Extract the 32 upper bits

    uint64_t first_mul = a_low * b_low;
    uint64_t second_mul = a_low * b_high;
    uint64_t third_mul = a_high * b_low;
    uint64_t fourth_mul = a_high * b_high;
    
    // Lower Half Calculation
    uint64_t mid = second_mul + third_mul;
    uint64_t mid_carry = (mid < second_mul); // Handles mid overflow (0 <= mid < 2^65)
    uint64_t mid_low = (mid & 0xFFFFFFFF) << 32; // Extract and Isolate the lower 32 bit of mid
    uint64_t res = first_mul + mid_low; // Return the lower 64 bits
    /* Lower half of a bit is attained by the formula:  Res % 2^64 = low
    *   +) (a x b % 2^64) = (first_mul + mid * 2^32 + fourth_mul * 2^64) % 2^64
    *                     = (first_mul + mid * 2^32) % 2^64 (1)
    * 
    *   +) mid                      = mid_low + mid_high * 2^32
    *      mid * 2^32               = mid_low * 2^32 + mid_high * 2^64
    *      (mid * 2^32) % 2^64      = (mid_low * 2^32 + mid_high * 2^64) % 2^64
    *      (mid * 2^32) % 2^64      = mid_low * 2^32                                = mid_low << 32 (2)
    * 
    * ----> (a x b % 2^64)      = first_mul + mid_low * 2^32
    * ----> (a x b) lower bits  = first_mul + mid_low * 2^32
    */

    // Upper Half Calculation
    uint64_t carry1 = (res < first_mul); // Check if adding mid_low to first_mul overflows to carry to the higher half
    uint64_t mid_high = mid >> 32; // The upper 32 bits of mid
    /* Higher half of a bit is attained by the formula:  floor(Res / 2^64) = high
    *   +) floor(a x b / 2^64) = floor((first_mul + mid * 2^32 + fourth_mul * 2^64) / 2^64)
    *                          = floor((first_mul / 2^64) + (mid * 2^-32) + fourth_mul)
    * 
    *   +) mid                  = mid_low + mid_high * 2^32
    *      mid * 2^-32          = (mid_low + mid_high * 2^32) / 2^32
    *      mid * 2^-32          = (mid_low / 2^32) + mid_high
    *      floor(mid * 2^-32)   = floor((mid_low / 2^32) + mid_high)
    *      floor(mid * 2^-32)   = floor({0 <= mid_low / 2^32 < 1} + mid_high)  (0 <= mid_low < 2^32)
    *      floor(mid * 2^-32)   = mid_high
    * 
    *   -----> floor(a x b / 2^64) = floor(first_mul / 2^64) + mid_high + fourth_mul
    *                              = mid_high + fourth_mul
    */

    *high_res = fourth_mul + carry1 + mid_high + (mid_carry << 32); // Takes the carry from lower half + the overflowed mid bit
    *low_res = res;
}
#endif

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ */
/* DIVISION & MODULO
*   +) This section is only include a small helper that is dedicated to calculating
*      the expression (a2 + a1) / b1 and (a2 + a1) % b1
*   +) A generalize helper that implements partial Knuth Algorithm D will not be implemented,
*      as it will be way too hard to emulate 128 bits integers, and simultaneously trying to
*      estimate the quotient on two seperate low and high bits.
*/
#if HAVE_UINT128
static inline void __DIV_HELPER_UI64__(uint64_t high, uint64_t low, uint64_t div, 
                                       uint64_t *qhat, uint64_t *rhat) {
    unsigned __int128 __INTERMEDIATE__ = ((unsigned __int128)high << BITS_IN_UINT64_T) | low;
    qhat = (uint64_t)(__INTERMEDIATE__ / div);
    rhat = (uint64_t)(__INTERMEDIATE__ % div);
}
#else
static inline void __DIV_HELPER_UI64__(uint64_t high, uint64_t low, uint64_t div, 
                                       uint64_t *qhat, uint64_t *rhat) {
    uint32_t a;
}
#endif

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ */
/* BITWISE OEPRATORS
*/

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ */
/* HELPING TASKS
*/
#if HAVE_BUILTIN_CLZ == 1
static inline uint8_t __COUNT_LZ__(uint64_t x) { 
    if (!x) return sizeof(uint64_t) * CHAR_BIT;
    return __builtin_clzll(x)
}
#elif HAVE_BUILTIN_CLZ == 2
#include <intrin.h>
static inline uint8_t __COUNT_LZ__(uint64_t x) { 
    if (!x) return sizeof(uint64_t) * CHAR_BIT;
    uint8_t index;
    _BitScanReverse(&index, x);
    return sizeof(uint64_t) - 1 - index;
}
#else
static inline uint8_t __COUNT_LZ__(uint64_t x) {
    if (!x) return sizeof(uint64_t) * CHAR_BIT;
    uint8_t __COUNT = 0;
    while (x) { x >>= 1; ++__COUNT; }
    return sizeof(uint64_t) * CHAR_BIT - __COUNT;
}
#endif

/* ----------------------------------------    --   TESTING LAB    --   ----------------------------------------- */
int main(int argc, char *argv[], char *envp[]) {

    return 0;
}