/*
 * account_logic.h
 *
 *  Created on: 2016年2月22日
 *      Author: zhongxiankui
 */

#ifndef ACCOUNT_COMMON_INCLUDE_SERVICE_ACCOUNT_LOGIC_H_
#define ACCOUNT_COMMON_INCLUDE_SERVICE_ACCOUNT_LOGIC_H_

#include <string>
#include <stdint.h>

using namespace std;


namespace account {
    /*
     * 第三方接口api请求路径信息
     */
    class ThirdApiInfo {
    public:
    	string m_url;//第三方接口请求url
    	string m_domain;//请求域名
    	QOSREQUEST m_qos;//请求ip地址在milink注册的L5信息
    	int m_httpAuth_timeoutMs;
    	int m_httpConn_timeoutMs;
    	bool m_retry;//失败是否重试
    };
}

#endif /* ACCOUNT_COMMON_INCLUDE_SERVICE_ACCOUNT_LOGIC_H_ */
