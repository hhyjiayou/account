/*
 * client_oauth_base.cc
 *
 *  Created on: 2016年2月22日
 *      Author: zhangyouming
 */
#include <unistd.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include "curl.h"
#include "string_util.h"
#include "client_oauth_base.h"
#include "account_log.h"
#include "account_conf.h"
#include "time_util.h"
#include "qos_client.h"

using namespace hy_base;

namespace client {

int BaseOauthClient::SendGetRequest(ThirdApiInfo third_api_info, map<string, string> params, string& response) {
    DEBUG("SendGetRequestWithRetry,domain:%s,l5_id:%d, cmd_id:%d, retry:%d",
    		third_api_info.m_domain.c_str(), third_api_info.m_qos._modid,
			third_api_info.m_qos._cmd, third_api_info.m_retry);

    third_api_info.m_httpConn_timeoutMs = account::Config::getHttpConnTimeoutMs();
    third_api_info.m_httpAuth_timeoutMs = account::Config::getHttpAuthTimeoutMs();
    if (account::Config::getHttpRetry() && third_api_info.m_retry) {
    	third_api_info.m_httpConn_timeoutMs = account::Config::getHttpConnTimeoutMs() / 2;
    	third_api_info.m_httpAuth_timeoutMs = account::Config::getHttpAuthTimeoutMs() / 2;
    }

    long long start_time = TimeUtil::GetCurrentTime();
    string errmsg = "";
    int ret = SendGetRequest(third_api_info, params, response, errmsg);
    if (ret == -1 && account::Config::getHttpRetry() && third_api_info.m_retry) {
        ret = SendGetRequest(third_api_info, params, response, errmsg);
        NORMAL("retryurlprefix:%s,ret:%d", third_api_info.m_url.c_str(), ret);
        if (ret != 0) {
            ERROR("retryurlprefix:%s,fail, host_ip:[%s]", third_api_info.m_url.c_str(),
            		third_api_info.m_qos._host_ip.c_str());
        }
    }
    long long end_time = TimeUtil::GetCurrentTime();

    //ip请求失败，milink的l5上报
    if(0 != ret){
    	ApiRouteResultUpdate(third_api_info.m_qos, -1, end_time - start_time, errmsg);
    }else{
    	ApiRouteResultUpdate(third_api_info.m_qos, 0, end_time - start_time, errmsg);
    }
    return ret;
}

