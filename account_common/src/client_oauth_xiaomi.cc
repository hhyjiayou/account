/*
 * client_oauth_xiaomi.cc
 *
 *  Created on: 2016年2月22日
 *      Author: zhangyouming
 */
#include <map>
#include "json/json.h"
#include "client_oauth_xiaomi.h"
#include "account_conf.h"
#include "string_util.h"
#include "account_log.h"
#include "time_util.h"
#include "account_error.h"

using namespace hy_base;
using namespace account;

namespace client {

int XiaomiOauthClient::GetAccessToken(string url, map<string, string> params, OpenAccountInfo& open_account_info) {
	ThirdApiInfo third_api_info;
	third_api_info.m_url = url;
	third_api_info.m_domain = Config::GetInstance()->GetGlobalConfig()["oauth_xiaomi.token_domain"];
	third_api_info.m_qos._modid = Config::GetInstance()->GetGlobalConfig().GetInt("oauth_xiaomi.l5_id_token");
	third_api_info.m_qos._cmd = Config::GetInstance()->GetGlobalConfig().GetInt("oauth_xiaomi.cmd_id_token");
	third_api_info.m_retry = (Config::GetInstance()->GetGlobalConfig().GetStr("oauth_xiaomi.token_retry", "false") == "true");

    string response;
    int ret = SendGetRequest(third_api_info, params, response);
    DEBUG("GetAccessToken-response:%s", response.c_str());
    if (ret == 0) {
        //小米oauth服务返回值是这个格式：&&&START&&&{"error":96002,"error_description":"缺少某个必需参数，或者格式不正确"}
        if (response.length() > 11) {
            response = response.substr(11, response.length() - 11);
        }
        Json::Reader reader;
        Json::Value root;
        if (!reader.parse(response, root, false)) {
            ERROR("GetAccessToken-parse-response-error:%s", response.c_str());
            return AccountError::LOGIN_VERIFY_OAUTH_ACCESSTOKEN_ERROR;
        }
        ret = root["error"].asInt();
        if (ret != 0) {
            ERROR("GetAccessToken-response-error-ret:%d", ret);
            return ret;
        }
        open_account_info.set_access_token(root["access_token"].asString().c_str());
        open_account_info.set_refresh_token(root["refresh_token"].asString().c_str());
        open_account_info.set_openid(root["openId"].asString().c_str());
        open_account_info.set_expires_in(root["expires_in"].asInt());
        open_account_info.set_token_update_ts(TimeUtil::GetCurrentTime() / 1000);
        return 0;
    } else {
        ERROR("SendGetRequest-error-ret:%d", ret);
        return AccountError::LOGIN_VERIFY_OAUTH_ACCESSTOKEN_ERROR;
    }
}

int XiaomiOauthClient::GetOuterProfile(string url, map<string, string> params, OuterProfile& outer_profile) {
	ThirdApiInfo third_api_info;
	third_api_info.m_url = url;
	third_api_info.m_domain = Config::GetInstance()->GetGlobalConfig()["oauth_xiaomi.profile_domain"];
	third_api_info.m_qos._modid = Config::GetInstance()->GetGlobalConfig().GetInt("oauth_xiaomi.l5_id_profile");
	third_api_info.m_qos._cmd = Config::GetInstance()->GetGlobalConfig().GetInt("oauth_xiaomi.cmd_id_profile");

    string response;
    int ret = SendGetRequest(third_api_info, params, response);
    DEBUG("GetOuterProfile-response:%s", response.c_str());
    if (ret == 0) {
        Json::Reader reader;
        Json::Value root;
        if (!reader.parse(response, root, false)) {
            ERROR("GetOuterProfile-parse-response-error:%s", response.c_str());
            return AccountError::LOGIN_GET_OAUTH_PROFILE_ERROR;
        }
        ret = root["code"].asInt();
        if (ret != 0) {
            ERROR("GetOuterProfile-response-error-ret:%d", ret);
            return ret;
        }
        Json::Value data = root["data"];
        outer_profile.set_nickname(data["miliaoNick"].asString().c_str());
        outer_profile.set_headimgurl(data["miliaoIcon_orig"].asString().c_str());
        outer_profile.set_sex(data["sex"].asInt());
        outer_profile.set_uid(data["userId"].asString().c_str());
        return 0;
    } else {
        ERROR("SendGetRequest-error-ret:%d", ret);
        return AccountError::LOGIN_GET_OAUTH_PROFILE_ERROR;
    }
}

int XiaomiOauthClient::GetAccessToken(string code, OpenAccountInfo& open_account_info, string appid_local) {
    map<string, string> params;
    string appid_conf_key = "oauth_xiaomi_" + appid_local;
    params["code"] = code;
    params["client_id"] = Config::GetInstance()->GetGlobalConfig()[(appid_conf_key + ".client_id").c_str()];
    params["redirect_uri"] = StringUtil::UrlDecode(Config::GetInstance()->GetGlobalConfig()[(appid_conf_key + ".redirect_uri").c_str()]);
    params["client_secret"] = StringUtil::UrlDecode(Config::GetInstance()->GetGlobalConfig()[(appid_conf_key + ".client_secret").c_str()]);
    params["grant_type"] = Config::GetInstance()->GetGlobalConfig()["oauth_xiaomi.grant_type"];
    return GetAccessToken(StringUtil::UrlDecode(Config::GetInstance()->GetGlobalConfig()["oauth_xiaomi.access_token_url"]), params,
            open_account_info);
}

int XiaomiOauthClient::GetOuterProfile(string access_token, OuterProfile& outer_profile, string appid_local) {
	string appid_conf_key = "oauth_xiaomi_" + appid_local;
    map<string, string> params;
    params["clientId"] = Config::GetInstance()->GetGlobalConfig()[(appid_conf_key + ".client_id").c_str()];
    params["token"] = access_token;
    return GetOuterProfile(StringUtil::UrlDecode(Config::GetInstance()->GetGlobalConfig()["oauth_xiaomi.profile_url"]), params,
            outer_profile);
}
}
