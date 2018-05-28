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

#ifndef __DETAIL_HI_REDIS_CLIENT_H__
#define __DETAIL_HI_REDIS_CLIENT_H__

#include <string>
#include <vector>

struct redisContext;
struct redisReply;

namespace redisclient {
namespace detail {

class HiRedisReply
{
public:
    HiRedisReply();
    virtual ~HiRedisReply();

    void bind_reply(void * reply, bool is_root = true);
    
    int type();
    const char * get_str();
    size_t get_strlen();
    int32_t get_int32();
    int64_t get_int64();

    size_t get_array_size();
    HiRedisReply * get_element(int index);
    
private:
    redisReply * _reply;
    bool _is_root;
};

class HiRedisClient
{
public:
    HiRedisClient();
    virtual ~HiRedisClient();

    const char * get_error();

    const char * prefix();

    bool connect(const char * ip, int port, const char * prefix, int timeout_ms);
    void close();
    bool is_connected();

    bool auth(const std::string & passwd);

    bool set(const std::string & key, const std::string & value, int expire_sec);
    bool get(const std::string & key, std::string & value);
    bool del(const std::string & key, int * value);
    bool exists(const std::string & key, int * value);

    bool incr(const std::string & key, int * value);
    bool decr(const std::string & key, int * value);

    bool expire(const std::string & key, int sec, int * value);
    bool ttl(const std::string & key, int * value);
    bool persist(const std::string & key, int * value);

    HiRedisReply * exec_commandv(const char * format, va_list ap);
    HiRedisReply * exec_command_argv(int argc, const char ** argv, const size_t * argvlen);

private:
    bool expect_integer_reply(const char * cmd, const std::string & key, int * value);
    bool expect_integer_reply(std::vector<const char *> &  argv_arr, const std::vector<size_t> & argvlen_arr, int * value);

private:
    redisContext * _ctx;
    std::string _error_info; 
    std::string _prefix;
};

}   //namespace detail
}   //namespace redisclient

#include <RedisClient/detail/impl/HiRedisClient.h>

#endif    //__DETAIL_HI_REDIS_CLIENT_H__
