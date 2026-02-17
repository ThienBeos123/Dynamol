#ifndef __DNML_STAT_H__
#define __DNML_STAT_H__


typedef enum ___44594E414D4F4C_737461747573___ {
    BIGINT_SUCCESS = 0,         // 0
    BIGINT_NULL,                // 1
    BIGINT_ERR_INVAL,           // 2
    BIGINT_ERR_RANGE,           // 3
    BIGINT_ERR_DOMAIN,          // 4

    STR_SUCCESS = 100,          // 100
    STR_NULL,                   // 101
    STR_EMPTY,                  // 102
    STR_INVALID_CAP,            // 103
    STR_INCOMPLETE,             // 104
    STR_INVALID_SIGN,           // 105
    STR_INVALID_BASE_PREFIX,    // 106
    STR_INVALID_BASE,           // 107
    STR_INVALID_DIGIT,          // 108
} dnml_status;


#endif