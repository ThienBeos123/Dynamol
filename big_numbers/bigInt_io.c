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
        limb_t *tmp = malloc(BYTES_IN_UINT64_T);
        if (!tmp) abort();
        x->limbs = tmp;
        x->cap   = 1;
        x->n     = 0;
        x->sign  = 1;
        return STR_SUCCESS;
    } else if (prefix_op_res == 2) return STR_INVALID_BASE_PREFIX;
    // The remaining case (prefix_op_res == 1) indicates we have a decimal string with 1+ leading zero


    //* ====== 3. Leading-Zeros Handling ====== *//
    while (str[curr_pos] == '0' && str[curr_pos] != '\0') ++curr_pos; // Skipping all leading zeros
    // String full of zeros
    if (str[curr_pos] == '\0') {
        if (sign = -1) return STR_INVALID_SIGN; // -0 is INVALID// Empty initialization
        limb_t *tmp = malloc(BYTES_IN_UINT64_T);
        if (!tmp) abort();
        x->limbs = tmp;
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
        limb_t *tmp = malloc(sizeof(uint64_t));
        if (!tmp) abort(); // OOM
        x->limbs = tmp;
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
    limb_t *tmp = malloc(cap * BYTES_IN_UINT64_T);
    if (!tmp) {
        arena_reset(&_STR_INIT_ARENA, tmp_mark);
        abort();
    }
    x->limbs = tmp;
    memcpy(x->limbs, tmp_limbs, cap * BYTES_IN_UINT64_T);
    x->cap  = cap;
    x->n    = cap;
    x->sign = sign;
    arena_reset(&_STR_INIT_ARENA, tmp_mark);
    return STR_SUCCESS;
}
str_status __BIGINT_STRNLEN_INIT__(bigInt *x, const char* str, size_t len) {
    if (x->limbs) return; // Already initialized
    if (*str == '\0') return STR_EMPTY;
    static local_thread dnml_arena _STR_INIT_ARENA;
    static local_thread bool _STR_INIT_ARENA_INITIALIZED = false;

    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    unsigned char sign_op_res = _sign_handle_nlen_(str, &curr_pos, &sign, len);
    if (sign_op_res == 4) return STR_INVALID_SIGN;
    else if (sign_op_res == 3) return STR_INCOMPLETE;

    //* ====== 2. Prefix Handling ====== *//
    uint8_t base = 10;
    unsigned char prefix_op_res = _prefix_handle_nlen_(str, &curr_pos, &base, len);
    if (prefix_op_res == 0) return STR_INVALID_BASE_PREFIX; 
    else if (prefix_op_res == 3) {
        if (sign == -1) return STR_INVALID_SIGN;
        limb_t *tmp = malloc(BYTES_IN_UINT64_T);
        if (!tmp) abort();
        x->limbs = tmp;
        x->cap   = 1;
        x->n     = 0;
        x->sign  = 1;
        return STR_SUCCESS;
    } else if (prefix_op_res == 2) return STR_INVALID_BASE_PREFIX;
    // The remaining case (prefix_op_res == 1) indicates we have a decimal string with 1+ leading zero


    //* ====== 3. Leading-Zeros Handling ====== *//
    // Skipping all leading zeros
    while (str[curr_pos] == '0' && (str[curr_pos] != '\0' || curr_pos < len)) ++curr_pos;
    // String full of zeros
    if (str[curr_pos] == '\0') return STR_INVALID_DIGIT;
    else if (curr_pos == len) {
        if (sign = -1) return STR_INVALID_SIGN; // -0 is INVALID// Empty initialization
        limb_t *tmp = malloc(BYTES_IN_UINT64_T);
        if (!tmp) abort();
        x->limbs = tmp;
        x->cap   = 1;
        x->n     = 0;
        x->sign  = 1;
        return STR_SUCCESS;
    }

    //* ======= 4. Instatiating Temporary BigInt ======= *//
    size_t bits = __BITCOUNT__(len - curr_pos + 1, base);
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
    for (; curr_pos < len; ++curr_pos) {
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
str_status __BIGINT_BASENLEN_INIT__(bigInt *x, const char* str, uint8_t base, size_t len) {
    if (x->limbs) return; // Already initialized
    if (*str == '\0') return STR_EMPTY;
    static local_thread dnml_arena _STR_INIT_ARENA;
    static local_thread bool _STR_INIT_ARENA_INITIALIZED = false;

    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    if (str[curr_pos] == '-') { sign = -1; ++curr_pos; }
    else if (str[curr_pos] == '+') ++curr_pos;
    if (str[curr_pos] == '\0') return STR_INVALID_DIGIT;
    if (curr_pos == len - 1) return STR_INCOMPLETE;

    //* ====== 2. Leading-Zeros Handling ====== *//
    // Skipping all leading zeros
    while (str[curr_pos] == '0' && (str[curr_pos] != '\0' || curr_pos < len)) ++curr_pos;
    // String full of zeros
    if (str[curr_pos] == '\0') return STR_INVALID_DIGIT;
    else if (curr_pos == len) {
        if (sign = -1) return STR_INVALID_SIGN; // -0 is INVALID// Empty initialization
        limb_t *tmp = malloc(sizeof(uint64_t));
        if (!tmp) abort(); // OOM
        x->limbs = tmp;
        x->cap   = 1;
        x->n     = 0;
        x->sign  = 1;
        return STR_SUCCESS;
    }

    //* ======= 3. Instatiating Temporary BigInt ======= *//
    size_t bits = __BITCOUNT__(len - curr_pos + 1, base);
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
    for (; curr_pos < len; ++curr_pos) {
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
    x->n    = __TEMPHOLDER__.n;
    x->sign = sign;
    arena_reset(&_STR_INIT_ARENA, tmp_mark);
    return STR_SUCCESS;
}




//todo ================================= 2. CONVERSIONS & ASSIGNMENTS ================================= *//
//* -------------------------- Conversions -------------------------- *//
char* __BIGINT_TO_STRING__(const bigInt x) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    uint8_t sign_space = (x.sign == -1) ? 1 : 0; // For NULl-Terminator + Sign
    size_t str_length = __BIGINT_COUNTDB__(&x, 10) + 1 + sign_space;
    // Arena & String
    static local_thread dnml_arena _BI_TO_STRING_ARENA;
    static local_thread bool process_initiate = false;
    if (!process_initiate) {
        init_arena(&_BI_TO_STRING_ARENA, x.n);
        process_initiate = true;
    } 
    char *res_str = malloc(str_length);

    //* Sign Handling
    res_str[str_length - 1] = '\0';
    if (sign_space) res_str[0] = '-';

    //* Temporary BigInt
    size_t tmp_mark = arena_mark(&_BI_TO_STRING_ARENA);
    limb_t *tmp_limbs = arena_alloc(&_BI_TO_STRING_ARENA, x.n);
    bigInt tmp_x = {
        .limbs = tmp_limbs,
        .cap   = x.n,
        .n     = x.n,
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
char* __BIGINT_TO_BASE__(const bigInt x, uint8_t base) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    uint8_t sign_space = (x.sign == -1) ? 1 : 0; // For NULl-Terminator + Sign
    size_t str_length = __BIGINT_COUNTDB__(&x, base) + 1 + sign_space;
    // Arena & String
    static local_thread dnml_arena _BI_TO_STRING_ARENA;
    static local_thread bool process_initiate = false;
    if (!process_initiate) {
        init_arena(&_BI_TO_STRING_ARENA, x.n);
        process_initiate = true;
    } 
    char *res_str = malloc(str_length);

    //* Sign Handling
    res_str[str_length - 1] = '\0';
    if (sign_space) res_str[0] = '-';

    //* Temporary BigInt
    size_t tmp_mark = arena_mark(&_BI_TO_STRING_ARENA);
    limb_t *tmp_limbs = arena_alloc(&_BI_TO_STRING_ARENA, x.n);
    bigInt tmp_x = {
        .limbs = tmp_limbs,
        .cap   = x.n,
        .n     = x.n,
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
char* __BIGINT_TO_STRINGF__(const bigInt x, uint8_t base, bool uppercase) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    uint8_t additional_val = (uppercase) ? 16 : 0;
    uint8_t sign_space = (x.sign == -1) ? 1 : 0; // For NULl-Terminator + Sign
    uint8_t prefix_space = (base == 2 || base == 8 || base == 16) ? 2 : 0;
    size_t str_length = __BIGINT_COUNTDB__(&x, base) + 1 + sign_space + prefix_space;
    // Arena & String
    static local_thread dnml_arena _BI_TO_STRING_ARENA;
    static local_thread bool process_initiate = false;
    if (!process_initiate) {
        init_arena(&_BI_TO_STRING_ARENA, x.n);
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
    limb_t *tmp_limbs = arena_alloc(&_BI_TO_STRING_ARENA, x.n);
    bigInt tmp_x = {
        .limbs = tmp_limbs,
        .cap   = x.n,
        .n     = x.n,
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
bigInt __BIGINT_FROM_STRING__(const char* str, str_status *err) {
    assert(err); if (!str) { *err = STR_NULL; return __BIGINT_ERROR_VALUE__(); }
    if (*str == '\0') { *err = STR_EMPTY; return __BIGINT_ERROR_VALUE__(); }
    bigInt res;
    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    unsigned char sign_op_res = _sign_handle_(str, &curr_pos, &sign);
    if (sign_op_res == 4) { *err = STR_INVALID_SIGN; return __BIGINT_ERROR_VALUE__(); }
    else if (sign_op_res == 3) { *err = STR_INCOMPLETE; return __BIGINT_ERROR_VALUE__(); }

    //* ====== 2. Prefix Handling ====== *//
    uint8_t base = 10;
    unsigned char prefix_op_res = _prefix_handle_(str, &curr_pos, &base);
    if (prefix_op_res == 0) {
        if (sign == -1) { *err = STR_INVALID_SIGN; return __BIGINT_ERROR_VALUE__(); } // 
        limb_t *tmp = malloc(BYTES_IN_UINT64_T);
        if (!tmp) abort();
        res.limbs = tmp;
        res.cap   = 1;
        res.n     = 0;
        res.sign  = 1;
        *err = STR_SUCCESS; return res;
    } else if (prefix_op_res == 2) { *err = STR_INVALID_BASE_PREFIX; return __BIGINT_ERROR_VALUE__(); }
    // The remaining case (prefix_op_res == 1) indicates we have a decimal string with 1+ leading zero

    //* ====== 3. Leading-Zeros Handling ====== *//
    while (str[curr_pos] == '0' && str[curr_pos] != '\0') ++curr_pos; // Skipping all leading zeros
    // String full of zeros
    if (str[curr_pos] == '\0') {
        // -0 is INVALID
        if (sign = -1) { *err = STR_INVALID_SIGN; return __BIGINT_ERROR_VALUE__(); };
        limb_t *tmp = malloc(BYTES_IN_UINT64_T);
        if (!tmp) abort();
        res.limbs = tmp;
        res.cap   = 1;
        res.n     = 0;
        res.sign  = 1;
        *err = STR_SUCCESS; return res;
    }

    //* ======= 4. Initiating Resulting BigInt ======= *//
    size_t d = strlen(&str[curr_pos]);
    size_t bits = __BITCOUNT__(d, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    limb_t *tmp = malloc(BYTES_IN_UINT64_T * cap);
    if (!tmp) abort();
    res.limbs = tmp; res.sign = sign;
    res.cap   = cap; res.n = cap;

    //* ============= 5. Parsing and Initiating Value ================ *//
    for (; curr_pos < d; ++curr_pos) {
        uint8_t lookup_index = (uint8_t)(str[curr_pos] - '\0');
        if (_VALUE_LOOKUP_[lookup_index] > base) {
            *err = STR_INVALID_DIGIT;
            free(res.limbs); return __BIGINT_ERROR_VALUE__();
        }
        __BIGINT_INTERNAL_MUL_UI64__(&res, base);
        __BIGINT_INTERNAL_ADD_UI64__(&res, _VALUE_LOOKUP_[lookup_index]);
    } return res;
}
bigInt __BIGINT_FROM_BASE__(const char* str, uint8_t base, str_status *err) {
    assert(err); if (!str) { *err = STR_NULL; return __BIGINT_ERROR_VALUE__(); }
    if (*str == '\0') { *err = STR_EMPTY; return __BIGINT_ERROR_VALUE__(); }
    bigInt res;
    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    if (str[curr_pos] == '-') { sign = -1; ++curr_pos; }
    else if (str[curr_pos] == '+') ++curr_pos;
    if (str[curr_pos] == '\0') { *err = STR_INCOMPLETE; return __BIGINT_ERROR_VALUE__(); }

    //* ====== 2. Leading-Zeros Handling ====== *//
    while (str[curr_pos] == '0' && str[curr_pos] != '\0') ++curr_pos; // Skipping all leading zeros
    // String full of zeros
    if (str[curr_pos] == '\0') {
        // -0 is INVALID
        if (sign = -1) { *err = STR_INVALID_SIGN; return __BIGINT_ERROR_VALUE__(); };
        limb_t *tmp = malloc(BYTES_IN_UINT64_T);
        if (!tmp) abort();
        res.limbs = tmp;
        res.cap   = 1;
        res.n     = 0;
        res.sign  = 1;
        *err = STR_SUCCESS; return res;
    }

    //* ======= 3. Initiating Resulting BigInt ======= *//
    size_t d = strlen(&str[curr_pos]);
    size_t bits = __BITCOUNT__(d, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    limb_t *tmp = malloc(BYTES_IN_UINT64_T * cap);
    if (!tmp) abort();
    res.limbs = tmp; res.sign = sign;
    res.cap   = cap; res.n = cap;

    //* ============= 4. Parsing and Initiating Value ================ *//
    for (; curr_pos < d; ++curr_pos) {
        uint8_t lookup_index = (uint8_t)(str[curr_pos] - '\0');
        if (_VALUE_LOOKUP_[lookup_index] > base) {
            *err = STR_INVALID_DIGIT;
            free(res.limbs); return __BIGINT_ERROR_VALUE__();
        }
        __BIGINT_INTERNAL_MUL_UI64__(&res, base);
        __BIGINT_INTERNAL_ADD_UI64__(&res, _VALUE_LOOKUP_[lookup_index]);
    } return res;
}
bigInt __BIGINT_FROM_STRNLEN__(const char* str, size_t len, str_status *err) {
    assert(err); if (!str) { *err = STR_NULL; return __BIGINT_ERROR_VALUE__(); }
    if (*str == '\0') { *err = STR_EMPTY; return __BIGINT_ERROR_VALUE__(); }
    bigInt res;
    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    unsigned char sign_op_res = _sign_handle_nlen_(str, &curr_pos, &sign, len);
    if (sign_op_res == 4) { *err = STR_INVALID_SIGN; return __BIGINT_ERROR_VALUE__(); }
    else if (sign_op_res == 3) { *err = STR_INCOMPLETE; return __BIGINT_ERROR_VALUE__(); }

    //* ====== 2. Prefix Handling ====== *//
    uint8_t base = 10;
    unsigned char prefix_op_res = _prefix_handle_nlen_(str, &curr_pos, &base, len);
    if (prefix_op_res == 0) { *err = STR_INVALID_BASE_PREFIX; return __BIGINT_ERROR_VALUE__(); }
    else if (prefix_op_res == 3) {
        if (sign == -1) { *err = STR_INVALID_SIGN; return __BIGINT_ERROR_VALUE__(); }
        limb_t *tmp = malloc(BYTES_IN_UINT64_T);
        if (!tmp) abort();
        res.limbs = tmp;
        res.cap   = 1;
        res.n     = 0;
        res.cap  = 1;
        *err = STR_SUCCESS; return res; 
    } else if (prefix_op_res == 2) { *err = STR_INVALID_BASE_PREFIX; return __BIGINT_ERROR_VALUE__(); }
    // The remaining case (prefix_op_res == 1) indicates we have a decimal string with 1+ leading zero

    //* ====== 3. Leading-Zeros Handling ====== *//
    // Skipping all leading zeros
    while (str[curr_pos] == '0' && (str[curr_pos] != '\0' || curr_pos < len)) ++curr_pos;
    // String full of zeros
    if (str[curr_pos] == '\0') { *err = STR_INVALID_DIGIT; return __BIGINT_ERROR_VALUE__(); }
    else if (curr_pos == len) {
        // -0 is INVALID
        if (sign = -1) { *err = STR_INVALID_SIGN; return __BIGINT_ERROR_VALUE__(); }
        // Empty initialization
        limb_t *tmp = malloc(BYTES_IN_UINT64_T);
        if (!tmp) abort();
        res.limbs = tmp;
        res.cap   = 1;
        res.n     = 0;
        res.sign  = 1;
        *err = STR_SUCCESS; return res;
    }

    //* ======= 4. Initiating Resulting BigInt ======= *//
    size_t bits = __BITCOUNT__(len, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    limb_t *tmp = malloc(BYTES_IN_UINT64_T * cap);
    if (!tmp) abort();
    res.limbs = tmp; res.sign = sign;
    res.cap   = cap; res.n = cap;

    //* ============= 5. Parsing and Initiating Value ================ *//
    for (; curr_pos < len; ++curr_pos) {
        uint8_t lookup_index = (uint8_t)(str[curr_pos] - '\0');
        if (_VALUE_LOOKUP_[lookup_index] > base) {
            *err = STR_INVALID_DIGIT;
            free(res.limbs); return __BIGINT_ERROR_VALUE__();
        }
        __BIGINT_INTERNAL_MUL_UI64__(&res, base);
        __BIGINT_INTERNAL_ADD_UI64__(&res, _VALUE_LOOKUP_[lookup_index]);
    } return res;
}
bigInt __BIGINT_FROM_BASENLEN__(const char* str, uint8_t base, size_t len, str_status *err) {
    assert(err); if (!str) { *err = STR_NULL; return __BIGINT_ERROR_VALUE__(); }
    if (*str == '\0') { *err = STR_EMPTY; return __BIGINT_ERROR_VALUE__(); }
    bigInt res;
    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    if (str[curr_pos] == '-') { sign = -1; ++curr_pos; }
    else if (str[curr_pos] == '+') ++curr_pos;
    if (str[curr_pos] == '\0') { *err = STR_INVALID_DIGIT; return __BIGINT_ERROR_VALUE__(); };
    if (curr_pos == len - 1) { *err = STR_INCOMPLETE; return __BIGINT_ERROR_VALUE__(); }

    //* ====== 2. Leading-Zeros Handling ====== *//
    // Skipping all leading zeros
    while (str[curr_pos] == '0' && (str[curr_pos] != '\0' || curr_pos < len)) ++curr_pos;
    // String full of zeros
    if (str[curr_pos] == '\0') { *err = STR_INVALID_DIGIT; return __BIGINT_ERROR_VALUE__(); }
    else if (curr_pos == len) {
        // -0 is INVALID
        if (sign = -1) { *err = STR_INVALID_SIGN; return __BIGINT_ERROR_VALUE__(); }
        // Empty initialization
        limb_t *tmp = malloc(BYTES_IN_UINT64_T);
        if (!tmp) abort();
        res.limbs = tmp;
        res.cap   = 1;
        res.n     = 0;
        res.sign  = 1;
        *err = STR_SUCCESS; return res;
    }

    //* ======= 3. Initiating Resulting BigInt ======= *//
    size_t bits = __BITCOUNT__(len, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    limb_t *tmp = malloc(BYTES_IN_UINT64_T * cap);
    if (!tmp) abort();
    res.limbs = tmp; res.sign = sign;
    res.cap   = cap; res.n = cap;

    //* ============= 4. Parsing and Initiating Value ================ *//
    for (; curr_pos < len; ++curr_pos) {
        uint8_t lookup_index = (uint8_t)(str[curr_pos] - '\0');
        if (_VALUE_LOOKUP_[lookup_index] > base) {
            *err = STR_INVALID_DIGIT;
            free(res.limbs); return __BIGINT_ERROR_VALUE__();
        }
        __BIGINT_INTERNAL_MUL_UI64__(&res, base);
        __BIGINT_INTERNAL_ADD_UI64__(&res, _VALUE_LOOKUP_[lookup_index]);
    } return res;
}
//* -------------------------- Assignments -------------------------- *//
/* --------- Truncative BigInt --> String ---------- */
str_status __BIGINT_TSET_STRING__(char* str, const bigInt x) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    static local_thread dnml_arena _BI_TSET_STRING_ARENA;
    static local_thread bool _ARENA_INITIATED_ = false;
    if (!_ARENA_INITIATED_) {
        init_arena(&_BI_TSET_STRING_ARENA, x.n * BYTES_IN_UINT64_T);
        _ARENA_INITIATED_ = true;
    }
    if (!str) return STR_NULL;
    size_t str_length = strlen(str); // Early segfauly if no NULL-Terminator found
    uint8_t sign_space = (x.sign == -1) ? 1 : 0;
    if (str_length <= sign_space) return STR_INVALID_CAP;
    if (sign_space) str[0] = '-';
    size_t tmp_mark = arena_mark(&_BI_TSET_STRING_ARENA);
    limb_t *tmp_limbs = arena_alloc(&_BI_TSET_STRING_ARENA, x.n * BYTES_IN_UINT64_T);
    memcpy(tmp_limbs, x.limbs, x.n * BYTES_IN_UINT64_T);
    bigInt tmp_buf = {
        .limbs = tmp_limbs, .sign = x.sign,
        .cap   = x.n,       .n    = x.n,
    };

    for (size_t i = str_length - 1; i >= sign_space; --i) {
        uint8_t numeric_value = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, 10);
        str[i] = _DIGIT_[numeric_value];
    }
    size_t digit_needed = __BIGINT_COUNTDB__(&x, 10);
    if (digit_needed < str_length) memset(&str[sign_space], '0', str_length - digit_needed);
    arena_reset(&_BI_TSET_STRING_ARENA, tmp_mark);
    return STR_SUCCESS;
}
str_status __BIGINT_TSET_BASE__(char* str, const bigInt x, uint8_t base) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    static local_thread dnml_arena _BI_TSET_BASE_ARENA;
    static local_thread bool _ARENA_INITIATED_ = false;
    if (!_ARENA_INITIATED_) {
        init_arena(&_BI_TSET_BASE_ARENA, x.n * BYTES_IN_UINT64_T);
        _ARENA_INITIATED_ = true;
    }
    if (!str) return STR_NULL;
    size_t str_length = strlen(str); // Early segfauly if no NULL-Terminator found
    uint8_t sign_space = (x.sign == -1) ? 1 : 0;
    if (str_length <= sign_space) return STR_INVALID_CAP;
    if (sign_space) str[0] = '-';
    size_t tmp_mark = arena_mark(&_BI_TSET_BASE_ARENA);
    limb_t *tmp_limbs = arena_alloc(&_BI_TSET_BASE_ARENA, x.n * BYTES_IN_UINT64_T);
    memcpy(tmp_limbs, x.limbs, x.n * BYTES_IN_UINT64_T);
    bigInt tmp_buf = {
        .limbs = tmp_limbs, .sign = x.sign,
        .cap   = x.n,       .n    = x.n,
    };

    for (size_t i = str_length - 1; i >= sign_space; --i) {
        uint8_t numeric_value = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, base);
        str[i] = _DIGIT_[numeric_value];
    }
    size_t digit_needed = __BIGINT_COUNTDB__(&x, base);
    if (digit_needed < str_length) memset(&str[sign_space], '0', str_length - digit_needed);
    arena_reset(&_BI_TSET_BASE_ARENA, tmp_mark);
    return STR_SUCCESS;
}
str_status __BIGINT_TSET_STRNLEN__(char* str, const bigInt x, size_t len) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    static local_thread dnml_arena _BI_TSET_STRNLEN_ARENA;
    static local_thread bool _ARENA_INITIATED_ = false;
    if (!_ARENA_INITIATED_) {
        init_arena(&_BI_TSET_STRNLEN_ARENA, x.n * BYTES_IN_UINT64_T);
        _ARENA_INITIATED_ = true;
    }
    if (!str) return STR_NULL;
    uint8_t sign_space = (x.sign == -1) ? 1 : 0;
    if (len <= sign_space) return STR_INVALID_CAP;
    if (sign_space) str[0] = '-';
    size_t tmp_mark = arena_mark(&_BI_TSET_STRNLEN_ARENA);
    limb_t *tmp_limbs = arena_alloc(&_BI_TSET_STRNLEN_ARENA, x.n * BYTES_IN_UINT64_T);
    memcpy(tmp_limbs, x.limbs, x.n * BYTES_IN_UINT64_T);
    bigInt tmp_buf = {
        .limbs = tmp_limbs, .sign = x.sign,
        .cap   = x.n,       .n    = x.n,
    };

    for (size_t i = len - 1; i >= sign_space; --i) {
        uint8_t numeric_value = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, 10);
        str[i] = _DIGIT_[numeric_value];
    }
    size_t digit_needed = __BIGINT_COUNTDB__(&x, 10);
    if (digit_needed < len) memset(&str[sign_space], '0', len - digit_needed);
    arena_reset(&_BI_TSET_STRNLEN_ARENA, tmp_mark);
    return STR_SUCCESS;
}
str_status __BIGINT_TSET_BASENLEN__(char* str, const bigInt x, uint8_t base, size_t len) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    static local_thread dnml_arena _BI_TSET_BASENLEN_ARENA;
    static local_thread bool _ARENA_INITIATED_ = false;
    if (!_ARENA_INITIATED_) {
        init_arena(&_BI_TSET_BASENLEN_ARENA, x.n * BYTES_IN_UINT64_T);
        _ARENA_INITIATED_ = true;
    }
    if (!str) return STR_NULL;
    uint8_t sign_space = (x.sign == -1) ? 1 : 0;
    if (len <= sign_space) return STR_INVALID_CAP;
    if (sign_space) str[0] = '-';
    size_t tmp_mark = arena_mark(&_BI_TSET_BASENLEN_ARENA);
    limb_t *tmp_limbs = arena_alloc(&_BI_TSET_BASENLEN_ARENA, x.n * BYTES_IN_UINT64_T);
    memcpy(tmp_limbs, x.limbs, x.n * BYTES_IN_UINT64_T);
    bigInt tmp_buf = {
        .limbs = tmp_limbs, .sign = x.sign,
        .cap   = x.n,       .n    = x.n,
    };

    for (size_t i = len - 1; i >= sign_space; --i) {
        uint8_t numeric_value = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, base);
        str[i] = _DIGIT_[numeric_value];
    }
    size_t digit_needed = __BIGINT_COUNTDB__(&x, base);
    if (digit_needed < len) memset(&str[sign_space], '0', len - digit_needed);
    arena_reset(&_BI_TSET_BASENLEN_ARENA, tmp_mark);
    return STR_SUCCESS;
}
/* ------------- Safe BigInt --> String ------------ */
str_status __BIGINT_SET_STRING__(char* str, const bigInt x) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    static local_thread dnml_arena _BI_SSET_STRING_ARENA;
    static local_thread bool _ARENA_INITIATED_ = false;
    if (!_ARENA_INITIATED_) {
        init_arena(&_BI_SSET_STRING_ARENA, x.n * BYTES_IN_UINT64_T);
        _ARENA_INITIATED_ = true;
    }
    if (!str) return STR_NULL;
    size_t str_length = strlen(str);
    uint8_t sign_space = (x.sign == -1) ? 1 : 0;
    if (str_length <= sign_space) return STR_INVALID_CAP;
    size_t digit_needed = __BIGINT_COUNTDB__(&x, 10);
    if (str_length < digit_needed + sign_space) return STR_INVALID_CAP;
    if (sign_space) str[0] = '-';
    size_t tmp_mark = arena_mark(&_BI_SSET_STRING_ARENA);
    limb_t *tmp_limbs = arena_alloc(&_BI_SSET_STRING_ARENA, x.n * BYTES_IN_UINT64_T);
    memcpy(tmp_limbs, x.limbs, x.n * BYTES_IN_UINT64_T);
    bigInt tmp_buf = {
        .limbs = tmp_limbs, .sign = x.sign,
        .cap   = x.n,       .n    = x.n,
    };

    for (size_t i = str_length - 1; i >= sign_space; --i) {
        uint8_t numeric_value = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, 10);
        str[i] = _DIGIT_[numeric_value];
    }
    if (digit_needed < str_length) memset(&str[sign_space], '0', str_length - digit_needed);
    arena_reset(&_BI_SSET_STRING_ARENA, tmp_mark);
    return STR_SUCCESS;
}
str_status __BIGINT_SET_BASE__(char* str, const bigInt x, uint8_t base) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    static local_thread dnml_arena _BI_SSET_BASE_ARENA;
    static local_thread bool _ARENA_INITIATED_ = false;
    if (!_ARENA_INITIATED_) {
        init_arena(&_BI_SSET_BASE_ARENA, x.n * BYTES_IN_UINT64_T);
        _ARENA_INITIATED_ = true;
    }
    if (!str) return STR_NULL;
    size_t str_length = strlen(str);
    uint8_t sign_space = (x.sign == -1) ? 1 : 0;
    if (str_length <= sign_space) return STR_INVALID_CAP;
    size_t digit_needed = __BIGINT_COUNTDB__(&x, base);
    if (str_length < digit_needed + sign_space) return STR_INVALID_CAP;
    if (sign_space) str[0] = '-';
    size_t tmp_mark = arena_mark(&_BI_SSET_BASE_ARENA);
    limb_t *tmp_limbs = arena_alloc(&_BI_SSET_BASE_ARENA, x.n * BYTES_IN_UINT64_T);
    memcpy(tmp_limbs, x.limbs, x.n * BYTES_IN_UINT64_T);
    bigInt tmp_buf = {
        .limbs = tmp_limbs, .sign = x.sign,
        .cap   = x.n,       .n    = x.n,
    };

    for (size_t i = str_length - 1; i >= sign_space; --i) {
        uint8_t numeric_value = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, base);
        str[i] = _DIGIT_[numeric_value];
    }
    if (digit_needed < str_length) memset(&str[sign_space], '0', str_length - digit_needed);
    arena_reset(&_BI_SSET_BASE_ARENA, tmp_mark);
    return STR_SUCCESS;
}
str_status __BIGINT_SET_STRNLEN__(char* str, const bigInt x, size_t len) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    static local_thread dnml_arena _BI_SSET_BASE_ARENA;
    static local_thread bool _ARENA_INITIATED_ = false;
    if (!_ARENA_INITIATED_) {
        init_arena(&_BI_SSET_BASE_ARENA, x.n * BYTES_IN_UINT64_T);
        _ARENA_INITIATED_ = true;
    }
    if (!str) return STR_NULL;
    uint8_t sign_space = (x.sign == -1) ? 1 : 0;
    if (len <= sign_space) return STR_INVALID_CAP;
    size_t digit_needed = __BIGINT_COUNTDB__(&x, 10);
    if (len < digit_needed + sign_space) return STR_INVALID_CAP;
    if (sign_space) str[0] = '-';
    size_t tmp_mark = arena_mark(&_BI_SSET_BASE_ARENA);
    limb_t *tmp_limbs = arena_alloc(&_BI_SSET_BASE_ARENA, x.n * BYTES_IN_UINT64_T);
    memcpy(tmp_limbs, x.limbs, x.n * BYTES_IN_UINT64_T);
    bigInt tmp_buf = {
        .limbs = tmp_limbs, .sign = x.sign,
        .cap   = x.n,       .n    = x.n,
    };

    for (size_t i = len - 1; i >= sign_space; --i) {
        uint8_t numeric_value = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, 10);
        str[i] = _DIGIT_[numeric_value];
    }
    if (digit_needed < len) memset(&str[sign_space], '0', len - digit_needed);
    arena_reset(&_BI_SSET_BASE_ARENA, tmp_mark);
    return STR_SUCCESS;
}
str_status __BIGINT_SET_BASENLEN__(char* str, const bigInt x, uint8_t base, size_t len) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    static local_thread dnml_arena _BI_SSET_BASE_ARENA;
    static local_thread bool _ARENA_INITIATED_ = false;
    if (!_ARENA_INITIATED_) {
        init_arena(&_BI_SSET_BASE_ARENA, x.n * BYTES_IN_UINT64_T);
        _ARENA_INITIATED_ = true;
    }
    if (!str) return STR_NULL;
    uint8_t sign_space = (x.sign == -1) ? 1 : 0;
    if (len <= sign_space) return STR_INVALID_CAP;
    size_t digit_needed = __BIGINT_COUNTDB__(&x, base);
    if (len < digit_needed + sign_space) return STR_INVALID_CAP;
    if (sign_space) str[0] = '-';
    size_t tmp_mark = arena_mark(&_BI_SSET_BASE_ARENA);
    limb_t *tmp_limbs = arena_alloc(&_BI_SSET_BASE_ARENA, x.n * BYTES_IN_UINT64_T);
    memcpy(tmp_limbs, x.limbs, x.n * BYTES_IN_UINT64_T);
    bigInt tmp_buf = {
        .limbs = tmp_limbs, .sign = x.sign,
        .cap   = x.n,       .n    = x.n,
    };

    for (size_t i = len - 1; i >= sign_space; --i) {
        uint8_t numeric_value = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, base);
        str[i] = _DIGIT_[numeric_value];
    }
    if (digit_needed < len) memset(&str[sign_space], '0', len - digit_needed);
    arena_reset(&_BI_SSET_BASE_ARENA, tmp_mark);
    return STR_SUCCESS;
}
/* ----------- Default String --> BigInt ----------- */
str_status __BIGINT_GET_STRING__(bigInt *x, const char *str) {
    assert(__BIGINT_INTERNAL_PVALID__(x));
    if (!str) return STR_NULL;
    if (*str == '\0') return STR_EMPTY;
    static local_thread dnml_arena _BI_GET_STRING_ARENA;
    static local_thread bool _ARENA_INITIATED_ = false;
    if (!_ARENA_INITIATED_) {
        init_arena(&_BI_GET_STRING_ARENA, x->n);
        _ARENA_INITIATED_ = true;
    }
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
        x->n = 0; x->sign = 1;
    } else if (prefix_op_res == 2) return STR_INVALID_BASE_PREFIX;
    // The remaining case (prefix_op_res == 1) indicates we have a decimal string with 1+ leading zero

    //* ====== 3. Leading-Zeros Handling ====== *//
    while (str[curr_pos] == '0' && str[curr_pos] != '\0') ++curr_pos; // Skipping all leading zeros
    // String full of zeros
    if (str[curr_pos] == '\0') {
        // -0 is INVALID
        if (sign = -1) return STR_INVALID_SIGN;
        x->n = 0; x->sign = 1;
        return STR_SUCCESS;
    }

    //* ======= 4. Initiating Resulting BigInt ======= *//
    size_t d = strlen(&str[curr_pos]);
    size_t bits = __BITCOUNT__(d, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    if (x->cap < cap) __BIGINT_INTERNAL_ENSCAP__(x, cap);
    size_t tmp_mark = arena_mark(&_BI_GET_STRING_ARENA);
    limb_t *tmp_limbs = arena_alloc(&_BI_GET_STRING_ARENA, cap * BYTES_IN_UINT64_T);
    bigInt tmp_buf = {
        .limbs = tmp_limbs, .sign = sign,
        .cap   = cap,      .n     = cap
    };

    //* ============= 5. Parsing and Initiating Value ================ *//
    for (; curr_pos < d; ++curr_pos) {
        uint8_t lookup_index = (uint8_t)(str[curr_pos] - '\0');
        if (_VALUE_LOOKUP_[lookup_index] > base) {
            arena_reset(&_BI_GET_STRING_ARENA, tmp_mark);
            return STR_INVALID_DIGIT;
        }
        __BIGINT_INTERNAL_MUL_UI64__(&tmp_buf, base);
        __BIGINT_INTERNAL_ADD_UI64__(&tmp_buf, _VALUE_LOOKUP_[lookup_index]);
    }
    memcpy(x->limbs, tmp_limbs, cap * BYTES_IN_UINT64_T);
    x->n = cap; x->n = cap; x->sign = sign;
    arena_reset(&_BI_GET_STRING_ARENA, tmp_mark);
    return STR_SUCCESS;
}
str_status __BIGINT_GET_BASE__(bigInt *x, const char *str, uint8_t base) {
    assert(__BIGINT_INTERNAL_PVALID__(x));
    if (!str) return STR_NULL;
    if (*str == '\0') return STR_EMPTY;
    static local_thread dnml_arena _BI_GET_STRING_ARENA;
    static local_thread bool _ARENA_INITIATED_ = false;
    if (!_ARENA_INITIATED_) {
        init_arena(&_BI_GET_STRING_ARENA, x->n);
        _ARENA_INITIATED_ = true;
    }
    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    if (str[curr_pos] == '-') { sign = -1; ++curr_pos; }
    else if (str[curr_pos] == '+') ++curr_pos;
    if (str[curr_pos] == '\0') return STR_INCOMPLETE;

    //* ====== 2. Leading-Zeros Handling ====== *//
    while (str[curr_pos] == '0' && str[curr_pos] != '\0') ++curr_pos; // Skipping all leading zeros
    // String full of zeros
    if (str[curr_pos] == '\0') {
        // -0 is INVALID
        if (sign = -1) return STR_INVALID_SIGN;
        x->n = 0; x->sign = 1;
        return STR_SUCCESS;
    }

    //* ======= 3. Initiating Resulting BigInt ======= *//
    size_t d = strlen(&str[curr_pos]);
    size_t bits = __BITCOUNT__(d, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    if (x->cap < cap) __BIGINT_INTERNAL_ENSCAP__(x, cap);
    size_t tmp_mark = arena_mark(&_BI_GET_STRING_ARENA);
    limb_t *tmp_limbs = arena_alloc(&_BI_GET_STRING_ARENA, cap * BYTES_IN_UINT64_T);
    bigInt tmp_buf = {
        .limbs = tmp_limbs, .sign = sign,
        .cap   = cap,      .n     = cap
    };


    //* ============= 4. Parsing and Initiating Value ================ *//
    for (; curr_pos < d; ++curr_pos) {
        uint8_t lookup_index = (uint8_t)(str[curr_pos] - '\0');
        if (_VALUE_LOOKUP_[lookup_index] > base) {
            arena_reset(&_BI_GET_STRING_ARENA, tmp_mark);
            return STR_INVALID_DIGIT;
        }
        __BIGINT_INTERNAL_MUL_UI64__(&tmp_buf, base);
        __BIGINT_INTERNAL_ADD_UI64__(&tmp_buf, _VALUE_LOOKUP_[lookup_index]);
    }
    memcpy(x->limbs, tmp_limbs, cap * BYTES_IN_UINT64_T);
    x->n = cap; x->n = cap; x->sign = sign;
    arena_reset(&_BI_GET_STRING_ARENA, tmp_mark);
    return STR_SUCCESS;
}
str_status __BIGINT_GET_STRNLEN__(bigInt *x, const char *str, size_t len) {
    assert(__BIGINT_INTERNAL_PVALID__(x));
    if (!str) return STR_NULL;
    if (*str == '\0') return STR_EMPTY;
    static local_thread dnml_arena _BI_GET_STRNLEN_ARENA;
    static local_thread bool _ARENA_INITIATED_ = false;
    if (!_ARENA_INITIATED_) {
        init_arena(&_BI_GET_STRNLEN_ARENA, x->n);
        _ARENA_INITIATED_ = true;
    }
     //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    unsigned char sign_op_res = _sign_handle_nlen_(str, &curr_pos, &sign, len);
    if (sign_op_res == 4) return STR_INVALID_SIGN;
    else if (sign_op_res == 3) return STR_INCOMPLETE;

    //* ====== 2. Prefix Handling ====== *//
    uint8_t base = 10;
    unsigned char prefix_op_res = _prefix_handle_nlen_(str, &curr_pos, &base, len);
    if (prefix_op_res == 0) return STR_INVALID_BASE_PREFIX;
    else if (prefix_op_res == 3) {
        if (sign == -1) return STR_INVALID_SIGN;
        x->n = 0; x->sign = 1;
        return STR_SUCCESS;
    } else if (prefix_op_res == 2) return STR_INVALID_BASE_PREFIX;
    // The remaining case (prefix_op_res == 1) indicates we have a decimal string with 1+ leading zero

    //* ====== 3. Leading-Zeros Handling ====== *//
    // Skipping all leading zeros
    while (str[curr_pos] == '0' && (str[curr_pos] != '\0' || curr_pos < len)) ++curr_pos;
    // String full of zeros
    if (str[curr_pos] == '\0') return STR_INVALID_DIGIT;
    else if (curr_pos == len) {
        // -0 is INVALID
        if (sign == -1) return STR_INVALID_SIGN;
        x->n = 0; x->sign = 1;
        return STR_SUCCESS;
    }

    //* ======= 4. Initiating Resulting BigInt ======= *//
    size_t bits = __BITCOUNT__(len, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    if (x->cap < cap) __BIGINT_INTERNAL_ENSCAP__(x, cap);
    size_t tmp_mark = arena_mark(&_BI_GET_STRNLEN_ARENA);
    limb_t *tmp_limbs = arena_alloc(&_BI_GET_STRNLEN_ARENA, cap * BYTES_IN_UINT64_T);
    bigInt tmp_buf = {
        .limbs = tmp_limbs, .sign = sign,
        .cap   = cap,      .n     = cap
    };

    //* ============= 5. Parsing and Initiating Value ================ *//
    for (; curr_pos < len; ++curr_pos) {
        uint8_t lookup_index = (uint8_t)(str[curr_pos] - '\0');
        if (_VALUE_LOOKUP_[lookup_index] > base) {
            arena_reset(&_BI_GET_STRNLEN_ARENA, tmp_mark);
            return STR_INVALID_DIGIT;
        }
        __BIGINT_INTERNAL_MUL_UI64__(&tmp_buf, base);
        __BIGINT_INTERNAL_ADD_UI64__(&tmp_buf, _VALUE_LOOKUP_[lookup_index]);
    }
    memcpy(x->limbs, tmp_limbs, cap * BYTES_IN_UINT64_T);
    x->n = cap; x->n = cap; x->sign = sign;
    arena_reset(&_BI_GET_STRNLEN_ARENA, tmp_mark);
    return STR_SUCCESS;
}
str_status __BIGINT_GET_BASENLEN__(bigInt *x, const char *str, uint8_t base, size_t n) {}
/* --------- Truncative String --> BigInt ---------- */
str_status __BIGINT_TGET_STRING__(bigInt *x, const char *str) {}
str_status __BIGINT_TGET_BASE__(bigInt *x, const char *str, uint8_t base) {}
str_status __BIGINT_TGET_STRNLEN__(bigInt *x, const char *str, size_t n) {}
str_status __BIGINT_TGET_BASENLEN__(bigInt *x, const char *str, uint8_t base, size_t n) {}
/* ------------- Safe String --> BigInt ------------ */
str_status __BIGINT_SGET_STRING__(bigInt *x, const char *str) {}
str_status __BIGINT_SGET_BASE__(bigInt *x, const char *str, uint8_t base) {}
str_status __BIGINT_SGET_STRNLEN__(bigInt *x, const char *str, size_t n) {}
str_status __BIGINT_SGET_BASENLEN__(bigInt *x, const char *str, uint8_t base, size_t n) {}




//todo ======================================== 3. INPUT & OUTPUT ======================================= *//
/* --------- Decimal Incremental OUTPUT ---------  */
void __BIGINT_PUT__(const bigInt x) {}
void __BIGINT_PUTB__(const bigInt x, uint8_t base) {}
void __BIGINT_PUTF__(const bigInt x, uint8_t base) {}
void __BIGINT_FPUT__(FILE *stream, const bigInt x) {}
void __BIGINT_FPUTB__(FILE *stream, const bigInt x) {}
void __BIGINT_FPUTF__(FILE *stream, const bigInt x) {}
/* --------- Decimal Incremental OUTPUT ---------  */
void __BIGINT_SPUT__(const bigInt x) {}
void __BIGINT_SPUTB__(const bigInt x, uint8_t base) {}
void __BIGINT_SPUTF__(const bigInt x, uint8_t base) {}
void __BIGINT_SFPUT__(FILE *stream, const bigInt x) {}
void __BIGINT_SFPUTB__(FILE *stream, const bigInt x) {}
void __BIGINT_SFPUTF__(FILE *stream, const bigInt x) {}
/* --------- Decimal INPUT ---------  */
void __BIGINT_GET__(bigInt *x) {}
void __BIGINT_SGET__(bigInt *x) {}
void __BIGINT_TGET__(bigInt *x) {}
void __BIGINT_FGET__(FILE *stream, bigInt *x) {}
void __BIGINT_FSGET__(FILE *stream, bigInt *x) {}
void __BIGINT_FTGET__(FILE *stream, bigInt *x) {}




//todo ================================= 4. SERIALIZATION & DESERIALIZATION ============================== *//
/* --------- Binary INPUT/OUTPUT ---------  */
void __BIGINT_FWRITE__(FILE *stream, const bigInt x) {}
str_status __BIGINT_FREAD__(FILE *stream, bigInt *x) {}
str_status __BIGINT_FSREAD__(FILE *stream, bigInt *x) {}
str_status __BIGINT_FTREAD__(FILE *stream, bigInt *x) {}
/* --------- SERIALIZATION / DESERIALIZATION ---------  */
char* __BIGINT_SERIALIZE__(FILE *stream, const bigInt x) {}
bigInt __BIGINT_DESERIALIZE__(FILE *stream, const char* str, size_t len, str_status *err) {}




//todo ====================================== 5. GENERAL UTILITIES ===================================== *//
void __BIGINT_LIMB_DUMP__(const bigInt x) {}
void __BIGINT_HEX_DUMP__(const bigInt x) {}
void __BIGINT_BIN_DUMP__(const bigInt x) {} 
void __BIGINT_INFO__(const bigInt x) {}

