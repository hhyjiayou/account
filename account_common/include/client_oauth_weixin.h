/*
 * client_oauth_weixin.h
 *
 *  Created on: 2016年2月22日
 *      Author: zhangyouming
 */
#ifndef ACCOUNT_ACCOUNT_COMMON_CLIENT_CLIENT_OAUTH_WEIXIN_H
#define ACCOUNT_ACCOUNT_COMMON_CLIENT_CLIENT_OAUTH_WEIXIN_H
#include <string.h>
#include <string>
#include <map>
#include "account.pb.h"
#include "client_oauth_base.h"

using namespace std;
using namespace zhibo_account;

namespace client {

class WeixinOauthClient: public BaseOauthClient {
public:

    WeixinOauthClient() {
    }
    ~WeixinOauthClient() {
    }
    /**
     * 查询第三方平台的openid、access_token相关信息
     * @url 纯URL，不带任何参数
     * @params 所有携带参数
     * @open_account_info 查询到的第三方平台的openid、access_token相关信息
     */
    int GetAccessToken(string url, map<string, string> params, OpenAccountInfo& open_account_info);

    /**
     * 查询第三方平台的用户资料相关信息
     * @url 纯URL，不带任何参数
     * @params 所有携带参数
     * @open_account_info 查询到的第三方平台的用户资料相关信息
     */
    int GetOuterProfile(string url, map<string, string> params, OuterProfile& outer_profile);

    /**
     * 根据临时票据查询第三方平台的openid、access_token相关信息
     */
    int GetAccessToken(string code, OpenAccountInfo& open_account_info,
    		string appid_local, const unsigned& account_type = 1);

    /**
     * 根据access_token和openid查询第三方平台的用户资料信息
     */
    int GetOuterProfile(string access_token, string openid, OuterProfile& outer_profile);

};
}
#endif
