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
    STR_SUCCESS = 100,
    STR_NULL,
    STR_EMPTY,
    STR_INCOMPLETE,
    STR_INVALID_SIGN,
    STR_INVALID_BASE_PREFIX,
    STR_INVALID_BASE,
    STR_INVALID_DIGIT,
} str_status;


#endif