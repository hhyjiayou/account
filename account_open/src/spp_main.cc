/*
 * s2s_feed_manager.cc
 *
 *  Created on: 2016年4月25日
 *      Author: zhubin
 */


#include <string>
#include <sys/time.h>

#include "sppincl.h"
#include "fileconfig.h"
#include "hy_header.h"
#include "hy_header_ext.pb.h"
#include "config.h"
#include "hy_logger.h"
#include "time_util.h"
#include "account_conf.h"
#include "account_error.h"
#include "open_service.h"
#include "hy_stat_mgr.h"
#include "hy_req_delay_watcher.h"

using namespace account;
using namespace hy_base;
using namespace std;

using namespace hy_base;
using namespace std;

hy_base::FileConfig g_config;

int packAndSentBuff(unsigned flow, int ret, CTCommu* tc, blob_type* _blob,
                    string snd_str) {
    int sndlen = snd_str.size() + sizeof(struct hy_base::HyHeader) + 2;
    int headlen = sizeof(struct hy_base::HyHeader) + 1;

    struct hy_base::HyHeader *p = (struct hy_base::HyHeader*) (_blob->data + 1);
    p->len = htonl(sndlen);

    char snd_buf[sndlen];
    memcpy(snd_buf, _blob->data, headlen);
    memcpy(snd_buf + headlen, snd_str.c_str(), snd_str.size());
    snd_buf[sndlen - 1] = HY_HEADER_EOT;

    blob_type send_blob;
    send_blob.data = snd_buf;
    send_blob.len = sndlen;
    tc->sendto(flow, &send_blob, NULL);
    DEBUG("send len:%d\n", sndlen);

    return 0;
}

int packAndSentBuff(int ex_head_len, unsigned flow, CTCommu* tc,
                    blob_type* _blob, string req_str, unsigned short rsp_code) {
    int ret = 0;
    int total_len =
            ex_head_len > 0 ?
            req_str.size() + sizeof(struct hy_base::HyHeader) + 2
            + ex_head_len :
            req_str.size() + sizeof(struct hy_base::HyHeader) + 2;
    int headlen =
            ex_head_len > 0 ?
            sizeof(struct hy_base::HyHeader) + 1 + ex_head_len :
            sizeof(struct hy_base::HyHeader) + 1;

    struct hy_base::HyHeader *p = (struct hy_base::HyHeader*) (_blob->data + 1);
    p->error_code = htons(rsp_code);
    p->len = htonl(total_len);

    char snd_buf[total_len];
    memcpy(snd_buf, _blob->data, headlen);
    memcpy(snd_buf + headlen, req_str.c_str(), req_str.size());
    snd_buf[total_len - 1] = HY_HEADER_EOT;

    blob_type send_blob;
    send_blob.data = snd_buf;
    send_blob.len = total_len;
    tc->sendto(flow, &send_blob, NULL);
    DEBUG("send len:%d\n", total_len);
    return ret;
}

extern "C" int spp_handle_init(void* arg1, void* arg2) {

    const char* etc = (const char*) arg1;

    CServerBase* base = (CServerBase*) arg2;
    hy_base::HyLogger *smartLog = hy_base::HyLogger::Instance();
    smartLog->SetServerBase(base);
    DEBUG("spp_handle_init, %s, %d\n", etc, base->servertype());
    
    int iRet = hy_base::HyStatMgr::instance()->init("../log/stat", 60);
    if (0 != iRet) {
        ERROR("stat manager init error, ret %d", iRet);
    }
    
    if (base->servertype() != SERVER_TYPE_WORKER)
        return 0;

    if (g_config.ParseFile(etc)) {
        ERROR("parse config file failed!\n");
    }

    Config* config = Config::GetInstance();
    config->init(etc);
    DEBUG("init config:%s ok", etc);
    return 0;
}

extern "C" int spp_handle_input(unsigned flow, void* arg1, void* arg2) {
    blob_type* blob = (blob_type*) arg1;
    DEBUG("handle data len:%d", blob->len);
    TConnExtInfo* extinfo = (TConnExtInfo*) blob->extdata;

    unsigned char* pBuf = (unsigned char*) blob->data;
    int iBufSize = blob->len;

    if (iBufSize < (int) (sizeof(hy_base::HyHeader) + sizeof(char))) {
        return 0;
    }

    hy_base::HyHeader *pPacket = (hy_base::HyHeader*) (pBuf + 1);
    if (*(char*) pBuf != HY_HEADER_SOH) {
        return -1;
    }

    int iPackLen = ntohl(pPacket->len);
    if (iPackLen < (int) (sizeof(hy_base::HyHeader) + 2 * sizeof(char))) //
    {
        return -2;
    }

    if (iBufSize < iPackLen) {
        return 0;
    }

    if (*(pBuf + iPackLen - 1) != HY_HEADER_EOT) {
        return -3;
    }
    TRACE("spp_handle_input flow:%d len:%d ip:%s", flow, blob->len,
          inet_ntoa(*(struct in_addr* )&extinfo->remoteip_));
    return iPackLen;
}

