#include "util.h"

unsigned char _sign_handle_(const char *str, size_t *curr_pos, uint8_t *sign) {
    *sign = 1;
    if (str[*curr_pos] == '-') { *sign = -1; *curr_pos += 1; }
    else if (str[*curr_pos] == '+') *curr_pos += 1;
    else if (str[*curr_pos] && str[*curr_pos] != '0') return 4;
    if (str[*curr_pos] == '\0') return 3;
}

unsigned char _prefix_handle_(const char *str, size_t *curr_pos, uint8_t *base) {
    *base = 10;
    if (str[*curr_pos] == '0') return 0;
    else if (str[*curr_pos + 1] <= '9' && str[*curr_pos + 1] >= '0') {
        *curr_pos += 1;
        return 1;
    }
    else {
        switch (str[*curr_pos + 1]) {
            // Hexadecimal
            case 'x':   *base = 16; *curr_pos += 2; break;
            case 'X':   *base = 16; *curr_pos += 2; break;
            // Binary
            case 'b':   *base = 2; *curr_pos += 2; break;
            case 'B':   *base = 2; *curr_pos += 2; break;
            // Octal
            case 'o':   *base = 8; *curr_pos += 2; break;
            case 'O':   *base = 8; *curr_pos += 2; break;
            //! INVALID BASE PREFIX
            default:    return 2; break;
        }
    }
}