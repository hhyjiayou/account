//
// Created by gaotong1 on 2016/5/12.
//

#ifndef ACCOUNT_CLIENT_OAUTH_GOOGLE_H
#define ACCOUNT_CLIENT_OAUTH_GOOGLE_H

#include <string.h>
#include <string>
#include <map>
#include "account.pb.h"
#include "client_oauth_base.h"

using namespace std;
using namespace zhibo_account;

namespace client {

class GoogleClient: public BaseOauthClient {
public:

    GoogleClient() {
    }
    ~GoogleClient() {
    }

    /**
     * 根据access_token,openid调用oauth接口查询第三方用户资料
     */
    int GetOuterProfile(string access_token, OuterProfile& outer_profile);

};
}

#endif //ACCOUNT_CLIENT_OAUTH_GOOGLE_H
