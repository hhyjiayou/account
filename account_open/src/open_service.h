//
// Created by gaotong1 on 2016/5/10.
//

#ifndef ACCOUNT_OPEN_SERVICE_H
#define ACCOUNT_OPEN_SERVICE_H

#include <client_oauth_google.h>
#include <client_oauth_facebook.h>
#include "account_open.pb.h"
#include "account_error.h"
#include "hy_logger.h"
#include "account.pb.h"
#include "client_oauth_weixin.h"
#include "client_oauth_qq.h"
#include "client_oauth_weibo.h"
#include "client_oauth_xiaomi.h"
#include "client_oauth_facebook.h"
#include "account_constant.h"
#include "client_oauth_google.h"
#include "client_oauth_instagram.h"

using namespace client;
using namespace hy_base;
using namespace zhibo_open;
using namespace zhibo_account;

class AccountOpenService
{

public:
    int VerifyAccessToken(VerifyAccessTokenReq& req, VerifyAccessTokenRsp& rsp);


    int GetAccessTokenProcess(GetAccessTokenByCodeReq& req, OpenAccountInfo& openAccountInfo);

    int GetAccessTokenProcess(GetAccessTokenAndBaseInfoByCodeReq& req, OpenAccountInfo& openAccountInfo, OuterProfile& outer_profile);

    int GetOpenProfile(GetOpenProfileReq& req, OuterProfile& outerProfile);

    static AccountOpenService* getInstance()
    {
        static AccountOpenService instance;
        return &instance;
    }

private:
    AccountOpenService()
    {
    }

    WeixinOauthClient weixin_client;
    QqOauthClient qq_client;
    WeiboOauthClient weibo_sina_client;
    XiaomiOauthClient xiaomi_client;
    client::FackebookClient fackebookClient;
    GoogleClient googleClient;
    InstagramOauthClient instagramClient;
    int filterErrorCode(int accountType, int ret);
};

#endif //ACCOUNT_OPEN_SERVICE_H
