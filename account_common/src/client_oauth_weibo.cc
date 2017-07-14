/*
 * client_oauth_weibo.cc
 *
 *  Created on: 2016年2月22日
 *      Author: zhangyouming
 */
#include "json/json.h"
#include "client_oauth_weibo.h"
#include "string_util.h"
#include "account_conf.h"
#include "account_log.h"
#include "account_error.h"

using namespace hy_base;
using namespace account;

namespace client {

int WeiboOauthClient::VerifyAccessToken(string access_token, string openid) {
	ThirdApiInfo third_api_info;
	third_api_info.m_url = StringUtil::UrlDecode(Config::GetInstance()->GetGlobalConfig()["oauth_weibo.access_token_verify_url"]);
	third_api_info.m_domain = Config::GetInstance()->GetGlobalConfig()["oauth_weibo.domain"];
	third_api_info.m_qos._modid = Config::GetInstance()->GetGlobalConfig().GetInt("oauth_weibo.l5_id");
	third_api_info.m_qos._cmd = Config::GetInstance()->GetGlobalConfig().GetInt("oauth_weibo.cmd_id");
	third_api_info.m_retry = true;

    map<string, string> params;
    params["access_token"] = access_token;
    string response;

    int ret = SendGetRequest(third_api_info, params, response);
    DEBUG("VerifyAccessToken-response:%s", response.c_str());
    if (ret == 0) {
        Json::Reader reader;
        Json::Value root;
        if (!reader.parse(response, root, false)) {
            ERROR("VerifyAccessToken-parse-response-error:%s", response.c_str());
            return AccountError::LOGIN_VERIFY_OAUTH_ACCESSTOKEN_ERROR;
        }
        ret = root["error_code"].asInt();
        if (ret != 0) {
            ERROR("VerifyAccessToken-response-error-ret:%d", ret);
            return ret;
        }
        if (openid == root["uid"].asString()) {
            return 0;
        } else {
            ERROR("VerifyAccessToken-openid-error,input-openid:%s,get-openid:%s", openid.c_str(), root["openid"].asString().c_str());
            return AccountError::LOGIN_VERIFY_OAUTH_ACCESSTOKEN_ERROR;
        }
    } else {
        ERROR("SendGetRequest-error-ret:%d", ret);
        return AccountError::LOGIN_VERIFY_OAUTH_ACCESSTOKEN_ERROR;
    }
}

int WeiboOauthClient::GetOuterProfile(string access_token, string openid, OuterProfile& outer_profile) {
	ThirdApiInfo third_api_info;
	third_api_info.m_url = StringUtil::UrlDecode(Config::GetInstance()->GetGlobalConfig()["oauth_weibo.profile_url"]);
	third_api_info.m_domain = Config::GetInstance()->GetGlobalConfig()["oauth_weibo.domain"];
	third_api_info.m_qos._modid = Config::GetInstance()->GetGlobalConfig().GetInt("oauth_weibo.l5_id");
	third_api_info.m_qos._cmd = Config::GetInstance()->GetGlobalConfig().GetInt("oauth_weibo.cmd_id");
	third_api_info.m_retry = true;

    map<string, string> params;
    params["access_token"] = access_token;
    params["uid"] = openid;
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
        ret = root["error_code"].asInt();
        if (ret != 0) {
            ERROR("GetOuterProfile-response-error-ret:%d", ret);
            return ret;
        }
        outer_profile.set_uid(root["idstr"].asString());
        outer_profile.set_nickname(root["screen_name"].asString().c_str());
        outer_profile.set_headimgurl(root["avatar_hd"].asString().c_str());
        string gender = root["gender"].asString();
        if (gender == "男" || gender == "m" || gender == "M") {
            outer_profile.set_sex(1);
        } else if (gender == "女" || gender == "f" || gender == "F") {
            outer_profile.set_sex(2);
        } else {
            outer_profile.set_sex(0);
        }
        string certification;
        if(root["verified_reason"].asString().length() > 0)
        {
            certification = root["verified_reason"].asString();
        }else if(root["verified_contact_name"].asString().length() > 0)
        {
            certification = root["verified_contact_name"].asString();
        }
        if (root["verified"].asBool() && certification.length() > 0) {
            outer_profile.set_certification_type(1);
            outer_profile.set_certification(certification);
        }
        return 0;
    } else {
        ERROR("SendGetRequest-error-ret:%d", ret);
        return AccountError::LOGIN_GET_OAUTH_PROFILE_ERROR;
    }
}
}
