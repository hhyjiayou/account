//
// Created by songxiangyu on 2016/10/10.
//

#ifndef ACCOUNT_CLIENT_INSTAGRAM_H
#define ACCOUNT_CLIENT_INSTAGRAM_H

#include <string.h>
#include <string>
#include <map>
#include "account.pb.h"
#include "client_oauth_base.h"

using namespace std;
using namespace zhibo_account;

namespace client {

    class InstagramOauthClient: public BaseOauthClient {
        public:

        InstagramOauthClient() {
        }
        ~InstagramOauthClient() {
        }
        /*
         * @func code exchange access_token
         * */
        int GetAccessTokenAndBaseInfo(const string& code, OpenAccountInfo& open_account_info,
        		OuterProfile& outer_profile, string appid_local);
    };
}

#endif //ACCOUNT_CLIENT_INSTAGRAM_H
