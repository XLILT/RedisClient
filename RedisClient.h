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

#ifndef __REDISCLIENT_H__
#define __REDISCLIENT_H__

#include <inttypes.h>
#include <stddef.h>
#include <string>
#include <memory>
#include <vector>

#include <RedisClient/detail/HiRedisClientForward.h>

namespace redisclient {

namespace detail {

typedef HiRedisReply RedisReplyImpl; 

typedef HiRedisClient RedisClientImpl; 

}   //namespace detail

class RedisReply
{
public:
    enum ReplyType
    {
        REPLY_STRING = 1,
        REPLY_ARRAY = 2,
        REPLY_INTEGER = 3,
        REPLY_NIL = 4,
        REPLY_STATUS = 5,
        REPLY_ERROR = 6,
    };

public:
    explicit RedisReply(detail::RedisReplyImpl * impl, bool is_root = true);

    ~RedisReply();

    //void bind_reply(void * reply);
    
    int type();

    const char * get_str();
    size_t get_strlen();
    int32_t get_int32();
    int64_t get_int64();

    size_t get_array_size();
    RedisReply * get_element(int index);

private:
    detail::RedisReplyImpl * _impl;

    bool _is_root;
    std::vector<RedisReply *> _sub_arr;
};

using SharedPtrRedisReply = std::shared_ptr<RedisReply>;

class RedisClient
{
public:
    RedisClient(const char * ip, int port = 6379, const char * prefix = "", int timeout_ms = 1500); 
    virtual ~RedisClient();

    const char * get_error();

    const char * prefix();

    bool auth(const std::string & passwd);

    bool set(const std::string & key, const std::string & value, int expire_sec = 0);
    bool get(const std::string & key, std::string * value);
    bool del(const std::string & key, int * value = nullptr);
    bool exists(const std::string & key, int * value);

    bool incr(const std::string & key, int * value = nullptr);
    bool decr(const std::string & key, int * value = nullptr);

    bool expire(const std::string & key, int sec, int * value = nullptr);
    bool ttl(const std::string & key, int * value);
    bool persist(const std::string & key, int * value = nullptr);

    SharedPtrRedisReply exec_commandv(const char * format, va_list ap);
    SharedPtrRedisReply exec_command(const char * format, ...);
    SharedPtrRedisReply exec_command_argv(int argc, const char ** argv, const size_t * argvlen);

protected:
    //bool connect(const char * ip, int port = 6379, const char * prefix = "", int timeout_ms = 1500);
    //void close();
    //bool is_connected();

protected:
    detail::RedisClientImpl * _impl;
};

}   // namespace redisclient

#include <RedisClient/impl/RedisClient.hpp>

#endif  // __REDISCLIENT_H__

