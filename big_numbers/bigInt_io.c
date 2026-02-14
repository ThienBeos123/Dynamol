#include "../system/include.h"
#include "../sconfigs/settings.h"
#include "../sconfigs/arena.h"
#include "../sconfigs/dnml_status.h"

#include "../intrinsics/intrinsics.h"
#include "../intrinsics/limb_math.h"
#include "../calculation_algorithms/calculation.h"
#include "../internal_utils/util.h"
#include "bigNums.h"

// Providing for
#include "_bigint.h"


static const uint8_t _VALUE_LOOKUP_[256] = {
    /* 0-47: Non-digit characters */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

    /* 48-57: '0'-'9' */
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9,

    /* 58-64: Characters between '9' and 'A' */
    -1, -1, -1, -1, -1, -1, -1,

    /* 65-70: Uppercase 'A'-'F' */
    10, 11, 12, 13, 14, 15,

    /* 71-96: Characters between 'F' and 'a' */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

    /* 97-102: Lowercase 'a'-'f' */
    10, 11, 12, 13, 14, 15,

    /* 103-255: Remaining ASCII range */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1
};



//todo ======================================= 1. CONSTRUCTIONS ======================================= *//
str_status __BIGINT_STRING_INIT__(bigInt *x, const char* str) {
    if (x->limbs) return; // Already initialized
    if (*str == '\0') return STR_EMPTY;
    static local_thread dnml_arena _STR_INIT_ARENA;
    static local_thread bool _STR_INIT_ARENA_INITIALIZED = false;

    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    if (str[curr_pos] == '-') { sign = -1; ++curr_pos; }
    else if (str[curr_pos] == '+') ++curr_pos;
    else if (str[curr_pos] && str[curr_pos] != '0') return STR_INVALID_SIGN;
    if (str[curr_pos] == '\0') return STR_INCOMPLETE;

    //* ====== 2. Prefix Handling ====== *//
    uint8_t base = 10;
    if (str[curr_pos] == '0') {
        if (str[curr_pos + 1] == '\0') { // Indicates its a singular 0 ('0')
            if (sign = -1) return STR_INVALID_SIGN; // -0 is INVALID
            // Empty initialization
            x->limbs = malloc(sizeof(uint64_t));
            x->cap   = 1;
            x->n     = 0;
            x->sign  = 1;
            return STR_SUCCESS;
        } else if (str[curr_pos + 1] - '0' <= '9' && str[curr_pos + 1] >= '0'){
            // This case handles if str[curr_pos] == '0' is a trailing zero
            // for a numeric string of base 10
            ++curr_pos;
        } else {
            switch (str[curr_pos + 1]) {
                // Hexadecimal
                case 'x':   base = 16; curr_pos += 2; break;
                case 'X':   base = 16; curr_pos += 2; break;
                // Binary
                case 'b':   base = 2; curr_pos += 2; break;
                case 'B':   base = 2; curr_pos += 2; break;
                // Octal
                case 'o':   base = 8; curr_pos += 2; break;
                case 'O':   base = 8; curr_pos += 2; break;
                //! INVALID BASE PREFIX
                default:    return STR_INVALID_BASE_PREFIX; break;
            }
        }
    }

    //* ====== 3. Leading-Zeros Handling ====== *//
    while (str[curr_pos] == '0' && str[curr_pos] != '\0') ++curr_pos; // Skipping all leading zeros
    // String full of zeros
    if (str[curr_pos] == '\0') {
        if (sign = -1) return STR_INVALID_SIGN; // -0 is INVALID// Empty initialization
        x->limbs = malloc(sizeof(uint64_t));
        x->cap   = 1;
        x->n     = 0;
        x->sign  = 1;
        return STR_SUCCESS;
    }

    //* ======= 4. Instatiating Temporary BigInt ======= *//
    size_t d = strlen(&str[curr_pos]);
    size_t cap = ((size_t)(__BITCOUNT__(d, base) / BIGINT_LIMBS_BITS)) + 1;
    if (!_STR_INIT_ARENA_INITIALIZED) {
        arena_init(&_STR_INIT_ARENA, cap * BYTES_IN_UINT64_T);
        _STR_INIT_ARENA_INITIALIZED = true;
    }
    size_t tmp_mark = arena_mark(&_STR_INIT_ARENA);
    limb_t *tmp_limbs = arena_alloc(&_STR_INIT_ARENA, cap * BYTES_IN_UINT64_T);
    bigInt __TEMPHOLDER__ = {
        .limbs = tmp_limbs,
        .cap   = cap,
        .n     = 0,
        .sign  = sign
    };

    //* ============= 5. Parsing and Initiating Value ================ *//
    for (; curr_pos < d; ++curr_pos) {
        uint8_t lookup_index = (uint8_t)(str[curr_pos] - '\0');
        if (_VALUE_LOOKUP_[lookup_index] > base) {
            arena_reset(&_STR_INIT_ARENA, tmp_mark);
            return STR_INVALID_DIGIT;
        }
        __BIGINT_INTERNAL_MUL_UI64__(&__TEMPHOLDER__, base);
        __BIGINT_INTERNAL_ADD_UI64__(&__TEMPHOLDER__, _VALUE_LOOKUP_[lookup_index]);
    }


    //* =========== 6. FULLY Initializing and Copy-over ============== *//
    x->limbs = malloc(cap * BYTES_IN_UINT64_T);
    if (!x->limbs) {
        arena_reset(&_STR_INIT_ARENA, tmp_mark);
        abort();
    }
    memcpy(x->limbs, tmp_limbs, cap * BYTES_IN_UINT64_T);
    x->cap  = cap;
    x->n    = cap;
    x->sign = sign;
    arena_reset(&_STR_INIT_ARENA, tmp_mark);
    return STR_SUCCESS;
}
str_status __BIGINT_BASE_INIT__(bigInt *x, const char* str, uint8_t base) {
    if (x->limbs) return; // Already initialized
    if (*str == '\0') return STR_EMPTY;
    static local_thread dnml_arena _STR_INIT_ARENA;
    static local_thread bool _STR_INIT_ARENA_INITIALIZED = false;


    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    if (str[curr_pos] == '-') { sign = -1; ++curr_pos; }
    else if (str[curr_pos] == '+') ++curr_pos;
    if (str[curr_pos] == '\0') return STR_INCOMPLETE;


    //* ====== 2. Leading-Zeros Handling ====== *//
    while (str[curr_pos] == '0' && str[curr_pos] != '\0') ++curr_pos; // Skipping all leading zeros
    // String full of zeros
    if (str[curr_pos] == '\0') {
        if (sign = -1) return STR_INVALID_SIGN; // -0 is INVALID// Empty initialization
        x->limbs = malloc(sizeof(uint64_t));
        x->cap   = 1;
        x->n     = 0;
        x->sign  = 1;
        return STR_SUCCESS;
    }

    //* ======= 4. Instatiating Temporary BigInt ======= *//
    size_t d = strlen(&str[curr_pos]);
    size_t cap = ((size_t)(__BITCOUNT__(d, base) / BIGINT_LIMBS_BITS)) + 1;
    if (!_STR_INIT_ARENA_INITIALIZED) {
        arena_init(&_STR_INIT_ARENA, cap * BYTES_IN_UINT64_T);
        _STR_INIT_ARENA_INITIALIZED = true;
    }
    size_t tmp_mark = arena_mark(&_STR_INIT_ARENA);
    limb_t *tmp_limbs = arena_alloc(&_STR_INIT_ARENA, cap * BYTES_IN_UINT64_T);
    bigInt __TEMPHOLDER__ = {
        .limbs = tmp_limbs,
        .cap   = cap,
        .n     = 0,
        .sign  = sign
    };

    //* ============= 5. Parsing and Initiating Value ================ *//
    for (; curr_pos < d; ++curr_pos) {
        uint8_t lookup_index = (uint8_t)(str[curr_pos] - '\0');
        if (_VALUE_LOOKUP_[lookup_index] > base) {
            arena_reset(&_STR_INIT_ARENA, tmp_mark);
            return STR_INVALID_DIGIT;
        }
        __BIGINT_INTERNAL_MUL_UI64__(&__TEMPHOLDER__, base);
        __BIGINT_INTERNAL_ADD_UI64__(&__TEMPHOLDER__, _VALUE_LOOKUP_[lookup_index]);
    }


    //* =========== 6. FULLY Initializing and Copy-over ============== *//
    x->limbs = malloc(cap * BYTES_IN_UINT64_T);
    if (!x->limbs) {
        arena_reset(&_STR_INIT_ARENA, tmp_mark);
        abort();
    }
    memcpy(x->limbs, tmp_limbs, cap * BYTES_IN_UINT64_T);
    x->cap  = cap;
    x->n    = cap;
    x->sign = sign;
    arena_reset(&_STR_INIT_ARENA, tmp_mark);
    return STR_SUCCESS;
}
str_status __BIGINT_STRNLEN_INIT__(bigInt *x, const char* str, size_t len) {}
str_status __BIGINT_BASENLEN_INIT__(bigInt *x, const char* str, uint8_t base, size_t len) {}





//todo ================================== 2. CONVERSIONS & ASSIGNMENTS ================================== *//







//todo ======================================== 3. INPUT & OUTPUT ======================================= *//






//todo ====================================== 4. (DE)SERIALIZATION ===================================== *//







//todo ====================================== 5. GENERAL UTILITIES ===================================== *//










