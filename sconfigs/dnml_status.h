#ifndef __DNML_STAT_H__
#define __DNML_STAT_H__


typedef enum ___424947494E54_4552524F52___ {
    BIGINT_SUCCESS = 0,
    BIGINT_NULL,
    BIGINT_ERR_INVAL,
    BIGINT_ERR_RANGE,
    BIGINT_ERR_DOMAIN
} bigint_status;

typedef enum {
    STR_SUCCESS = 100,          // 100
    STR_NULL,                   // 101
    STR_EMPTY,                  // 102
    STR_INVALID_CAP,            // 103
    STR_INCOMPLETE,             // 104
    STR_INVALID_SIGN,           // 105
    STR_INVALID_BASE_PREFIX,    // 106
    STR_INVALID_BASE,           // 107
    STR_INVALID_DIGIT,          // 108
} str_status;


#endif