    int BaseOauthClient::SendPostRequest(ThirdApiInfo third_api_info, const string& post_fields, string& response)
    {
        DEBUG("SendPostRequestWithRetry,domain:%s,l5_id:%d, cmd_id:%d, retry:%d",
        		third_api_info.m_domain.c_str(), third_api_info.m_qos._modid,
				third_api_info.m_qos._cmd, third_api_info.m_retry);

        third_api_info.m_httpConn_timeoutMs = account::Config::getHttpConnTimeoutMs();
		third_api_info.m_httpAuth_timeoutMs = account::Config::getHttpAuthTimeoutMs();
		if (account::Config::getHttpRetry() && third_api_info.m_retry) {
			third_api_info.m_httpConn_timeoutMs = account::Config::getHttpConnTimeoutMs() / 2;
			third_api_info.m_httpAuth_timeoutMs = account::Config::getHttpAuthTimeoutMs() / 2;
		}

		long long start_time = TimeUtil::GetCurrentTime();
        string errmsg = "";
        int ret = SendPostRequest(third_api_info, post_fields, response, errmsg);
        if (ret == -1 && account::Config::getHttpRetry() && third_api_info.m_retry) {
            ret = SendPostRequest(third_api_info, post_fields, response, errmsg);
            NORMAL("retryurlprefix:%s,ret:%d", third_api_info.m_url.c_str(), ret);
            if (ret != 0) {
                ERROR("retryurlprefix:%s,fail, host_ip:[%s]", third_api_info.m_url.c_str(),
                		third_api_info.m_qos._host_ip.c_str());
            }
        }
        long long end_time = TimeUtil::GetCurrentTime();

        if(0 != ret){
        	ApiRouteResultUpdate(third_api_info.m_qos, -1, end_time - start_time, errmsg);
        }else{
        	ApiRouteResultUpdate(third_api_info.m_qos, 0, end_time - start_time, errmsg);
        }
        return ret;
    }


int BaseOauthClient::SendGetRequest(ThirdApiInfo third_api_info, map<string, string> params, string& response,string& errmsg) {
    int ret = 0;
    struct CurlRespMemory respMem;
    respMem.memPtr = (char*) malloc(1); //tail :0
    respMem.size = 0;
    if (params.size() > 0) {
        map<string, string>::iterator iter;
        for (iter = params.begin(); iter != params.end(); iter++) {
            if (iter == params.begin()) {
            	third_api_info.m_url.append("?");
            } else {
            	third_api_info.m_url.append("&");
            }
            third_api_info.m_url.append(iter->first);
            third_api_info.m_url.append("=");
            third_api_info.m_url.append(StringUtil::UrlEncode(iter->second));
        }
    }
    GenerateRandIpGroupByL5(third_api_info.m_qos);
    if (third_api_info.m_qos._host_ip.length() > 0) {
        StringUtil::ReplaceAll(third_api_info.m_url, third_api_info.m_domain, third_api_info.m_qos._host_ip);
    }
    DEBUG("SendGetRequest-finalurl:%s", third_api_info.m_url.c_str());
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
        if (third_api_info.m_qos._host_ip.length() > 0) {
            struct curl_slist *header = NULL;
            string host = "Host:" + third_api_info.m_domain;
            header = curl_slist_append(header, host.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
        }
        curl_easy_setopt(curl, CURLOPT_URL, third_api_info.m_url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, GetRespCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void* ) &respMem);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);

        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, third_api_info.m_httpConn_timeoutMs);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, third_api_info.m_httpAuth_timeoutMs);
        
        //      curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            ERROR("url:%s,error:%s\n", third_api_info.m_url.c_str(), curl_easy_strerror(res));
            ret = -1;
        } else {
            long retcode = 0;
            /*http://curl.haxx.se/libcurl/c/easy_getinfo_options.html*/
            res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &retcode);
            DEBUG("retcode:[%lld], res:[%d]", retcode, res);
            if (CURLE_OK == res && retcode) {
                DEBUG("respMem.size:[%d]", respMem.size);
                if (respMem.size > 1) {
                    string respStr(respMem.memPtr, respMem.size);
                    response = respStr;
                }
            }
        }
//           curl_slist_free_all(header);
    } else {
        ERROR("url:%s,init curl error\n", third_api_info.m_url.c_str());
    }

    free(respMem.memPtr);
    return ret;
}


int BaseOauthClient::SendPostRequest(ThirdApiInfo& third_api_info, const string& post_fields, string& response, string& errmsg)
{
	int ret = 0;
	struct CurlRespMemory respMem;
	respMem.memPtr = (char*) malloc(1); //tail :0
	respMem.size = 0;
	GenerateRandIpGroupByL5(third_api_info.m_qos);
	if (third_api_info.m_qos._host_ip.length() > 0) {
		StringUtil::ReplaceAll(third_api_info.m_url, third_api_info.m_domain, third_api_info.m_qos._host_ip);
	}
	DEBUG("SendPostRequest-finalurl:%s", third_api_info.m_url.c_str());
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();
	if (curl) {
		if (third_api_info.m_qos._host_ip.length() > 0) {
			struct curl_slist *header = NULL;
			string host = "Host:" + third_api_info.m_domain;
			header = curl_slist_append(header, host.c_str());
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
		}
		curl_easy_setopt(curl, CURLOPT_URL, third_api_info.m_url.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, GetRespCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void* ) &respMem);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, third_api_info.m_httpConn_timeoutMs);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, third_api_info.m_httpAuth_timeoutMs);

		//      curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		if (res != CURLE_OK) {
			ERROR("url:%s,error:%s\n", third_api_info.m_url.c_str(), curl_easy_strerror(res));
			ret = -1;
		} else {
			long retcode = 0;
			/*http://curl.haxx.se/libcurl/c/easy_getinfo_options.html*/
			res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &retcode);
			DEBUG("retcode:[%lld], res:[%d]", retcode, res);
			if (CURLE_OK == res && retcode) {
				DEBUG("respMem.size:[%d]", respMem.size);
				if (respMem.size > 1) {
					string respStr(respMem.memPtr, respMem.size);
					response = respStr;
				}
			}
		}
