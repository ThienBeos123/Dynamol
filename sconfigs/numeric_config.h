#ifndef ___DNML_NUM_CONFIG_H___
#define ___DNML_NUM_CONFIG_H___

#include <stdint.h>
#include <limits.h>
#define BYTES_IN_UINT64_T sizeof(uint64_t)
#define BITS_IN_UINT64_T ((sizeof(uint64_t)) * CHAR_BIT)
#define I64_MAX_BIT_MASK 0x7FFFFFFFFFFFFFFFULL
#define I64_MIN_BIT_MASK 0x8000000000000000ULL

/* Precomputation */
#define log2_10 3.322
#define log2_16 4
#define log2_2  1
#define log2_8  3

//*========================================== Thresholds //*========================================== *//
/* String Parsing */
#define DASI_NAIVE_PARSE            750 // digits <= 750
#define DASI_DandC_PARSE            750 // digits > 750

/* Multiplication */
#define BIGINT_SCHOOLBOOK           64
#define BIGINT_KARATSUBA            256
#define BIGINT_TOOM                 1024
#define BIGINT_SSA                  1024

/* Division + Euclidean Modulo */
#define BIGINT_SHORT                1   // n < 1        DIV + MOD
#define BIGINT_KNUTH                64  // n < 64       DIV + MOD
#define BIGINT_BURNIKEL             512 // n < 512      DIV
#define BIGINT_BARETT               512 // n < 512            MOD
#define BIGINT_NEWTON               512 // n >= 512     DIV + MOD

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

