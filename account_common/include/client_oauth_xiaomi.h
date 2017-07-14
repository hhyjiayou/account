/*
 * client_oauth_xioami.h
 *
 *  Created on: 2016年2月22日
 *      Author: zhangyouming
 */
#ifndef ACCOUNT_ACCOUNT_COMMON_CLIENT_CLIENT_OAUTH_XIAOMI_H
#define ACCOUNT_ACCOUNT_COMMON_CLIENT_CLIENT_OAUTH_XIAOMI_H
#include <string.h>
#include <string>
#include <map>
#include "account.pb.h"
#include "client_oauth_base.h"

using namespace std;
using namespace zhibo_account;

namespace client {

class XiaomiOauthClient: public BaseOauthClient {
public:

    XiaomiOauthClient() {
    }
    ~XiaomiOauthClient() {
    }
    int GetAccessToken(string url, map<string, string> params, OpenAccountInfo& open_account_info);

    int GetOuterProfile(string url, map<string, string> params, OuterProfile& outer_profile);

    int GetAccessToken(string code, OpenAccountInfo& open_account_info, string appid_local);

    int GetOuterProfile(string access_token, OuterProfile& outer_profile, string appid_local);

};
}
#endif
