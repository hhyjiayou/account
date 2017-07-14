/*
 * account_log.h
 *
 *  Created on: 2016年2月20日
 *      Author: zhongxiankui
 */

#ifndef CONFIG_INCLUDE_ACCOUNT_LOG_H_
#define CONFIG_INCLUDE_ACCOUNT_LOG_H_

#include <stdio.h>
using namespace std;

#ifdef MHTTP_LOG

#include "commlog.h"

#define TRACE(args...)  MFCGI_Log_Orig(__FILE__, __LINE__, __FUNCTION__, LM_TRACE, args)
#define DEBUG(args...)  MFCGI_Log_Orig(__FILE__, __LINE__, __FUNCTION__, LM_DEBUG, args)
#define NORMAL(args...)  MFCGI_Log_Orig(__FILE__, __LINE__, __FUNCTION__, LM_INFO, args)
#define ERROR(args...) MFCGI_Log_Orig(__FILE__, __LINE__, __FUNCTION__, LM_ERROR, args)

#endif

#ifdef STD_LOG

extern FILE *mystdout;
#define TRACE(format, args...) \
    do{ \
        fprintf(mystdout, "[%s %s][TRACE][%s][%d]", __DATE__, __TIME__,  __FILE__, __LINE__); \
        fprintf(mystdout ,format"\n", ##args); \
    }while(0)

#define DEBUG(format, args...) \
    do{ \
        fprintf(mystdout, "[%s %s][DEBUG][%s][%d]", __DATE__, __TIME__, __FILE__, __LINE__);  \
        fprintf(mystdout ,format"\n", ##args); \
    }while(0)

#define NORMAL(format, args...) \
    do{ \
        fprintf(mystdout, "[%s %s][NORMAL][%s][%d]", __DATE__, __TIME__, __FILE__, __LINE__); \
        fprintf(mystdout ,format"\n", ##args); \
    } while(0)

#define ERROR(format, args...) \
    do{ \
        fprintf(mystdout, "[%s %s][ERROR][%s][%d]", __DATE__, __TIME__, __FILE__, __LINE__); \
        fprintf(mystdout ,format"\n", ##args); \
}while(0)

#endif

#ifdef MCP_LOG
#include "tfc_debug_log.h"
#define TRACE(fmt, args...) TFCDebugLog::instance()->LOG_FUNC(LOG_TRACE, fmt, ##args)
#define DEBUG(fmt, args...) TFCDebugLog::instance()->LOG_FUNC(LOG_DEBUG, fmt, ##args)
#define NORMAL(fmt, args...)  TFCDebugLog::instance()->LOG_FUNC(LOG_NORMAL, fmt, ##args)
#define ERROR(fmt, args...) TFCDebugLog::instance()->LOG_FUNC(LOG_ERROR, fmt, ##args)
#endif

#ifndef MHTTP_LOG
#ifndef STD_LOG
#ifndef MCP_LOG
#include "hy_logger.h"
using namespace hy_base;
#endif
#endif
#endif

#endif /* CONFIG_INCLUDE_ACCOUNT_LOG_H_ */
