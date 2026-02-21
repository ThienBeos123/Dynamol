#include "util.h"

uint8_t is_numeric(char c) { return (c <= '9' && c >= '0'); }
uint16_t _skip_whitespace__(FILE *stream) {
    uint16_t c;
    while ((c = fgetc(stream)) != EOF && isspace(c));
    return c;
}
uint8_t _is_valid_digit__(uint16_t *curr_char) { 
    return (*curr_char != EOF && !isspace(*curr_char)); 
}

uint8_t _sign_handle_(const char *str, size_t *curr_pos, uint8_t *sign) {
    *sign = 1;
    if (str[*curr_pos] == '-') { 
        *sign = -1; *curr_pos += 1; 
        // In this case, the string is "-\null"
        if (str[*curr_pos] == '\0') return 3;
    }
    else if (str[*curr_pos] == '+') {
        *curr_pos += 1;
        // In this case, the string is "+\null"
        if (str[*curr_pos] == '\0') return 3;
    }
    // This case forces the next character to be 0->9 for the prefix/a decimal
    else if (str[*curr_pos] && !is_numeric(str[*curr_pos])) return 4;
}

uint8_t _prefix_handle_(const char *str, size_t *curr_pos, uint8_t *base) {
    *base = 10;
    if (is_numeric(str[*curr_pos]) && str[*curr_pos] != '0') return 1; // A decimal (eg: 9...)
    else { // The string is currently "0..."
        if (str[*curr_pos + 1] == '\0') return 0; // The string currently is "0\null"
        else if (is_numeric(str[*curr_pos + 1])) { // The string currently is "0(numerical)" (eg: 0942)
            *curr_pos += 1; // A leading zero --> Decimal
            return 1;
        } else {
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
}

/* --------------------- */
/* --- Nlen variants --- */
/* --------------------- */
uint8_t _sign_handle_nlen_(const char *str, size_t *curr_pos, uint8_t *sign, size_t len) {
    *sign = 1;
    if (str[*curr_pos] == '-') { 
        *sign = -1; *curr_pos += 1; 
        // In this case, the string is "-\null" or ended as "-"
        if (str[*curr_pos] == '\0' || *curr_pos == len - 1) return 3;
    }
    else if (str[*curr_pos] == '+') {
        *curr_pos += 1;
        // In this case, the string is "+\null" or ended as "+"
        if (str[*curr_pos] == '\0'|| *curr_pos == len - 1) return 3;
    }
    // This case forces the next character to be 0->9 for the prefix/a decimal
    else if (str[*curr_pos] && !is_numeric(str[*curr_pos])) return 4;
}

uint8_t _prefix_handle_nlen_(const char *str, size_t *curr_pos, uint8_t *base, size_t len) {
    *base = 10;
    if (is_numeric(str[*curr_pos]) && str[*curr_pos] != '0') return 1; // A decimal (eg: 9...)
    else { // The string is currently "0..."
        if (str[*curr_pos + 1] == '\0') return 0; // The string currently is "0\null"
        else if (*curr_pos == len - 1) return 3; // The string ended as "0"
        else if (is_numeric(str[*curr_pos + 1])) { // The string currently is "0(numerical)" (eg: 0942)
            *curr_pos += 1; // A leading zero --> Decimal
            return 1;
        } else {
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
}

/* ----------------------------- */
/* --- Stream parse variants --- */
/* ----------------------------- */
uint8_t _prefix_handle_stream__(FILE* stream, uint8_t *base, uint16_t *curr_char) {
    *base = 10;
    if (is_numeric(*curr_char) && *curr_char != 0) return 1;  // A decimal (eg: 9...)
    else { // The string is currently "0..."
        *curr_char = fgetc(stream);
        if (*curr_char == EOF || isspace(*curr_char)) return 0; // The string currently is "0\null"
        else if (is_numeric(*curr_char)) { // The string currently is "0(numerical)" (eg: 0942)
            *curr_char = fgetc(stream); // A leading zero --> Decimal
            return 1;
        } else {
            switch (*curr_char) {
                // Hexadecimal
                case 'x':   *base = 16; *curr_char = fgetc(stream); break;
                case 'X':   *base = 16; *curr_char = fgetc(stream); break;
                // Binary
                case 'b':   *base = 2; *curr_char = fgetc(stream); break;
                case 'B':   *base = 2; *curr_char = fgetc(stream); break;
                // Octal
                case 'o':   *base = 8; *curr_char = fgetc(stream); break;
                case 'O':   *base = 8; *curr_char = fgetc(stream); break;
                //! INVALID BASE PREFIX
                default:    return 2; break;
            }
        }
    }
}