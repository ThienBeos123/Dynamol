#ifndef bigint_config_h
#define bigint_config_h

#include <stdint.h>
#include <limits.h>
#define BIGINT_LIMBS_BITS 64
typedef uint64_t bigint_limb;
#define BITS_IN_UINT64_T ((sizeof(uint64_t)) * CHAR_BIT)


/* Multiplication Threshold */
#define BIGINT_SCHOOLBOOK   32
#define BIGINT_KARATSUBA    128
#define BIGINT_TOOM         1024
#define BIGINT_FFT          1024

/* Division & Modulo Threshold */
#define BIGINT_KNUTH        64
#define BIGINT_BARETT       1024
#define BIGINT_NEWTON       1024

/* Primality Testing Threshold */
#define BIGINT_SMALL_MR         64
#define BIGINT_MILLER_RABIN     1024
#define BIGINT_BPSW             8192
#define BIGINT_ECPP             8192

/* Greatest Common Divisor - GCD Threshold */
#define BIGINT_STEIN            128
#define BIGINT_LEHMER           1024
#define BIGINT_HALF_GCD         1024

#define __bienable_mont__       1   
#define __bienable_newton__     1
#define __bienable_fft__        1
#define __bienable_ecpp__       1

#define __bicrypto_offset__     0

#endif