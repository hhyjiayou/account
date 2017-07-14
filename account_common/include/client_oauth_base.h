/*
 * client_oauth_base.h
 *
 *  Created on: 2016年2月22日
 *      Author: zhangyouming
 */

#ifndef ACCOUNT_ACCOUNT_COMMON_CLIENT_CLIENT_OAUTH_BASE_H
#define ACCOUNT_ACCOUNT_COMMON_CLIENT_CLIENT_OAUTH_BASE_H
#include <string.h>
#include <string>
#include <map>
#include "account.pb.h"
#include "hy_api.h"
#include "account_conf.h"
#include "qos_client.h"
#include "account_logic.h"

using namespace std;
using namespace zhibo_account;
using namespace hy_base;
using namespace account;

namespace client {

    struct CurlRespMemory {
        char* memPtr;
        size_t size;
        CurlRespMemory():memPtr(0), size(0){  //构造函数初始化
        }
    };

class BaseOauthClient {
public:

    BaseOauthClient() {
    }
    virtual ~BaseOauthClient() {
    }

    /**
     * curl-get封装
     * @url 纯URL，不带任何参数
     * @domain url对应的域名
     * @ipgroup 对于需要换ip重试的多少个IP
     * @params 所有携带参数
     * @response 请求响应的内容
     * @retry 超时后是否要重试
     */
    int SendGetRequest(ThirdApiInfo third_api_info, map<string, string> params, string& response);

    /**
    * curl-post封装
    * @url 纯URL，不带任何参数
    * @domain url对应的域名
    * @ipgroup 对于需要换ip重试的多少个IP
    * @params 所有携带参数
    * @response 请求响应的内容
    * @retry 超时后是否要重试
    */
    int SendPostRequest(ThirdApiInfo third_api_info, const string& post_fields, string& response);

    /**
     * curl-get封装
     * @url 纯URL，不带任何参数
     * @domain url对应的域名
     * @ip 指定IP访问对应的ip
     * @params 所有携带参数
     * @connecttimeout 建立连接的timeout时间，单位毫秒
     * @timeout 连接建立好后读写timeout，单位毫秒
     * @response 请求响应的内容
     * @errmsg 出错信息
     */
    int SendGetRequest(ThirdApiInfo third_api_info, map<string, string> params, string& response,string& errmsg);


    /**
    * curl-get封装
    * @url 纯URL，不带任何参数
    * @domain url对应的域名
    * @ip 指定IP访问对应的ip
    * @params 所有携带参数
    * @connecttimeout 建立连接的timeout时间，单位毫秒
    * @timeout 连接建立好后读写timeout，单位毫秒
    * @response 请求响应的内容
    * @errmsg 出错信息
    */
    int SendPostRequest(ThirdApiInfo& third_api_info, const string& post_fields, string& response, string& errmsg);

    /**
     * @ipgroupstr 格式为:163.177.71.162,163.177.92.16;163.177.71.162,163.177.92.16
     * @ipgroup 被选出的ip
     * 从ipgroupstr随机选出两个ip放入ipgroup集合
     */
    int GenerateRandIpGroup(string ipgroupstr,vector<string>& ipgroup);
    int GenerateRandIpGroupByL5(QOSREQUEST& qos);

    /**
     * 处理https回报数据
     */
    static size_t GetRespCallback(void *respDataPtr, size_t size, size_t nmemb, void *localMemPtr);
    static size_t GetHeadInfoCallback(char *respDataPtr, size_t size, size_t nmemb, void *localMemPtr);

};
}
#endif
