/*
 * account_conf.cc
 *
 *  Created on: 2016年2月20日
 *      Author: zhongxiankui
 */

#include "account_conf.h"

#include <linux/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include "account_constant.h"
#include "fstream"

namespace account {

    Config *Config::instance_ = NULL;
    int Config::httpAuthTimeoutMs = 5000;
    int Config::httpConnTimeoutMs = 5000;
    bool Config::httpRetry = false;

    FileConfig Config::globalConfig;

    Config::Config() {

    }

    Config *Config::GetInstance() {
        if (instance_ == NULL) {
            instance_ = new Config();
        }
        return instance_;
    }

    void Config::init(const char *szConfigFile) {
        globalConfig.ParseFile(szConfigFile);

        //http
        httpAuthTimeoutMs = globalConfig.GetInt("comm.http_auth_timeout_ms", 5000);
        httpConnTimeoutMs = globalConfig.GetInt("comm.http_conn_timeout_ms", 5000);
        httpRetry = (globalConfig.GetStr("comm.http_retry", "false") == "true");
    }

    FileConfig &Config::GetGlobalConfig() {
        return globalConfig;
    }

    int Config::getHttpAuthTimeoutMs() {
		return httpAuthTimeoutMs;
	}

	int Config::getHttpConnTimeoutMs() {
		return httpConnTimeoutMs;
	}

	bool Config::getHttpRetry() {
		return httpRetry;
	}
}
