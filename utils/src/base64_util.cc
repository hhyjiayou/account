#include "base64_util.h"
#include <cassert>
#include <limits>
#include <stdexcept>
#include <cctype>
#include <stdio.h>

char Base64Util::_b64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char Base64Util::_reverse_table[128] = { 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
        64, 64, 64, 64, 64, 64, 64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64,
        64, 64, 64, 64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64,
        64, 64 };

string Base64Util::base64_encode(const string& bindata) {
    using ::std::numeric_limits;

    if (bindata.size() > (numeric_limits<string::size_type>::max() / 4u) * 3u) {
        //throw ::std::length_error("Converting too large a string to base64.");
        //printf("onverting too large a string to base64.\n");
        return "";
    }

    const ::std::size_t binlen = bindata.size();

    string retval((((binlen + 2) / 3) * 4), '=');
    ::std::size_t outpos = 0;
    int bits_collected = 0;
    unsigned int accumulator = 0;

    for (string::const_iterator i = bindata.begin(); i != bindata.end(); ++i) {
        accumulator = (accumulator << 8) | (*i & 0xffu);
        bits_collected += 8;
        while (bits_collected >= 6) {
            bits_collected -= 6;
            retval[outpos++] = _b64_table[(accumulator >> bits_collected) & 0x3fu];
        }
    }
    if (bits_collected > 0) { // Any trailing bits that are missing.
        assert(bits_collected < 6);
        accumulator <<= 6 - bits_collected;
        retval[outpos++] = _b64_table[accumulator & 0x3fu];
    }
    assert(outpos >= (retval.size() - 2));
    assert(outpos <= retval.size());
    return retval;
}

string Base64Util::base64_decode(const string& ascdata) {
    string retval;
    int bits_collected = 0;
    unsigned int accumulator = 0;

    for (string::const_iterator i = ascdata.begin(); i != ascdata.end(); ++i) {
        const int c = *i;
        if (std::isspace(c) || c == '=') {
            continue;
        }
        if ((c > 127) || (c < 0) || (_reverse_table[c] > 63)) {
            //throw std::invalid_argument("This contains characters not legal in a base64 encoded string.");
            //printf(("This contains characters not legal in a base64 encoded string."););
            return "";
        }
        accumulator = (accumulator << 6) | _reverse_table[c];
        bits_collected += 6;
        if (bits_collected >= 8) {
            bits_collected -= 8;
            retval += (char) ((accumulator >> bits_collected) & 0xffu);
        }
    }
    return retval;
}

/*
 int main()
 {
 string str = "1234567abcdefg";
 string base64_str = Base64Util::base64_encode(str);
 cout << base64_str << endl;
 string decode_base64_str = Base64Util::base64_decode(base64_str);
 cout << decode_base64_str << endl;

 return 0;
 }
 */