//           curl_slist_free_all(header);
	} else {
		ERROR("url:%s,init curl error\n", third_api_info.m_url.c_str());
	}

	free(respMem.memPtr);
	return ret;
}

int BaseOauthClient::GenerateRandIpGroup(string ipgroupstr, vector<string>& ipgroup) {
    if (ipgroupstr.length() > 0) {
        unsigned long long ts = TimeUtil::GetCurrentTime();
        vector<string> groups = StringUtil::Split(ipgroupstr, ";");
        if (groups.size() > 0) {
            string group1 = groups[ts % groups.size()];
            vector<string> group1_ips = StringUtil::Split(group1, ",");
            if (group1_ips.size() > 0) {
                ipgroup.push_back(group1_ips[ts % group1_ips.size()]);
            }
            if (groups.size() > 1) {
                string group2 = groups[(ts + 1) % groups.size()];
                vector<string> group2_ips = StringUtil::Split(group2, ",");
                if (group2_ips.size() > 0) {
                    ipgroup.push_back(group2_ips[ts % group2_ips.size()]);
                }
            } else if (group1_ips.size() > 1) {
                ipgroup.push_back(group1_ips[(ts + 1) % group1_ips.size()]);
            }
        }

    }
    return 0;
}

/*http://curl.haxx.se/libcurl/c/getinmemory.html*/
size_t BaseOauthClient::GetRespCallback(void *respDataPtr, size_t size, size_t nmemb, void *localMemPtr) {
    size_t respSize = size * nmemb;

    struct CurlRespMemory* respMemPtr = (struct CurlRespMemory*) localMemPtr;
    respMemPtr->memPtr = (char*) realloc(respMemPtr->memPtr, respMemPtr->size + respSize + 1); //多用一个字节，coding复杂度
    if (respMemPtr->memPtr == NULL) {
        ERROR("realloc out of memory");
        return 0;
    }

    DEBUG("respSize:[%llu]", respSize);
    memcpy(&(respMemPtr->memPtr[respMemPtr->size]), respDataPtr, respSize);
    respMemPtr->size += respSize;
    respMemPtr->memPtr[respMemPtr->size] = 0;

    return respSize;
}

    size_t BaseOauthClient::GetHeadInfoCallback(char *respDataPtr, size_t size, size_t nmemb, void *localMemPtr){

//        size_t resp_size = size * nmemb;
//        struct CurlRespMemory* resp_mem_ptr = (struct CurlRespMemory*) localMemPtr;
//
//        resp_mem_ptr->memPtr = new char[size * nmemb + 1]; //多用一个字节，coding复杂度
//
//        if (resp_mem_ptr->memPtr == NULL) {
//            ERROR("realloc out of memory");
//            return 0;
//        }
//        DEBUG("respSize:[%llu]", resp_size);
//        memcpy(resp_mem_ptr->memPtr, respDataPtr, resp_size);
//        resp_mem_ptr->size += resp_size;
//        resp_mem_ptr->memPtr[resp_mem_ptr->size] = 0;
//        return resp_size;
        size_t respSize = size * nmemb;

        struct CurlRespMemory* respMemPtr = (struct CurlRespMemory*) localMemPtr;
        respMemPtr->memPtr = (char*) realloc(respMemPtr->memPtr, respMemPtr->size + respSize + 1); //多用一个字节，coding复杂度
        if (respMemPtr->memPtr == NULL) {
            ERROR("realloc out of memory");
            return 0;
        }

        memcpy(&(respMemPtr->memPtr[respMemPtr->size]), respDataPtr, respSize);
        respMemPtr->size += respSize;
        respMemPtr->memPtr[respMemPtr->size] = 0;

        return respSize;
    }

int BaseOauthClient::GenerateRandIpGroupByL5(QOSREQUEST& qos){
	string err_msg;
	int ret = ApiGetRoute(qos, 0.02, err_msg);
	if (ret < 0) {
		ERROR("get ip address error, ret:[%d], l5_id:[%d], cmd_id:[%d], err_msg:[%s]", ret, qos._modid, qos._cmd, err_msg.c_str());
	}
	ERROR("[NORMAL] host_ip:[%s], port:[%d]", qos._host_ip.c_str(), qos._host_port);
	return 0;
}
}
