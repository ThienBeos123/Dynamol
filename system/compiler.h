#ifndef ___DNML_COMPILER_H___
#define ___DNML_COMPILER_H___

#include "arch.h"

/* Compiler Detection */
#if defined(__clang__) /* Clang Compiler */
    #define __compiler_clang 1
#elif defined(__GNUC__) /* GNU GCC Compiler */
    #define __compiler_gcc  1
#elif defined(_MSC_VER) /* MSVC Compiler */
    #define __compiler_msvc 1
    #include <intrin.h>
#else /* Unsupported, Niche Compilers */
    #define __compiler_undefined 1
#endif


/* Architecture-specific Intrinsics */
#if __ARCH_X86_64__
    #include <immintrin.h>
#elif __ARCH_ARM64__
    #include <arm_neon.h>
#endif


/* Inline and Restrict */
#if __compiler_msvc
    #define inline __forceinline
    #define restrict __restrict
#else
    #define inline __attribute__((always_inline))
    #define restrict __restrict__
#endif

/* Count Leading Zeros - CLZ */
#if __compiler_msvc
    #define HAVE_CLZ 1
    #define clz_intrinsics _lzcnt_u64
#elif (__compiler_clang || __compiler_gcc)
    #define HAVE_CLZ 1
    #define clz_intrinsics __builtin_clzll
#else
    #define HAVE_CLZ 0
    #define clz_intrinsics
#endif


/* Count Trailing Zeros - CTZ */
#if __compiler_msvc
    #define HAVE_CTZ 1
    #define ctz_intrinsics _tzcnt_u64
#elif (__compiler_clang || __compiler_gcc)
    #define HAVE_CTZ 1
    #define ctz_intrinsics __builtin_ctzll
#else
    #define HAVE_CTZ 0
    #define ctz_intrinsics
#endif


/* Branch Prediction */
#if (__compiler_clang || __compiler_gcc)
    #define likely(x)       __builtin_expect(!!(x), 1)
    #define unlikely(x)     __builtin_expect(!!(x), 0)
#else
    #define likely(x)       (x)
    #define unlikely(x)     (x)
#endif


/* Static Assertions */
#if __STDC_VERSION__ >= 201112L
    #define static_assert _Static_assert
#else
    #define static_assert(cond, msg) typedef char static_assert_##msg[(cond) ? 1 : -1]
#endif



/* Assume / Unreachable */
#if (__compiler_clang || __compiler_gcc)
    #define assume(x)           if(!(x)) __builtin_unreachable()
    #define unreachable()                __builtin_unreachable()
#elif __compiler_msbvc
    #define assume(x) __assume(x)
    #define unreachable() __assume(0)
#else
    #define assume(x)
    #define unreachable()
#endif

#endif

