/*
 * account_conf.h
 *
 *  Created on: 2016年2月20日
 *      Author: zhongxiankui
 */

#ifndef CONFIG_INCLUDE_ACCOUNT_CONF_H_
#define CONFIG_INCLUDE_ACCOUNT_CONF_H_

#include "fileconfig.h"
#include "comm_util.h"
#include <map>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "string_util.h"
using namespace std;
using namespace hy_base;

namespace account {

    class Config {
        public:
        static Config *GetInstance();
        void init(const char *szConfigFile);

        FileConfig &GetGlobalConfig();
        static int getHttpAuthTimeoutMs();
	    static int getHttpConnTimeoutMs();
	    static bool getHttpRetry();

        private:
        Config();
        static Config *instance_;
        static FileConfig globalConfig;

        //http
        static int httpAuthTimeoutMs;
        //单位毫秒
        static int httpConnTimeoutMs;
        //单位毫秒
        static bool httpRetry;//是支持重试
    };

}

#endif /* CONFIG_INCLUDE_ACCOUNT_CONF_H_ */
