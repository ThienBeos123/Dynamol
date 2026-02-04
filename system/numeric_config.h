#ifndef bigint_config_h
#define bigint_config_h

#include <stdint.h>
#include <limits.h>
#define BIGINT_LIMBS_BITS 64
typedef uint64_t bigint_limb;
#define BITS_IN_UINT64_T ((sizeof(uint64_t)) * CHAR_BIT)


/* Multiplication */
#define BIGINT_SCHOOLBOOK           64
#define BIGINT_KARATSUBA            256
#define BIGINT_TOOM                 1024
#define BIGINT_SSA                  1024

/* Division + Euclidean Modulo */
#define BIGINT_KNUTH                64
#define BIGINT_BARETT               512
#define BIGINT_NEWTON               512

/* GCD - Greatest Common Divisor */
#define BIGINT_STEIN                512
#define BIGINT_LEHMER               2048
#define BIGINT_HALF_GCD             2048

/* Modular Inverse */
#define BIGINT_XEUCLID              256
#define BIGINT_BINARY_XGCD          2048
#define BIGINT_HALF_XGCD            2048

/* Primality Testing */
#define DETERMINISTIC_MR            64
#define MILLER_RABIN                1024
#define MR_BPSW                     4096
#define ECPP                        4096  

#define __bienable_mont__       1   
#define __bienable_newton__     1
#define __bienable_fft__        1
#define __bienable_ecpp__       1

#define __bicrypto_offset__     0

#endif