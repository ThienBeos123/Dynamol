// Providing for
#include "bigInt_func.h"


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

static const char _DIGIT_[32] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'a', 'b', 'c', 'd', 'e', 'f',
};



//todo ======================================= 1. CONSTRUCTIONS ======================================= *//
str_status __BIGINT_STRING_INIT__(bigInt *x, const char* str) {
    if (x->limbs) return; // Already initialized
    if (*str == '\0') return STR_EMPTY;
    static local_thread dnml_arena _STR_INIT_ARENA;
    static local_thread bool _STR_INIT_ARENA_INITIALIZED = false;

    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    unsigned char sign_op_res = _sign_handle_(str, &curr_pos, &sign);
    if (sign_op_res == 4) return STR_INVALID_SIGN;
    else if (sign_op_res == 3) return STR_INCOMPLETE;

    //* ====== 2. Prefix Handling ====== *//
    uint8_t base = 10;
    unsigned char prefix_op_res = _prefix_handle_(str, &curr_pos, &base);
    if (prefix_op_res == 0) {
        if (sign == -1) return STR_INVALID_SIGN;
        x->limbs = malloc(BYTES_IN_UINT64_T);
        x->cap   = 1;
        x->n     = 0;
        x->sign  = 1;
        return STR_SUCCESS;
    } else if (prefix_op_res == 2) return STR_INVALID_BASE_PREFIX;


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
    size_t bits = __BITCOUNT__(d, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
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
    x->n    = __TEMPHOLDER__.n;
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

    //* ======= 3. Instatiating Temporary BigInt ======= *//
    size_t d = strlen(&str[curr_pos]);
    size_t bits = __BITCOUNT__(d, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
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

    //* ============= 4. Parsing and Initiating Value ================ *//
    for (; curr_pos < d; ++curr_pos) {
        uint8_t lookup_index = (uint8_t)(str[curr_pos] - '\0');
        if (_VALUE_LOOKUP_[lookup_index] > base) {
            arena_reset(&_STR_INIT_ARENA, tmp_mark);
            return STR_INVALID_DIGIT;
        }
        __BIGINT_INTERNAL_MUL_UI64__(&__TEMPHOLDER__, base);
        __BIGINT_INTERNAL_ADD_UI64__(&__TEMPHOLDER__, _VALUE_LOOKUP_[lookup_index]);
    }


    //* =========== 5. FULLY Initializing and Copy-over ============== *//
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




//todo ================================= 2. CONVERSIONS & ASSIGNMENTS ================================= *//
/* ------------- Conversions ------------- */
char* __BIGINT_TO_STRING__(const bigInt *x) {
    uint8_t sign_space = (x->sign == -1) ? 1 : 0; // For NULl-Terminator + Sign
    size_t str_length = __BIGINT_COUNTDB__(x, 10) + 1 + sign_space;
    // Arena & String
    static local_thread dnml_arena _BI_TO_STRING_ARENA;
    static local_thread bool process_initiate = false;
    if (!process_initiate) {
        init_arena(&_BI_TO_STRING_ARENA, x->n);
        process_initiate = true;
    } 
    char *res_str = malloc(str_length);

    //* Sign Handling
    res_str[str_length - 1] = '\0';
    if (sign_space) res_str[0] = '-';

    //* Temporary BigInt
    size_t tmp_mark = arena_mark(&_BI_TO_STRING_ARENA);
    limb_t *tmp_limbs = arena_alloc(&_BI_TO_STRING_ARENA, x->n);
    bigInt tmp_x = {
        .limbs = tmp_limbs,
        .cap   = x->n,
        .n     = x->n,
        .sign  = 1,
    };

    //* Filling up the string
    for (size_t i = str_length - 2; i >= sign_space; --i) {
        uint8_t val = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_x, 10);
        res_str[i] = _DIGIT_[val];
    }
    arena_reset(&_BI_TO_STRING_ARENA, tmp_mark);
    return res_str;
}
char* __BIGINT_TO_BASE__(const bigInt *x, uint8_t base) {
    uint8_t sign_space = (x->sign == -1) ? 1 : 0; // For NULl-Terminator + Sign
    size_t str_length = __BIGINT_COUNTDB__(x, base) + 1 + sign_space;
    // Arena & String
    static local_thread dnml_arena _BI_TO_STRING_ARENA;
    static local_thread bool process_initiate = false;
    if (!process_initiate) {
        init_arena(&_BI_TO_STRING_ARENA, x->n);
        process_initiate = true;
    } 
    char *res_str = malloc(str_length);

    //* Sign Handling
    res_str[str_length - 1] = '\0';
    if (sign_space) res_str[0] = '-';

    //* Temporary BigInt
    size_t tmp_mark = arena_mark(&_BI_TO_STRING_ARENA);
    limb_t *tmp_limbs = arena_alloc(&_BI_TO_STRING_ARENA, x->n);
    bigInt tmp_x = {
        .limbs = tmp_limbs,
        .cap   = x->n,
        .n     = x->n,
        .sign  = 1,
    };

    //* Filling up the string
    for (size_t i = str_length - 2; i >= sign_space; --i) {
        uint8_t val = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_x, base);
        res_str[i] = _DIGIT_[val];
    }
    arena_reset(&_BI_TO_STRING_ARENA, tmp_mark);
    return res_str;
}
char* __BIGINT_TO_STRINGF__(const bigInt *x, uint8_t base, bool uppercase) {
    uint8_t additional_val = (uppercase) ? 16 : 0;
    uint8_t sign_space = (x->sign == -1) ? 1 : 0; // For NULl-Terminator + Sign
    uint8_t prefix_space = (base == 2 || base == 8 || base == 16) ? 2 : 0;
    size_t str_length = __BIGINT_COUNTDB__(x, base) + 1 + sign_space + prefix_space;
    // Arena & String
    static local_thread dnml_arena _BI_TO_STRING_ARENA;
    static local_thread bool process_initiate = false;
    if (!process_initiate) {
        init_arena(&_BI_TO_STRING_ARENA, x->n);
        process_initiate = true;
    } 
    char *res_str = malloc(str_length);

    //* Sign & Prefix Handling
    res_str[str_length - 1] = '\0';
    if (sign_space) res_str[0] = '-';
    if (prefix_space) {
        res_str[0 + sign_space] = '0';
        switch (base) {
            case 2:     res_str[1 + sign_space] = 'b';
            case 8:     res_str[1 + sign_space] = 'o';
            case 16:    res_str[1 + sign_space] = 'x';
        }
    }

    //* Temporary BigInt
    size_t tmp_mark = arena_mark(&_BI_TO_STRING_ARENA);
    limb_t *tmp_limbs = arena_alloc(&_BI_TO_STRING_ARENA, x->n);
    bigInt tmp_x = {
        .limbs = tmp_limbs,
        .cap   = x->n,
        .n     = x->n,
        .sign  = 1,
    };

    //* Filling up the string
    for (size_t i = str_length - 2; i >= sign_space + prefix_space; --i) {
        uint8_t val = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_x, base);
        res_str[i] = _DIGIT_[val + additional_val];
    }
    arena_reset(&_BI_TO_STRING_ARENA, tmp_mark);
    return res_str;
}
bigInt __BIGINT_FROM_STRING__(const char* str) {}
bigInt __BIGINT_FROM_BASE__(const char* str, uint8_t base) {}
bigInt __BIGINT_FROM_STRNLEN__(const char* str, size_t n) {}
bigInt __BIGINT_FROM_BASENLEN__(const char* str, uint8_t base, size_t n) {}
/* --------- Truncative BigInt --> String ---------- */
void __BIGINT_SET_STRING__(char* str, const bigInt x) {}
void __BIGINT_SET_BASE__(char* str, const bigInt x, uint8_t base) {}
void __BIGINT_SET_STRNLEN__(char* str, const bigInt x, size_t n) {}
void __BIGINT_SET_BASENLEN__(char* str, const bigInt x, uint8_t base, size_t n) {}
/* ------------- Safe BigInt --> String ------------ */
str_status __BIGINT_SSET_STRING__(char* str, const bigInt x) {}
str_status __BIGINT_SSET_BASE__(char* str, const bigInt x, uint8_t base) {}
str_status __BIGINT_SSET_STRNLEN__(char* str, const bigInt x, size_t n) {}
str_status __BIGINT_SSET_BASENLEN__(char* str, const bigInt x, uint8_t base, size_t n) {}
/* --------- Truncative String --> BigInt ---------- */
void __BIGINT_GET_STRING__(char* str, const bigInt x) {}
void __BIGINT_GET_BASE__(char* str, const bigInt x, uint8_t base) {}
void __BIGINT_GET_STRNLEN__(char* str, const bigInt x, size_t n) {}
void __BIGINT_GET_BASENLEN__(char* str, const bigInt x, uint8_t base, size_t n) {}
/* ------------- Safe String --> BigInt ------------ */
str_status __BIGINT_SGET_STRING__(char* str, const bigInt x) {}
str_status __BIGINT_SGET_BASE__(char* str, const bigInt x, uint8_t base) {}
str_status __BIGINT_SGET_STRNLEN__(char* str, const bigInt x, size_t n) {}
str_status __BIGINT_SGET_BASENLEN__(char* str, const bigInt x, uint8_t base, size_t n) {}





//todo ======================================== 3. INPUT & OUTPUT ======================================= *//






//todo ================================= 4. SERIALIZATION & DESERIALIZATION ============================== *//







//todo ====================================== 5. GENERAL UTILITIES ===================================== *//










