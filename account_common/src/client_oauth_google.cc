//
// Created by gaotong1 on 2016/5/12.
//

#include "client_oauth_google.h"
#include "account_conf.h"
#include "string_util.h"
#include "hy_logger.h"
#include "account_error.h"
#include "json/json.h"

using namespace client;
using namespace account;


int GoogleClient::GetOuterProfile(string access_token, OuterProfile& outer_profile)
{
    int ret = 0;
    ThirdApiInfo third_api_info;
    third_api_info.m_url = StringUtil::UrlDecode(Config::GetInstance()->GetGlobalConfig()["oauth_google.profile_url"]);
    third_api_info.m_domain = Config::GetInstance()->GetGlobalConfig()["oauth_google.domain"];
    third_api_info.m_qos._modid = Config::GetInstance()->GetGlobalConfig().GetInt("oauth_google.l5_id");
	third_api_info.m_qos._cmd = Config::GetInstance()->GetGlobalConfig().GetInt("oauth_google.cmd_id");
	third_api_info.m_retry = true;

    map<string,string> params;
    params["access_token"] = access_token;
    string rspHtml;
    ret = SendGetRequest(third_api_info, params, rspHtml);
    if(!ret && !rspHtml.empty())
    {
        Json::Reader reader;
        Json::Value root;
        if (!reader.parse(rspHtml, root, false)) {
            ERROR("GetOuterProfile-parse-response-error:%s", rspHtml.c_str());
            return AccountError::LOGIN_GET_OAUTH_PROFILE_ERROR;
        }
        if(!root["error"].isObject())
        {
            if(root["displayName"].isString() && root["id"].isString())
            {
                outer_profile.set_nickname(root["displayName"].asString());
                outer_profile.set_uid(root["id"].asString());
                if (root["gender"].isString())
                {
                    string gender = root["gender"].asString();
                    if (gender == "male" || gender == "男" || gender == "m")
                    {
                        outer_profile.set_sex(1); //
                    } else if (gender == "female" || gender == "女" || gender == "f")
                    {
                        outer_profile.set_sex(2); //
                    } else
                    {
                        outer_profile.set_sex(0);
                    }
                }
                if (root["image"].isObject() && root["image"]["url"].isString())
                {
                    outer_profile.set_headimgurl(root["image"]["url"].asString());
                }
            }else
            {
                ERROR("GetOuterProfile-parse-response-error:%s", rspHtml.c_str());
                return AccountError::LOGIN_GET_OAUTH_PROFILE_ERROR;
            }
        }else{
            int ret = root["error"]["code"].asInt();
            ERROR("GetOuterProfile-parse-response-error:%s", rspHtml.c_str());
            return ret;
        }
        
        
    }else{
        ERROR("SendGetRequest-error-ret:%d", ret);
        return AccountError::LOGIN_GET_OAUTH_PROFILE_ERROR;
    }
    
    return ret;
}
