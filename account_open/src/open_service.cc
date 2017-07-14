//
// Created by gaotong1 on 2016/5/10.
//

#include "open_service.h"

#define GOOGLE_INVALID_CODE 401
#define FB_INVALID_CODE 190
#define QQ_INVALID_CODE_100016  100016
#define WX_INVALID_CODE_40001  40001
#define WX_INVALID_CODE_40029  40029
#define XM_INVALID_CODE_96013  96013

int AccountOpenService::VerifyAccessToken(VerifyAccessTokenReq& req, VerifyAccessTokenRsp& rsp)
{
    int ret = AccountError::RET_SUCCESS;
    if (req.accesstoken().empty() || req.openid().empty())
    {
        ERROR("empty access_token:[%s] or openid:[%s]", req.accesstoken().c_str(), req.openid().c_str());
        return AccountError::LOGIC_PARAM_NEQ_ERROR;
    }

	string unionid = "";
    if (TYPE_QQ == req.account_type() || TYPE_H5_QQ == req.account_type())//get access_token and unionid
    {
        string client_id;
        ret = qq_client.VerifyAccessToken(req.accesstoken(), req.openid(), unionid, client_id);
        if(!ret && req.appid() == "10010" && !client_id.empty()){
            string config_appid = Config::GetInstance()->GetGlobalConfig()["oauth_qq_10010.appid"];
            if(client_id != config_appid){
                ret = AccountError::LOGIN_VERIFY_OAUTH_ACCESSTOKEN_ERROR;
                ERROR("access_token[ %s ], client_id[ %s ], config_appid[ %s ]", req.accesstoken().c_str(), client_id.c_str(), config_appid.c_str());
            }
        }
    } else if (TYPE_SINAWEIBO == req.account_type())
    {
        ret = weibo_sina_client.VerifyAccessToken(req.accesstoken(), req.openid());
    } else
    {
        ERROR("params invalid! token:%s, openid:%s", req.accesstoken().c_str(), req.openid().c_str());
        return AccountError::LOGIC_PARAM_NEQ_ERROR;
    }
    if (ret)
    {
        ERROR("VerifyAccessTokenReq ret:%d, token:%s, openid:%s", ret, req.accesstoken().c_str(), req.openid().c_str());
    }
    ret = filterErrorCode(req.account_type(), ret);
    if (ret < 0)
    {
        ret = AccountError::LOGIN_VERIFY_OAUTH_ACCESSTOKEN_ERROR;
    }
    rsp.set_unionid(unionid);
    DEBUG("unionid:%s", unionid.c_str());
    return ret;
}

int AccountOpenService::GetOpenProfile(GetOpenProfileReq& req, OuterProfile& outerProfile)
{
    if (TYPE_SINAWEIBO == req.account_type() || TYPE_WEIXIN == req.account_type()
        || TYPE_QQ == req.account_type() || TYPE_H5_QQ == req.account_type())
    {
        if (req.accesstoken().empty() || req.openid().empty())
        {
            ERRORPB(req);
            return AccountError::LOGIC_PARAM_NEQ_ERROR;
        }
    } else
    {
        if (req.accesstoken().empty())
        {
            ERRORPB(req);
            return AccountError::LOGIC_PARAM_NEQ_ERROR;
        }
    }
    int ret = 0;
    if (TYPE_SINAWEIBO == req.account_type())
    {
        ret = weibo_sina_client.GetOuterProfile(req.accesstoken(), req.openid(), outerProfile);
    } else if (TYPE_WEIXIN == req.account_type())
    {
        ret = weixin_client.GetOuterProfile(req.accesstoken(), req.openid(), outerProfile);
    } else if (TYPE_QQ == req.account_type() || TYPE_H5_QQ == req.account_type())
    {
        ret = qq_client.GetOuterProfile(req.account_type(), req.accesstoken(), req.openid(), outerProfile, req.appid());
    } else if (TYPE_XIAOMI == req.account_type())
    {
        ret = xiaomi_client.GetOuterProfile(req.accesstoken(), outerProfile, req.appid());
    }else if(TYPE_FACEBOOK == req.account_type())
    {
        ret = fackebookClient.GetOuterProfile(req.accesstoken(), outerProfile);
        if(!ret){
            string head_img_url;
            NORMAL("src_img_url[ %s ]", outerProfile.headimgurl().c_str());
            int head_ret = fackebookClient.GetLargerHeadImgUrl(outerProfile.uid(), head_img_url);
            if(!head_ret && !head_img_url.empty()){
                DEBUG("head_img_url[ %s ]", head_img_url.c_str());
                outerProfile.set_headimgurl(head_img_url);
            }
        }
        
    } else if(TYPE_GOOGLE == req.account_type()){
        ret = googleClient.GetOuterProfile(req.accesstoken(), outerProfile);
    }
    else
    {
        ERRORPB(req);
        return AccountError::LOGIC_PARAM_NEQ_ERROR;
    };
    if (ret)
    {
        ERROR("GetOuterProfile error. ret:%d, accountType:%d, token:%s, openid:%s", ret, req.account_type(),
              req.accesstoken().c_str(), req.openid().c_str());
    }
    ret = filterErrorCode(req.account_type(), ret);
    if (ret < 0)
    {
        ret = AccountError::LOGIN_GET_OAUTH_PROFILE_ERROR;
    }
    return ret;
}

