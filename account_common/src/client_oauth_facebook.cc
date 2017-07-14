//
// Created by gaotong1 on 2016/5/12.
//

#include "client_oauth_facebook.h"
#include "account_conf.h"
#include "string_util.h"
#include "hy_logger.h"
#include "account_error.h"
#include "json/json.h"
#include "curl.h"
#include "boost/regex.h"
using namespace client;
using namespace account;
using namespace boost;

int FackebookClient::GetOuterProfile(string access_token, OuterProfile& outer_profile)
{
    int ret = 0;

    ThirdApiInfo third_api_info;
    third_api_info.m_url = StringUtil::UrlDecode(Config::GetInstance()->GetGlobalConfig()["oauth_facebook.profile_url"]);
    third_api_info.m_domain = Config::GetInstance()->GetGlobalConfig()["oauth_facebook.domain"];
    third_api_info.m_qos._modid = Config::GetInstance()->GetGlobalConfig().GetInt("oauth_facebook.l5_id");
	third_api_info.m_qos._cmd = Config::GetInstance()->GetGlobalConfig().GetInt("oauth_facebook.cmd_id");
	third_api_info.m_retry = true;

    map<string,string> params;
    params["access_token"] = access_token;
    params["fields"] = "id,name,gender,picture";
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
            if(root["name"].isString() && root["id"].isString())
            {
                outer_profile.set_nickname(root["name"].asString());
                outer_profile.set_uid(root["id"].asString());
                string gender = root["gender"].asString();
                if(gender == "male" || gender == "男" || gender == "m"){
                    outer_profile.set_sex(1); //
                }else if(gender == "female" || gender == "女" || gender == "f"){
                    outer_profile.set_sex(2); //
                } else{
                    outer_profile.set_sex(0);
                }
                if(root["picture"].isObject() && root["picture"]["data"].isObject() && root["picture"]["data"]["url"].isString()){
                    outer_profile.set_headimgurl(root["picture"]["data"]["url"].asString());
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
        ERROR("SendGetRequest-error-ret:%d, rspHtml:%s", ret, rspHtml.c_str());
        return AccountError::LOGIN_GET_OAUTH_PROFILE_ERROR ;
    }
    
    return ret;
}


//   https://graph.facebook.com/1807974459423908/picture?width=100
int FackebookClient::GetLargerHeadImgUrl(const string& user_id, string& head_img_url)
{
    int ret = 0;

    ThirdApiInfo third_api_info;
    third_api_info.m_url = StringUtil::UrlDecode(Config::GetInstance()->GetGlobalConfig()["oauth_facebook.pic_url"]) +
    		"/" + user_id + "/picture?width=" + Config::GetInstance()->GetGlobalConfig()["oauth_facebook.pic_width"];
    third_api_info.m_domain = Config::GetInstance()->GetGlobalConfig()["oauth_facebook.domain"];
    third_api_info.m_qos._modid = Config::GetInstance()->GetGlobalConfig().GetInt("oauth_facebook.l5_id");
    third_api_info.m_qos._cmd = Config::GetInstance()->GetGlobalConfig().GetInt("oauth_facebook.cmd_id");

    string head_info;
    unsigned int connecttimeout = account::Config::getHttpConnTimeoutMs();
    unsigned int timeout = account::Config::getHttpAuthTimeoutMs();
    struct CurlRespMemory respMem;

	GenerateRandIpGroupByL5(third_api_info.m_qos);
    if (third_api_info.m_qos._host_ip.size()) {
        StringUtil::ReplaceAll(third_api_info.m_url, third_api_info.m_domain, third_api_info.m_qos._host_ip);
    }
    DEBUG("SendGetRequest-finalurl:%s", third_api_info.m_url.c_str());
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if(curl){
        curl_easy_setopt(curl, CURLOPT_URL, third_api_info.m_url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION , GetHeadInfoCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void* ) &respMem);
        curl_easy_setopt(curl, CURLOPT_HEADER, 1);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS,connecttimeout);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS,timeout );
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
        res =  curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if(res != CURLE_OK){
            ERROR("url[ %s ], error[ %s ]", third_api_info.m_url.c_str(), curl_easy_strerror(res));
            ret = -1;
        } else {
            long retcode = 0;
            res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &retcode);
            DEBUG("retcode[ %lld ], res[ %d ]", retcode, res);
            if (CURLE_OK == res && retcode == 200) {
                //DEBUG("respMem.size:[%d]", respMem.size);
                if (respMem.size > 1) {
                    string respStr(respMem.memPtr, respMem.size);
                    head_info = respStr;
                    NORMAL("head_info[ %s ]", head_info.c_str());
                    if(!head_info.empty()) {
                        vector<string> head_seg_vec = StringUtil::Split(head_info, "\n");
                        DEBUG("head_seg_size[ %d ]", head_seg_vec.size());
                        if(head_seg_vec.size()){
                            for(size_t index = 0; index < head_seg_vec.size(); ++index){
                                size_t location = head_seg_vec[index].find("Location:");
                                if(location != string::npos){
                                    NORMAL("location[ %s ]", head_seg_vec[index].c_str());
                                    vector<string> end_url_vec = StringUtil::Split(head_seg_vec[index], " ");
                                    if(end_url_vec.size() >= 2){
                                        head_img_url = end_url_vec[1];
                                        head_img_url = StringUtil::ReplaceAll(head_img_url, "\r", "");
                                        DEBUG("head_img_url[ %s ]", head_img_url.c_str());
                                    }
                                    break;
                                }
                            }
                        }
                    }else{
                        ERROR("SendGetRequest-error-ret:%d, head_info:%s", ret, head_info.c_str());
                        ret = AccountError::LOGIN_GET_OAUTH_PROFILE_ERROR ;
                    }
                }else{
                    ERROR("rsp is empty");
                    ret = -1;
                }
            }else{
                ERROR("get info failed");
                ret = -1;
            }
        }
        free(respMem.memPtr);
    }else{
        ERROR("curl_easy_init failed");
        ret = -1;
    }
    return ret;
}
