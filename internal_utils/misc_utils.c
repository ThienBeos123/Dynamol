#include "util.h"

uint8_t __COUNT_DIGIT_BASE__() {}
uint64_t __MAG_I64__(int64_t val) {
    return (val == INT64_MIN) ?
        (uint64_t)(llabs(val + 1)) + 1 :
        (uint64_t)(llabs(val));
}