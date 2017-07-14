/*
 * comm_util.h
 *
 *  Created on: 2016年2月17日
 *      Author: zhongxiankui
 */

#ifndef UTILS_INCLUDE_COMM_UTIL_H_
#define UTILS_INCLUDE_COMM_UTIL_H_

#define __STDC_FORMAT_MACROS 1
#include <inttypes.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "openssl/md5.h"
#include "account_error.h"
#include "zhibo_qua_util.h"
using namespace std;
//hy_api返回码定义
#define HY_API_RECV_TIMEOUT 3313
#define HY_API_SEND_TIMEOUT 3314

//L5返回码定义
#define MLB_OVERLOAD 3401
#define MLB_UNKONW_ERROR 3403

namespace account {

class Util {
public:
    static string uint64ToString(uint64_t val);
    static string getCurrentDateStr(string format);
    static string getCurrentDateStr();
    static uint64_t getCurrentTimeMillis();
    static bool is_all_digit(const string & str);
    static string long_to_string(long long x);
    static string int_to_string(int x);
    static string double_to_string(double x);
    static uint64_t string_to_ull(const string& x);
    //注意：当字符串为空时，也会返回一个空字符串
    static void splitString(string & s, string & delim, vector<string>* ret);

    static int ip_s2u(const string& s_ip, unsigned& u_ip);
    static string ip_u2s(unsigned u_ip);
    static void getCurrentTimeAsHuman(string& seperator, string& time);
    static void getBeforeTimeAsHuman(string& seperator, string& timeStr, int beforeDays);
    static void removeSpecificChHeadTail(string& chr, string& stringin, string& stringout);

    //替换字符
    static void replace_all(string& strin, const string& old_value,const string& new_value);

    /*
     * 判断字符串是否全由大写组成
     * */
    static string strToUpper(const string& passwd);

    /**
     * func distinguish device language by qua
     * return value: 1-zh_cn  2-zh_hant 3-en
     **/
     static int distinguishLangByQua(const string& qua);
};
}

#endif /* UTILS_INCLUDE_COMM_UTIL_H_ */
