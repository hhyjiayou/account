#ifndef BASE64_UTIL_H
#define BASE64_UTIL_H

#include <string>

using namespace std;

class Base64Util
{
public:
    static string base64_encode(const string &bindata);
    static string base64_decode(const string &ascdata);
    static void binaryformat(unsigned char *buff, int len);

private:
    static char _b64_table[65];
    static char _reverse_table[128];
};

#endif
