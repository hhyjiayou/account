/*
 * client_oauth_weixin.cc
 *
 *  Created on: 2016年2月22日
 *      Author: zhangyouming
 */
#include <map>
#include "json/json.h"
#include "client_oauth_weixin.h"
#include "account_conf.h"
#include "string_util.h"
#include "time_util.h"
#include "account_log.h"
#include "account_error.h"

using namespace hy_base;
using namespace account;

namespace client {

int WeixinOauthClient::GetAccessToken(string url, map<string, string> params, OpenAccountInfo& open_account_info) {
    ThirdApiInfo third_api_info;
    third_api_info.m_url = url;
    third_api_info.m_domain = Config::GetInstance()->GetGlobalConfig()["oauth_weixin.domain"];
    third_api_info.m_qos._modid = Config::GetInstance()->GetGlobalConfig().GetInt("oauth_weixin.l5_id");
    third_api_info.m_qos._cmd = Config::GetInstance()->GetGlobalConfig().GetInt("oauth_weixin.cmd_id");
    third_api_info.m_retry = (Config::GetInstance()->GetGlobalConfig().GetStr("oauth_weixin.token_retry", "false") == "true");

    string response;
    int ret = SendGetRequest(third_api_info, params, response);
    DEBUG("GetAccessToken-response:%s", response.c_str());
    if (ret == 0) {
        Json::Reader reader;
        Json::Value root;
        if (!reader.parse(response, root, false)) {
            ERROR("GetAccessToken-parse-response-error:%s", response.c_str());
            return AccountError::LOGIN_VERIFY_OAUTH_ACCESSTOKEN_ERROR;
        }
        ret = root["errcode"].asInt();
        if (ret != 0) {
            ERROR("GetAccessToken-response-error-ret:%d", ret);
            return ret;
        }
        open_account_info.set_access_token(root["access_token"].asString().c_str());
        //open_account_info.set_refresh_token(root["refresh_token"].asString().c_str());  //refresh_token和expires_in未用到，清空
        open_account_info.set_refresh_token("");
        open_account_info.set_openid(root["openid"].asString().c_str());
        open_account_info.set_expires_in(root["expires_in"].asInt());
        //open_account_info.set_expires_in(0);
        open_account_info.set_token_update_ts(TimeUtil::GetCurrentTime() / 1000);
        open_account_info.set_unionid(root["unionid"].asString().c_str());
        return 0;
    } else {
        ERROR("SendGetRequest-error-ret:%d", ret);
        return AccountError::LOGIN_VERIFY_OAUTH_ACCESSTOKEN_ERROR;
    }
}

int WeixinOauthClient::GetOuterProfile(string url, map<string, string> params, OuterProfile& outer_profile) {
	ThirdApiInfo third_api_info;
	third_api_info.m_url = url;
	third_api_info.m_domain = Config::GetInstance()->GetGlobalConfig()["oauth_weixin.domain"];
	third_api_info.m_qos._modid = Config::GetInstance()->GetGlobalConfig().GetInt("oauth_weixin.l5_id");
	third_api_info.m_qos._cmd = Config::GetInstance()->GetGlobalConfig().GetInt("oauth_weixin.cmd_id");
	third_api_info.m_retry = true;

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
        ret = root["errcode"].asInt();
        if (ret != 0) {
            ERROR("GetOuterProfile-response-error-ret:%d", ret);
            return ret;
        }
        outer_profile.set_uid(root["openid"].asString());
        outer_profile.set_nickname(root["nickname"].asString().c_str());
        outer_profile.set_headimgurl(root["headimgurl"].asString().c_str());
        outer_profile.set_sex(root["sex"].asInt());
        return 0;
    } else {
        ERROR("SendGetRequest-error-ret:%d", ret);
        return AccountError::LOGIN_GET_OAUTH_PROFILE_ERROR;
    }
}

int WeixinOauthClient::GetAccessToken(string code, OpenAccountInfo& open_account_info,
		string appid_local, const unsigned& account_type) {
    map<string, string> params;
    params["code"] = code;
    string appid;
    string secret_key;
    if(appid_local.empty()){
    	appid_local = "10007";
    }
    string appid_conf_key = "oauth_weixin_" + appid_local;

    if(account_type == 101){ //h5_wx_appid
        appid = Config::GetInstance()->GetGlobalConfig()[(appid_conf_key  + ".h5_appid").c_str()];
        secret_key = Config::GetInstance()->GetGlobalConfig()[(appid_conf_key  + ".h5_secret_key").c_str()];
    }else if(account_type == 102){  //DB
        appid = Config::GetInstance()->GetGlobalConfig()[(appid_conf_key  + ".db_appid").c_str()];
        secret_key = Config::GetInstance()->GetGlobalConfig()[(appid_conf_key  + ".db_secret_key").c_str()];
    }else if(account_type == 103){  //debug
        appid = Config::GetInstance()->GetGlobalConfig()[(appid_conf_key  + ".debug_appid").c_str()];
        secret_key = Config::GetInstance()->GetGlobalConfig()[(appid_conf_key  + ".debug_secret_key").c_str()];
    }else if(account_type == 104){  //黑金
        appid = Config::GetInstance()->GetGlobalConfig()[(appid_conf_key  + ".blackgold_appid").c_str()];
        secret_key = Config::GetInstance()->GetGlobalConfig()[(appid_conf_key  + ".blackgold_secret_key").c_str()];
    }else if(account_type == 105){  //vans_game
        appid = Config::GetInstance()->GetGlobalConfig()[(appid_conf_key  + ".vansgame_appid").c_str()];
        secret_key = Config::GetInstance()->GetGlobalConfig()[(appid_conf_key  + ".vansgame_secret_key").c_str()];
    } else{ //默认1
        appid = Config::GetInstance()->GetGlobalConfig()[(appid_conf_key  + ".appid").c_str()];
        secret_key = Config::GetInstance()->GetGlobalConfig()[(appid_conf_key  + ".secret").c_str()];
    }

    if(appid.empty() || secret_key.empty()){
    	ERROR("empty appid:[%s], or sercret_key:[%s], appid_local:[%s]", appid.c_str(), secret_key.c_str(), appid_local.c_str());
    	return AccountError::LOGIN_VERIFY_OAUTH_ACCESSTOKEN_ERROR;
    }
    params["appid"] = appid;
    params["secret"] = secret_key;
    params["grant_type"] = Config::GetInstance()->GetGlobalConfig()["oauth_weixin.grant_type"];
    return GetAccessToken(StringUtil::UrlDecode(Config::GetInstance()->GetGlobalConfig()["oauth_weixin.access_token_url"]), params,
            open_account_info);
}

int WeixinOauthClient::GetOuterProfile(string access_token, string openid, OuterProfile& outer_profile) {
    map<string, string> params;
    params["access_token"] = access_token;
    params["openid"] = openid;
    return GetOuterProfile(StringUtil::UrlDecode(Config::GetInstance()->GetGlobalConfig()["oauth_weixin.profile_url"]), params,
            outer_profile);
}

}
