/*
*  COPYRIGHT NOTICE
*  Xiaolong Ma
*  All rights reserved.
*
*  @version : 1.0
*  @author : mxl
*  @E-mail : xiaolongicx@gmail.com
*  @date : 2018/4/5
*
*  Revision Notes :
*/

#ifndef __DETAIL_IMPL_HI_REDIS_CLIENT_H__
#define __DETAIL_IMPL_HI_REDIS_CLIENT_H__

#include <hiredis/hiredis.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

#include <vector>

namespace redisclient {
namespace detail {

inline HiRedisReply::HiRedisReply()
    :_reply(nullptr),
    _is_root(false)
{}

inline HiRedisReply::~HiRedisReply()
{
    if(_reply)
    {
        freeReplyObject(_reply);
        _reply = nullptr;
    }
}

inline void HiRedisReply::bind_reply(void * reply, bool is_root)
{
    _is_root = is_root;
    _reply = static_cast<redisReply *>(reply);
}

inline int HiRedisReply::type()
{
    return _reply->type; 
}

inline const char * HiRedisReply::get_str()
{
    return _reply->str;
}

inline size_t HiRedisReply::get_strlen()
{
    return _reply->len; 
}

inline int32_t HiRedisReply::get_int32()
{
    return static_cast<int32_t>(_reply->integer);
}

inline int64_t HiRedisReply::get_int64()
{
    return _reply->integer;
}

inline size_t HiRedisReply::get_array_size()
{
    return _reply->elements;
}

inline HiRedisReply * HiRedisReply::get_element(int index)
{
    HiRedisReply * reply = new HiRedisReply();
    reply->bind_reply(*(_reply->element + index), false);

    return reply;
}

inline HiRedisClient::HiRedisClient()
    :_ctx(nullptr),
     _error_info(),
    _prefix()
{}

inline HiRedisClient::~HiRedisClient()
{
    close();
}

inline const char * HiRedisClient::get_error()
{
    return _error_info.c_str();
}

inline const char * HiRedisClient::prefix()
{
    return _prefix.c_str();
}

inline bool HiRedisClient::connect(const char * ip, int port, const char * prefix, int timeout_ms)
{
	bool ret = true;

    int second = timeout_ms / 1000;
    int millisecond = timeout_ms % 1000;
    struct timeval timeout = { second, millisecond * 1000 };

    close();   

    _ctx = redisConnectWithTimeout(ip, port, timeout);

	if (_ctx == NULL || _ctx->err) {
        if (_ctx) {
            _error_info = _ctx->errstr;
            
            redisFree(_ctx);
            _ctx = nullptr;

            ret = false;
        } else {
            _error_info = "Connection error: can't allocate redis context";

            ret = false;
        }
    }

    _prefix = prefix;

    return ret;
}

void HiRedisClient::close()
{
    if(_ctx)
    {
        redisFree(_ctx);
        _ctx = nullptr;
    }
}

bool HiRedisClient::is_connected()
{
    if(!_ctx)
    {
        return false;
    }

    redisReply * reply = static_cast<redisReply *>(redisCommand(_ctx, "PING"));
    if(reply && reply->type == REDIS_REPLY_STATUS && 0 == strcasecmp(reply->str, "PONG"))
    {
        return true;
    }

    return false;
}

inline bool HiRedisClient::auth(const std::string & passwd)
{
    bool ret = true;

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "AUTH";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    argv_arr.emplace_back(passwd.c_str());
    argvlen_arr.emplace_back(passwd.size());

    redisReply * reply = static_cast<redisReply *>(redisCommandArgv(_ctx, argv_arr.size(), &argv_arr[0], &argvlen_arr[0]));

    do
    {
        if(!reply)
        {
            ret = false;
            _error_info = "reply is null";
            break;
        }

        if(reply->type == REDIS_REPLY_ERROR)
        {
            ret = false;

            _error_info = reply->str;
            break;                
        }

        if(reply->type != REDIS_REPLY_STATUS)
        {
            ret = false;

            char error_str[128] = {0};
            snprintf(error_str, sizeof(error_str), "reply type is not REDIS_REPLY_STATUS but %d", reply->type);

            _error_info = error_str;
            break;                
        }
    }while(0);
       
    return ret;
}

bool HiRedisClient::set(const std::string & key, const std::string & value, int expire_sec)
{
    bool ret = true;

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "SET";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    argv_arr.emplace_back(value.data());
    argvlen_arr.emplace_back(value.size());

    if(expire_sec > 0)
    {
        argv_arr.emplace_back("EX");
        argvlen_arr.emplace_back(strlen("EX"));

        char num_str[16] = {0};
        snprintf(num_str, sizeof(num_str), "%d", expire_sec);

        argv_arr.emplace_back(num_str);
        argvlen_arr.emplace_back(strlen(num_str));
    }

    redisReply * reply = static_cast<redisReply *>(redisCommandArgv(_ctx, argv_arr.size(), &argv_arr[0], &argvlen_arr[0]));

    do
    {
        if(!reply)
        {
            ret = false;
            _error_info = "reply is null";
            break;
        }

        if(reply->type == REDIS_REPLY_ERROR)
        {
            ret = false;

            _error_info = reply->str;
            break;                
        }

        if(reply->type != REDIS_REPLY_STATUS)
        {
            ret = false;

            char error_str[128] = {0};
            snprintf(error_str, sizeof(error_str), "reply type is not REDIS_REPLY_STATUS but %d", reply->type);

            _error_info = error_str;
            break;                
        }
    }while(0);
       
    return ret;
}

inline bool HiRedisClient::get(const std::string & key, std::string & value)
{
    bool ret = true;

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "GET";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    redisReply * reply = static_cast<redisReply *>(redisCommandArgv(_ctx, argv_arr.size(), &argv_arr[0], &argvlen_arr[0]));

    do
    {
        if(!reply)
        {
            ret = false;
            _error_info = "reply is null";
            break;
        }
        
        if(reply->type == REDIS_REPLY_ERROR)
        {
            ret = false;

            _error_info = reply->str;
            break;                
        }
        
        if(reply->type != REDIS_REPLY_STRING)
        {
            ret = false;
            
            char error_str[128] = {0};
            snprintf(error_str, sizeof(error_str), "reply type is not REDIS_REPLY_STRING but %d", reply->type);

            _error_info = error_str;
            break;                
        }

        value.assign(reply->str, reply->len);
    }while(0);
    
    return ret;
}

inline bool HiRedisClient::del(const std::string & key, int * value)
{
    return expect_integer_reply("DEL", key, value);
}

inline bool HiRedisClient::exists(const std::string & key, int * value)
{
    return expect_integer_reply("EXISTS", key, value);
}

inline bool HiRedisClient::incr(const std::string & key, int * value)
{
    return expect_integer_reply("INCR", key, value);
}

inline bool HiRedisClient::decr(const std::string & key, int * value)
{
    return expect_integer_reply("DECR", key, value);
}

inline bool HiRedisClient::expire(const std::string & key, int sec, int * value)
{
    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char *cmd = "EXPIRE";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    char num_str[16] = {0};
    snprintf(num_str, sizeof(num_str), "%d", sec);

    argv_arr.emplace_back(num_str);
    argvlen_arr.emplace_back(strlen(num_str));

    return expect_integer_reply(argv_arr, argvlen_arr, value);
}

inline bool HiRedisClient::ttl(const std::string & key, int * value)
{
    return expect_integer_reply("TTL", key, value);
}

inline bool HiRedisClient::persist(const std::string & key, int * value)
{
    return expect_integer_reply("PERSIST", key, value);
}

inline bool HiRedisClient::expect_integer_reply(const char * cmd, const std::string & key, int * value)
{
    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    return expect_integer_reply(argv_arr, argvlen_arr, value);
}

bool HiRedisClient::expect_integer_reply(std::vector<const char *> &  argv_arr, const std::vector<size_t> & argvlen_arr, int * value)
{
    bool ret = true;

    redisReply * reply = static_cast<redisReply *>(redisCommandArgv(_ctx, argv_arr.size(), &argv_arr[0], &argvlen_arr[0]));

    do
    {
        if(!reply)
        {
            ret = false;
            _error_info = "reply is null";
            break;
        }

        if(reply->type == REDIS_REPLY_ERROR)
        {
            ret = false;

            _error_info = reply->str;
            break;                
        }

        if(reply->type != REDIS_REPLY_INTEGER)
        {
            ret = false;
            
            char error_str[128] = {0};
            snprintf(error_str, sizeof(error_str), "reply type is not REDIS_REPLY_INTEGER but %d", reply->type);

            _error_info = error_str;
            break;                
        }

        if(value)
        {
            *value = reply->integer;
        }
    }while(0);
    
    return ret;
}

HiRedisReply * HiRedisClient::exec_commandv(const char * format, va_list ap)
{
    redisReply * reply = static_cast<redisReply *>(redisvCommand(_ctx, format, ap));
    HiRedisReply * hireply = new HiRedisReply();
    hireply->bind_reply(reply);

    return hireply;
}

inline HiRedisReply * HiRedisClient::exec_command_argv(int argc, const char ** argv, const size_t * argvlen)
{
    redisReply * reply = static_cast<redisReply *>(redisCommandArgv(_ctx, argc, argv, argvlen));
    HiRedisReply * hireply = new HiRedisReply();
    hireply->bind_reply(reply);

    return hireply;
}

}   //namespace detail
}   //namespace redisclient

#endif    //__DETAIL_IMPL_HI_REDIS_CLIENT_H__

