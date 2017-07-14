/*
 * client_oauth_qq.h
 *
 *  Created on: 2016年2月22日
 *      Author: zhangyouming
 */

#ifndef ACCOUNT_ACCOUNT_COMMON_CLIENT_CLIENT_OAUTH_QQ_H
#define ACCOUNT_ACCOUNT_COMMON_CLIENT_CLIENT_OAUTH_QQ_H
#include <string.h>
#include <string>
#include <map>
#include "account.pb.h"
#include "client_oauth_base.h"
#include "account_constant.h"

using namespace std;
using namespace zhibo_account;

namespace client {

class QqOauthClient: public BaseOauthClient {
public:

    QqOauthClient() {
    }
    ~QqOauthClient() {
    }

    /**
     * 通过调用第三方的oauth接口验证客户端输入的access_token和openid是否证实一致,并获取unionid
     * 对于骑士团账号，判断client_id是否和配置文件里面的第三方账号appid一致，若不一致，accesstoken解开也视为无效token
     */
    int VerifyAccessToken(string access_token, string openid, string& unionid, string &client_id);

    /**
     * 根据access_token,openid调用oauth接口查询第三方用户资料
     */
    int GetOuterProfile(uint32_t account_type, string access_token,
    		string openid, OuterProfile& outer_profile, string appid_local);

};
}
#endif
