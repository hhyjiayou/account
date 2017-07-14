/*
 * account_error.h
 *
 *  Created on: 2016年2月20日
 *      Author: zhongxiankui
 */

#ifndef CONFIG_INCLUDE_ACCOUNT_ERROR_H_
#define CONFIG_INCLUDE_ACCOUNT_ERROR_H_

class AccountError {

public:
    static const int RET_SUCCESS = 0;
    static const int RET_ERROR = -1;

    static const int LOGIC_PB_PARSE_ERROR = 1202;
	static const int LOGIC_PARAM_NEQ_ERROR = 1212;
    static const int LOGIC_INVALID_CODE_ERROR = 1214;//

     //响应客户端的错误编码6001~6500
    static const int LOGIN_GET_OAUTH_ACCESSTOKEN_ERROR = 6001;//获取oauth_accesstoken失败
    static const int LOGIN_VERIFY_OAUTH_ACCESSTOKEN_ERROR = 6002;//检验oauth_accesstoken失败
    static const int LOGIN_GET_OAUTH_PROFILE_ERROR = 6003;//获取第三方个人资料失败
    static const int PASSTOKEN_INVALID_ERROR = 6004;//passtoken失效
    static const int INPUT_PARAMS_INCOMPLETE_ERROR = 6005;//输入参数不全
    static const int GENERATE_SERVICETOKEN_ERROR = 6006;//生成新的servicetoken失败
    static const int GET_UID_ERROR = 6007;//获取uid失败
    static const int BUILD_ZB_ACCOUNT_INFO_ERROR = 6008;//创建直播帐号信息失败
    static const int BUILD_ZB_TOEKN_ERROR = 6009;//创建直播token失败
    static const int SERVER_ERROR = 6010;//服务端bug
};

#endif /* CONFIG_INCLUDE_ACCOUNT_ERROR_H_ */
