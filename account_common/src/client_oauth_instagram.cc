//
// Created by songxiangyu on 2016/10/10.
//

#include <map>
#include "json/json.h"
#include "client_oauth_instagram.h"
#include "account_conf.h"
#include "string_util.h"
#include "time_util.h"
#include "account_log.h"
#include "account_error.h"

using namespace hy_base;
using namespace account;

namespace client {
    int InstagramOauthClient::GetAccessTokenAndBaseInfo(const string &code, OpenAccountInfo &open_account_info,
    		OuterProfile& outer_profile, string appid_local) {
    	ThirdApiInfo third_api_info;
    	third_api_info.m_url = StringUtil::UrlDecode(Config::GetInstance()->GetGlobalConfig()["oauth_instagram.access_token_url"]);
    	third_api_info.m_domain = Config::GetInstance()->GetGlobalConfig()["oauth_instagram.domain"];
    	third_api_info.m_qos._modid = Config::GetInstance()->GetGlobalConfig().GetInt("oauth_instagram.l5_id");
    	third_api_info.m_qos._cmd = Config::GetInstance()->GetGlobalConfig().GetInt("oauth_instagram.cmd_id");
    	third_api_info.m_retry = true;

        string post_fields;
        Config* config_instance = Config::GetInstance();
		string appid_conf_key = "oauth_instagram_" + appid_local;
        post_fields = string("client_id=") +  config_instance->GetGlobalConfig()[(appid_conf_key + ".client_id").c_str()] + "&";
        post_fields += string("client_secret=") + config_instance->GetGlobalConfig()[(appid_conf_key + ".client_secret").c_str()] + "&";
        post_fields += string("grant_type=") + config_instance->GetGlobalConfig()["oauth_instagram.grant_type"] + "&";
        post_fields += string("redirect_uri=") + StringUtil::UrlDecode( config_instance->GetGlobalConfig()[(appid_conf_key + ".redirect_uri").c_str()] ) + "&";
        NORMAL("redirect_uri[ %s ]", StringUtil::UrlDecode( config_instance->GetGlobalConfig()[(appid_conf_key + ".redirect_uri").c_str()]).c_str());
        post_fields += string("code=") + code;
        string response;

        int ret = SendPostRequest(third_api_info, post_fields, response);
        DEBUG("GetAccessTokenAndBaseInfo[ %s ]", response.c_str());
        if (!ret) {
            Json::Reader reader;
            Json::Value root;
            if (!reader.parse(response, root, false)) {
                ERROR("GetAccessTokenAndBaseInfo response[ %s ]", response.c_str());
                ret = AccountError::LOGIN_GET_OAUTH_ACCESSTOKEN_ERROR;  //6001 获取accessToken失败
            }else{
                DEBUG("GetAccessTokenAndBaseInfo response[ %s ]", response.c_str());
                if(!root["code"].asInt()){   //调用成功时，返回json中没有code字段，失败时含有"code"字段
                    string access_token = root["access_token"].asString();
                    if(!access_token.empty() && root["user"].isObject()){
                        DEBUG("access_token[ %s ], user[ %s ]", access_token.c_str(), root["user"].toStyledString().c_str());
                        string id = root["user"]["id"].asString();
                        string username = root["user"]["username"].asString();
                        string full_name = root["user"]["full_name"].asString();
                        string head_img_url = root["user"]["profile_picture"].asString();
                        open_account_info.set_openid(id);
                        open_account_info.set_access_token(access_token);
                        open_account_info.set_expires_in(0);
                        open_account_info.set_token_update_ts(0);
                        open_account_info.set_refresh_token("");
                        outer_profile.set_nickname(full_name);
                        outer_profile.set_headimgurl(head_img_url);
                        outer_profile.set_sex(0);  //instagram的baseInfo暂时拿不到性别
                    }else{
                        ERROR("get successful, but access_token or base_info is empty");  //access_token或者baseInfo为空，认为还是调用失败，返回6002
                        ret = AccountError::LOGIN_GET_OAUTH_ACCESSTOKEN_ERROR;
                    }
                }else{
                    ret = root["code"].asInt();
                    ERROR("GetAccessTokenAndBaseInfo failed, ret[ %d ], rsp[ %s ]", ret, response.c_str());
                }
            }
        } else {
            ERROR("SendGetRequest-error-ret:%d", ret);
            ret = AccountError::LOGIN_GET_OAUTH_ACCESSTOKEN_ERROR;
        }
        return ret;
    }
}
