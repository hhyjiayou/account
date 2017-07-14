/*
 * comm_util.cc
 *
 *  Created on: 2016年2月17日
 *      Author: zhongxiankui
 */
#include "comm_util.h"

#include "limits.h"
#include "hy_logger.h"

#include <sstream>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <string_util.h>
#include "boost/regex.hpp"
#include <string>
#include "account_conf.h"

using namespace std;
using namespace hy_base;
using namespace account;
using namespace boost;
using namespace zhibo::live;
string Util::uint64ToString(uint64_t val) {
    char buf[64];
    snprintf(buf, sizeof buf, "%" PRIu64, val);
    string str(buf);
    return str;
}

string Util::getCurrentDateStr(string format) {
    char buf[80];
    time_t now = time(NULL);
    strftime(buf, 80, format.c_str(), localtime(&now));
    string str(buf);
    return str;
}

string Util::getCurrentDateStr() {
    return getCurrentDateStr("%Y%m%d");
}

uint64_t Util::getCurrentTimeMillis() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

bool Util::is_all_digit(const string & str)
{
   int i ;
   for(i = 0; i != str.length(); i++)
   {
       if(!isdigit(str[i]))
       {
           return false;
       }
   }
   return true;
}

string Util::long_to_string(long long x)
{
    stringstream strstream;
    strstream << x;
    return strstream.str();
}

string Util::int_to_string(int x)
{
    stringstream strstream;
    strstream << x;
    return strstream.str();
}

string Util::double_to_string(double x)
{
    stringstream strstream;
    strstream << x;
    return strstream.str();
}

uint64_t Util::string_to_ull(const string& x)
{
    if(x.empty())
    {
        return 0;
    }
    stringstream strstream;
    strstream.str(x);
    uint64_t  value;
    strstream >> value;
    return value;
}

//注意：当字符串为空时，也会返回一个空字符串
void Util::splitString(string& s, string& delim,vector< string >* ret)
{
    size_t last = 0;
    size_t index=s.find_first_of(delim,last);
    while (index!=std::string::npos)
    {
        ret->push_back(s.substr(last,index-last));
        last=index+1;
        index=s.find_first_of(delim,last);
    }
    if (index-last>0)
    {
        ret->push_back(s.substr(last,index-last));
    }
}

int Util::ip_s2u(const string& s_ip, unsigned& u_ip)
{
    sockaddr_in sa;
    int ret = inet_pton(AF_INET, s_ip.c_str(), &sa.sin_addr);
    if (ret <= 0)
    {
        return -1;
    }
    else
    {
        u_ip = (unsigned) sa.sin_addr.s_addr;
        return 0;
    }
}

string Util::ip_u2s(unsigned u_ip)
{
    char buf[INET_ADDRSTRLEN] = { 0 };
    sockaddr_in sa;
    sa.sin_addr.s_addr = u_ip;

    const char* p = inet_ntop(AF_INET, &sa.sin_addr, buf, sizeof(buf));
    return p ? p : string();
}

void Util::getCurrentTimeAsHuman(string& seperator, string& timeStr)
{
    time_t timep;
    struct tm *p;
    time(&timep); /*获得time_t结构的时间，UTC时间*/
    p = gmtime(&timep); /*转换为struct tm结构的UTC时间*/
    timeStr = int_to_string(1900 + p->tm_year) + seperator + int_to_string(1 + p->tm_mon) + seperator + int_to_string(p->tm_mday);
}

void Util::getBeforeTimeAsHuman(string& seperator, string& timeStr, int beforeDays)
{
    time_t t, tt;
    struct tm *p;
    tt = time(NULL);
    t = tt - beforeDays * 24 * 3600;
    p = localtime(&t);
    timeStr = int_to_string(1900 + p->tm_year) + seperator + int_to_string(1 + p->tm_mon) + seperator + int_to_string(p->tm_mday);
}

void Util::removeSpecificChHeadTail(string& chr, string& stringin, string& stringout)
{
    stringout = stringin;
    if(stringin.empty())
    {
        return;
    }
    if(-1 == stringout.find_first_not_of(chr))
    {
        stringout = "";
        return;
    }
    stringout.erase(0, stringout.find_first_not_of(chr));
    stringout.erase(stringout.find_last_not_of(chr) + 1);
}

void Util::replace_all(string& strin, const string& old_value,const string& new_value)
{
    while(true)
    {
        int pos=0;
        if((pos=strin.find(old_value,0))!=string::npos)
            strin.replace(pos,old_value.length(),new_value);
        else break;
    }
}

/*
 * 小写转大写
 * */
string Util::strToUpper(const string& passwd){
    string upPasswd = passwd;
    for (size_t  index = 0; index < upPasswd.size(); index++) {
        if (upPasswd[index] >= 'a' && upPasswd[index] <= 'z'){
            upPasswd[index] -= 32;
        }
    }
    return upPasswd;
}

int Util::distinguishLangByQua(const string& qua){
    int lang = 1;  //默认中文简体
    if(qua.size()){
        vector<string> qua_vec = StringUtil::Split(qua, "-");
        if(qua_vec.size() >= 6){
            string qua_lang = qua_vec[5];
            for (int index = 0; index < qua_lang.size(); ++index) { //转小写
                qua_lang[index] = static_cast<char>(tolower(qua_lang[index]));
            }
            NORMAL("qua_lang[ %s ]", qua_lang.c_str());
            if(qua_lang.find("zh_cn") != string::npos || qua_lang.find("zh_hans") != string::npos){
                lang = 1;  //简体中文标识
            }else if(qua_lang.find("zh_hant") != string::npos || qua_lang.find("zh_tw") != string::npos || qua_lang.find("zh_hk") != string::npos){
                lang = 2; //繁体中文标识
            }else{
                lang = 3; //其他语言都认为是英文
            }
        }
    }
    return lang;
}
