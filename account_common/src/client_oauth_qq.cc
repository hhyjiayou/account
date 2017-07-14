/*
 * client_oauth_qq.cc
 *
 *  Created on: 2016年2月22日
 *      Author: zhangyouming
 */
#include "json/json.h"
#include "client_oauth_qq.h"
#include "string_util.h"
#include "account_conf.h"
#include "account_log.h"
#include "account_error.h"
#include "time_util.h"

using namespace hy_base;
using namespace account;

namespace client {

int QqOauthClient::VerifyAccessToken(string access_token, string openid, string& unionid, string &client_id) {
    ThirdApiInfo third_api_info;
    third_api_info.m_url = StringUtil::UrlDecode(Config::GetInstance()->GetGlobalConfig()["oauth_qq.access_token_verify_url"]);
    third_api_info.m_domain = Config::GetInstance()->GetGlobalConfig()["oauth_qq.domain"];
    third_api_info.m_qos._modid = Config::GetInstance()->GetGlobalConfig().GetInt("oauth_qq.l5_id");
    third_api_info.m_qos._cmd = Config::GetInstance()->GetGlobalConfig().GetInt("oauth_qq.cmd_id");
    third_api_info.m_retry = true;
    
    map<string, string> params;
    params["access_token"] = access_token;
    params["unionid"] = "1";
    string response;

    int ret = SendGetRequest(third_api_info, params, response);
    DEBUG("VerifyAccessToken-response:%s", response.c_str());
    if (0 == ret) {
        Json::Reader reader;
        Json::Value root;
        if (response.length() > 9) { //返回值格式为：callback( {"client_id":"1105190146","openid":"88B59874CDD1D2EDED4DAC97D70EC7EB"} );
            response = response.substr(9, response.length() - 9 - 2);
        }
        if (!reader.parse(response, root, false)) {
            ERROR("VerifyAccessToken-parse-response-error:%s", response.c_str());
            return AccountError::LOGIN_VERIFY_OAUTH_ACCESSTOKEN_ERROR;
        }
        ret = root["error"].asInt();
        if (ret != 0) {
            ERROR("VerifyAccessToken-response-error-ret:%d", ret);
            return ret;
        }
        if (openid == root["openid"].asString()) {
            client_id = root["client_id"].asString();
            unionid = root["unionid"].asString();
             if(unionid.empty()){
             	ERROR("VerifyAccessToken-unionid-error, input-accessToken:%s", access_token.c_str());
             	return AccountError::LOGIN_VERIFY_OAUTH_ACCESSTOKEN_ERROR;
             }
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

int QqOauthClient::GetOuterProfile(uint32_t account_type, string access_token,
		string openid, OuterProfile& outer_profile, string appid_local) {
	ThirdApiInfo third_api_info;
	third_api_info.m_url = StringUtil::UrlDecode(Config::GetInstance()->GetGlobalConfig()["oauth_qq.profile_url"]);
	third_api_info.m_domain = Config::GetInstance()->GetGlobalConfig()["oauth_qq.domain"];
	third_api_info.m_qos._modid = Config::GetInstance()->GetGlobalConfig().GetInt("oauth_qq.l5_id");
	third_api_info.m_qos._cmd = Config::GetInstance()->GetGlobalConfig().GetInt("oauth_qq.cmd_id");
	third_api_info.m_retry = true;

    map<string, string> params;
    params["access_token"] = access_token;
    params["openid"] = openid;
    string appid_conf_key = "oauth_qq_" + appid_local;
    if(TYPE_QQ == account_type){
    	params["oauth_consumer_key"] = Config::GetInstance()->GetGlobalConfig()[(appid_conf_key + ".appid").c_str()];
    }else{
    	params["oauth_consumer_key"] = Config::GetInstance()->GetGlobalConfig()[(appid_conf_key + ".h5_appid").c_str()];
    }
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
        ret = root["ret"].asInt();
        if (ret != 0) {
            ERROR("GetOuterProfile-response-error-ret:%d", ret);
            return ret;
        }
        outer_profile.set_nickname(root["nickname"].asString().c_str());
        if(!root["figureurl_qq_2"].asString().empty())
        	outer_profile.set_headimgurl(root["figureurl_qq_2"].asString().c_str());
        else
        	outer_profile.set_headimgurl(root["figureurl_qq_1"].asString().c_str());
        string gender = root["gender"].asString();
        if (gender == "男" || gender == "m" || gender == "M") {
            outer_profile.set_sex(1);
        } else if (gender == "女" || gender == "f" || gender == "F") {
            outer_profile.set_sex(2);
        } else {
            outer_profile.set_sex(0);
        }
        return 0;
    } else {
        ERROR("SendGetRequest-error-ret:%d", ret);
        return AccountError::LOGIN_GET_OAUTH_PROFILE_ERROR;
    }
}



}