int AccountOpenService::GetAccessTokenProcess(GetAccessTokenByCodeReq& req, OpenAccountInfo& openAccountInfo)
{
    int ret = 0;
    if (req.code().empty())
    {
        ERROR("empty code:[%s] ", req.code().c_str());
        return AccountError::LOGIC_PARAM_NEQ_ERROR;
    }
    if (TYPE_WEIXIN == req.account_type()
        || TYPE_H5_WEIXIN == req.account_type()
        || TYPE_DB_WEIXIN == req.account_type()
        || TYPE_DEBUG_WEIXIN == req.account_type()
        || TYPE_BLACK_GOLD_WEIXIN == req.account_type()
		|| TYPE_VANS_GAME_WEIXIN == req.account_type()) {
        ret = weixin_client.GetAccessToken(req.code(), openAccountInfo, req.appid(), req.account_type());
    } else if (TYPE_XIAOMI == req.account_type())
    {
        ret = xiaomi_client.GetAccessToken(req.code(), openAccountInfo, req.appid());
    }  else
    {
        ERROR("params invalid! type:%d, code:%s ", req.account_type(), req.code().c_str());
        return AccountError::LOGIC_PARAM_NEQ_ERROR;
    }
    if (ret)
    {
        ERROR("GetAccessTokenProcess error. ret:%d, accountType:%d, token:%s", ret, req.account_type(), req.code().c_str());
    }
    ret = filterErrorCode(req.account_type(), ret);
    if (ret < 0)
    {
        ret = AccountError::LOGIN_GET_OAUTH_ACCESSTOKEN_ERROR;
    }
    return ret;
}

/*用于instagram获取accesstoken和基本信息*/
int AccountOpenService::GetAccessTokenProcess(GetAccessTokenAndBaseInfoByCodeReq& req, OpenAccountInfo& openAccountInfo, OuterProfile& outer_profile)
{
    int ret = 0;
    if (req.code().empty())
    {
        ERROR("empty code:[%s] ", req.code().c_str());
        return AccountError::LOGIC_PARAM_NEQ_ERROR;
    }
    ret = instagramClient.GetAccessTokenAndBaseInfo(req.code(), openAccountInfo, outer_profile, req.appid());
    if (ret)
    {
        ERROR("GetAccessTokenProcess error. ret:%d, accountType:%d, token:%s", ret, req.account_type(), req.code().c_str());
    }
    ret = filterErrorCode(req.account_type(), ret);  //instagram的错误码采用http错误码标识，为了便于识别未进行过滤
    if (ret < 0)
    {
        ret = AccountError::LOGIN_GET_OAUTH_ACCESSTOKEN_ERROR;
    }
    return ret;
}

int AccountOpenService::filterErrorCode(int accountType, int ret)
{
    if(accountType == TYPE_WEIXIN){
        if (WX_INVALID_CODE_40029 == ret || WX_INVALID_CODE_40001 == ret) {
            return AccountError::LOGIC_INVALID_CODE_ERROR;
        }
    }else if(accountType == TYPE_SINAWEIBO){
        
    }else if(accountType == TYPE_QQ){
        if (QQ_INVALID_CODE_100016 == ret){
            return AccountError::LOGIC_INVALID_CODE_ERROR;
        }
    }else if(accountType == TYPE_XIAOMI){
        if (XM_INVALID_CODE_96013 == ret) {
            return AccountError::LOGIC_INVALID_CODE_ERROR;
        }
    }else if(accountType == TYPE_FACEBOOK){
        if (FB_INVALID_CODE == ret) {
            return AccountError::LOGIC_INVALID_CODE_ERROR;
        }
    }else if(accountType == TYPE_GOOGLE){
        if (GOOGLE_INVALID_CODE == ret) {
            return AccountError::LOGIC_INVALID_CODE_ERROR;
        }
    }
    return ret;
}