string accountTypeName[] = {"","weixin", "qq", "sinaweibo", "xiaomi"};
string getTypeName(int type){
    if(type < sizeof(accountTypeName)/ sizeof(accountTypeName[0]))
    {
        return accountTypeName[type];
    }
    return "";
}

extern "C" int spp_handle_process(unsigned flow, void* arg1, void* arg2) {
    CServerBase* base = (CServerBase*) arg2;
    DEBUG("spp_handle_process, %d\n", base->servertype());
    blob_type* _blob = (blob_type*) arg1;
    CTCommu* _pobjSppCommu = (CTCommu*) _blob->owner;

    struct hy_base::HyHeader* pPacket = (struct hy_base::HyHeader*) (_blob->data
                                                                     + 1);
    int cmd = ntohl(pPacket->cmd);
    int ex_head_len = ntohl(pPacket->header_ext_len);
    DEBUG("cmd:%d\n", cmd);
    DEBUG("ex_head_len:%d\n", ex_head_len);
    string data = "";
    string extData = "";
    HyHeaderExt hy_header_ext;
    string trace_id = "";
    string rpc_id = "";
    if (ex_head_len == 0) {
        // soh + hy_base::HyHeader + body + eot
        char *pbody = (_blob->data + sizeof(struct hy_base::HyHeader) + 1);
        int bodylen = _blob->len - 1 - sizeof(struct hy_base::HyHeader) - 1;
        data.assign(pbody, bodylen);
        DEBUG("total len:%d body len:%d\n", _blob->len, bodylen);
    } else {
        // soh + hy_base::HyHeader + hy_header_ext + body + eot
        char *pbody = (_blob->data + sizeof(struct hy_base::HyHeader) + 1
                       + ex_head_len);
        int bodylen = _blob->len - 1 - sizeof(struct hy_base::HyHeader) - 1
                      - ex_head_len;
        data.assign(pbody, bodylen);
        DEBUG("total len:%d body len:%d  ex_head_len : %d\n", _blob->len,
              bodylen, ex_head_len);

        char *pExt = (_blob->data + sizeof(HyHeader) + 1);
        extData.assign(pExt, ex_head_len);

        hy_header_ext.ParseFromArray(
                _blob->data + sizeof(HyHeader) + sizeof(char), ex_head_len);
        DEBUGPB(hy_header_ext);
        if (hy_header_ext.has_xtrace_info()) {
            XtraceInfo xtrace_info = hy_header_ext.xtrace_info();
            if (xtrace_info.has_trace_id()) {
                trace_id = xtrace_info.trace_id();
            }
            if (xtrace_info.has_rpc_id()) {
                rpc_id = xtrace_info.rpc_id();
            }
        }
    }

    AccountOpenService* openService = AccountOpenService::getInstance();

    if (cmd == 1001) { // 验证token
        zhibo_open::VerifyAccessTokenReq req;
        zhibo_open::VerifyAccessTokenRsp rsp;
        
        bool flag = req.ParseFromString(data);
        DEBUGPB(req);
        int ret =0;
        //Todo:PB 解析失败处理
        if (!flag) {
            ret = AccountError::LOGIC_PB_PARSE_ERROR;
            rsp.set_retcode(ret);
            string snd_str = rsp.SerializeAsString();
            packAndSentBuff(ex_head_len, flow, _pobjSppCommu, _blob, snd_str, 0);
            ERROR("parse pb VerifyAccessTokenReq failed.");
            return -1;
        }

        string stat_cmd = req.appid() + ".account_open.verify" + getTypeName(req.account_type());
        HyReqDelayWatcher watcher;
        watcher.begin(stat_cmd.c_str());
        
        ret = openService->VerifyAccessToken(req, rsp);
        if(ret)
        {
            ERROR("VerifyAccessToken ERROR. ret:%d", ret);
        }
        rsp.set_retcode(ret);
        watcher.end(ret);
        DEBUGPB(rsp);
        string snd_str = rsp.SerializeAsString();
        packAndSentBuff(ex_head_len, flow, _pobjSppCommu, _blob, snd_str, 0);
        
    } else if(cmd == 1002) {
        zhibo_open::GetAccessTokenByCodeReq req;
        zhibo_open::GetAccessTokenByCodeRsp rsp;
        bool flag = req.ParseFromString(data);
        DEBUGPB(req);
        int ret =0;
        //Todo:PB 解析失败处理
        if (!flag) {
            ret = AccountError::LOGIC_PB_PARSE_ERROR;
            rsp.set_retcode(ret);
            string snd_str = rsp.SerializeAsString();
            packAndSentBuff(ex_head_len, flow, _pobjSppCommu, _blob, snd_str, 0);
            ERROR("parse pb GetAccessTokenByCodeReq failed.");
            ERRORPB(req);
            return -1;
        }
        string stat_cmd = req.appid() + ".account_open.gettoken" + getTypeName(req.account_type());
        HyReqDelayWatcher watcher;
        watcher.begin(stat_cmd.c_str());
        OpenAccountInfo* accountInfo = new OpenAccountInfo();
        ret = openService->GetAccessTokenProcess(req, *accountInfo);
        if(!ret)
        {
            if(!accountInfo->has_uid())
            {
                accountInfo->set_uid(0);
            }
            rsp.set_allocated_open_account_info(accountInfo);
        }else{
            ERROR("GetAccessTokenProcess ERROR. ret:%d", ret);
            ERRORPB(req);
        }
        watcher.end(ret);
        
        rsp.set_retcode(ret);
        DEBUGPB(rsp);
        string snd_str = rsp.SerializeAsString();
        packAndSentBuff(ex_head_len, flow, _pobjSppCommu, _blob, snd_str, 0);
    }else if(cmd == 1003)
    {
        zhibo_open::GetOpenProfileReq req;
        zhibo_open::GetOpenProfileRsp rsp;
        bool flag = req.ParseFromString(data);
        DEBUGPB(req);
        int ret =0;
        //Todo:PB 解析失败处理
        if (!flag) {
            ret = AccountError::LOGIC_PB_PARSE_ERROR;
            rsp.set_retcode(ret);
            string snd_str = rsp.SerializeAsString();
            packAndSentBuff(ex_head_len, flow, _pobjSppCommu, _blob, snd_str, 0);
            ERROR("parse pb GetOpenProfileReq failed.");
            return -1;
        }
        
        string stat_cmd = req.appid() + ".account_open.getprofile" + getTypeName(req.account_type());
        HyReqDelayWatcher watcher;
        watcher.begin(stat_cmd.c_str());
        OuterProfile* profile= new OuterProfile();
        ret = openService->GetOpenProfile(req, *profile);
        if(!ret)
        {
            rsp.set_allocated_outerprofile(profile);
        }else{
            ERROR("GetOpenProfile ERROR. ret:%d", ret);
            ERRORPB(req);
        }
        watcher.end(ret);
        
        rsp.set_retcode(ret);
        DEBUGPB(rsp);
        string snd_str = rsp.SerializeAsString();
        packAndSentBuff(ex_head_len, flow, _pobjSppCommu, _blob, snd_str, 0);
    }else if(cmd == 1004){ //针对instagram
        zhibo_open::GetAccessTokenAndBaseInfoByCodeReq req;
        zhibo_open::GetAccessTokenAndBaseInfoByCodeRsp rsp;
        bool flag = req.ParseFromString(data);
        DEBUGPB(req);
        int ret =0;
        //Todo:PB 解析失败处理
        if (!flag) {
            ret = AccountError::LOGIC_PB_PARSE_ERROR;
            rsp.set_retcode(ret);
            string snd_str = rsp.SerializeAsString();
            packAndSentBuff(ex_head_len, flow, _pobjSppCommu, _blob, snd_str, 0);
            ERROR("parse pb GetAccessTokenByCodeReq failed.");
            ERRORPB(req);
            return -1;
        }
        string stat_cmd = req.appid() + ".account_open.gettoken" + getTypeName(req.account_type());
        HyReqDelayWatcher watcher;
        watcher.begin(stat_cmd.c_str());
        OpenAccountInfo open_account_info;
        OuterProfile outer_profile;
        ret = openService->GetAccessTokenProcess(req, open_account_info, outer_profile);
        if(!ret)
        {
            if(!open_account_info.has_uid())
            {
                open_account_info.set_uid(0);  //设置uid，防止解析pb时由于没有设置required字段出现3203
            }
            rsp.mutable_open_account_info()->CopyFrom(open_account_info);
            rsp.mutable_outer_profile()->CopyFrom(outer_profile);
        }else{
            ERROR("GetAccessTokenProcess ERROR. ret:%d", ret);
            ERRORPB(req);
        }
        watcher.end(ret);
        rsp.set_retcode(ret);
        DEBUGPB(rsp);
        string snd_str = rsp.SerializeAsString();
        packAndSentBuff(ex_head_len, flow, _pobjSppCommu, _blob, snd_str, 0);
    }
    else {
        DEBUG("error cmd cannot found. CMD:%d", cmd);
    }
    return 0;
}





