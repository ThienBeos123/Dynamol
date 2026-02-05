#ifndef ___DNML_ARCH_H___
#define ___DNML_ARCH_H___

/* Detecting x86/x64 Architecture */
#if defined(__x86_64__) || defined(_M_X64)
    #define __ARCH_X86_64__ 1
#else
    #define __ARCH_X86_64__ 0
#endif

/* Detecting ARM Architecture */
#if defined(__aarch64_) || defined(_M_ARM64)
    #define __ARCH_ARM64__ 1
#else
    #define __ARCH_ARM64__ 0
#endif

/* Endian Detection */
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    #define __LITTLE_ENDIAN__ 1
#else
    #define __LITTLE_ENDIAN__ 0
#endif

/* Alighment Adjustment */
#if __ARCH_X86_64__ || __ARCH_ARM64__
    #define __BIGINT_DEFAULT_ALIGNMENT__ 32
#else
    #define __BIGINT_DEFAULT_ALIGNMENT__ 8
#endif

#endif