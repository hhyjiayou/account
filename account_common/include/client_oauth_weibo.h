/*
 * client_oauth_weibo.h
 *
 *  Created on: 2016年2月22日
 *      Author: zhangyouming
 */
#ifndef ACCOUNT_ACCOUNT_COMMON_CLIENT_CLIENT_OAUTH_WEIBO_H
#define ACCOUNT_ACCOUNT_COMMON_CLIENT_CLIENT_OAUTH_WEIBO_H
#include <string.h>
#include <string>
#include <map>
#include "account.pb.h"
#include "client_oauth_base.h"

using namespace std;
using namespace zhibo_account;

namespace client {

class WeiboOauthClient: public BaseOauthClient {
public:

    WeiboOauthClient() {
    }
    ~WeiboOauthClient() {
    }

    /**
     * 通过调用第三方的oauth接口验证客户端输入的access_token和openid是否证实一致
     */
    int VerifyAccessToken(string access_token, string openid);

    /**
     * 根据access_token,openid调用oauth接口查询第三方用户资料
     */
    int GetOuterProfile(string access_token, string openid, OuterProfile& outer_profile);

};
}
#endif
