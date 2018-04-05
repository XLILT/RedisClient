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

namespace redisclient {

class RedisReplyImpl;
class RedisClientImpl;

class RedisReply
{
public:
    RedisReply() {}
    virtual ~RedisReply() {}

    int bind_reply(void * reply);
    
    int type();
    const char * get_str();
    int get_strlen();
    int64_t get_int64();

    size_t get_array_size();
    RedisReply * get_element(int index);

private:
    RedisReplyImpl * _impl;
};

class RedisClient
{
public:
    RedisClient() {}
    virtual ~RedisClient() {}

    int connect(const char &ip, int port = 6379, const char * prefix = "");
    void close();
    bool is_connected();

    int set(const char * key, const char * value, int expire_sec);
    int get(const char *key, RedisReply & reply);
    int del(const char * key);
    int inc(const char * key);
    int dec(const char * key);
    int expire(const char * key , int sec);
    int ttl(const char * key);
    int persist(const char * key);
    int exists(const char * key);

    int exec_command(RedisReply & reply, int argc, const char ** argv, const size_t argvlen);

pravite:
    RedisClientImpl * _impl;

};

}   // namespace redisclient

#endif  // __REDISCLIENT_H__

