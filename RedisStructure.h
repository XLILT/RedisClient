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

#ifndef __REDISSTRUCTURE_H__
#define __REDISSTRUCTURE_H__

#include <vector>
#include <string>
#include <utility>
#include <RedisClient/RedisClient.h>

namespace redisclient {

using StringArray = std::vector<std::string>;
using MemberScorePair = std::pair<std::string, std::string>;
using MemberScoreArray = std::vector<MemberScorePair>;
using FieldValuePair = std::pair<std::string, std::string>;
using FieldValueArray = std::vector<FieldValuePair>;

class RedisStructure
{
public:
    RedisStructure();
    virtual ~RedisStructure();

    void bind_client(RedisClient * client);
    const char * get_error();

protected:
    bool expect_integer_reply(std::vector<const char *> &  argv_arr, const std::vector<size_t> & argvlen_arr, int * value);
    bool expect_string_reply(std::vector<const char *> &  argv_arr, const std::vector<size_t> & argvlen_arr, std::string * value);

protected:
    RedisClient * _client;
    std::string _error_info;
};

class RedisList : public RedisStructure
{
public:
    RedisList();
    virtual ~RedisList();

    bool lpush(const std::string & key, const std::string & value, int * len = nullptr);
    bool rpush(const std::string & key, const std::string & value, int * len = nullptr);
    bool lpop(const std::string & key, std::string * value = nullptr);
    bool rpop(const std::string & key, std::string * value = nullptr);

    bool lindex(const std::string & key, int index, std::string * value);

    bool llen(const std::string & key, int * len);
    SharedPtrRedisReply lrange(const std::string & key, int start, int stop);
};

class RedisSet : public RedisStructure
{
public:
    RedisSet();
    virtual ~RedisSet();

    bool sadd(const std::string & key, const StringArray & member_array, int * add_count = nullptr);
    bool srem(const std::string & key, const StringArray & member_array, int * rem_count = nullptr);
    SharedPtrRedisReply spop(const std::string & key, uint32_t count);

    bool sismember(const std::string & key, const std::string &member, int * is_member);
    SharedPtrRedisReply smembers(const std::string & key);
    bool scard(const std::string & key, int * len);    
};

class RedisZset : public RedisStructure
{
public:
    RedisZset();
    virtual ~RedisZset();

    bool zadd(const std::string & key, const MemberScoreArray & ms_array, int * add_count = nullptr);
    bool zincrby(const std::string & key, const std::string & increment, const std::string & member, std::string * score = nullptr);
    bool zrem(const std::string & key, const StringArray & member_array, int * rem_count = nullptr);
    bool zscore(const std::string & key, const std::string & member, std::string * score);
    bool zrank(const std::string & key, const std::string & member, int * rank);
    bool zrevrank(const std::string & key, const std::string & member, int * rank);

    bool zcard(const std::string & key, int * len);

    SharedPtrRedisReply zrange(const std::string & key, int start, int stop, bool withsscores = true);
    SharedPtrRedisReply zrevrange(const std::string & key, int start, int stop, bool withscores = true);
};

class RedisHash : public RedisStructure
{
public:
    RedisHash();
    virtual ~RedisHash();

    bool hset(const std::string & key, const std::string & field, const std::string & value, int * add_count = nullptr);
    bool hget(const std::string & key, const std::string & field, std::string * value);

    bool hmset(const std::string & key, const FieldValueArray & fv_array);
    SharedPtrRedisReply hmget(const std::string & key, const StringArray & field_array);

    bool hexists(const std::string & key, const std::string & field, int * is_exist);
    bool hdel(const std::string & key, const StringArray & field_array, int * del_count = nullptr);

    bool hincrby(const std::string & key, const std::string & field, int increment, int * value = nullptr);

    bool hlen(const std::string & key, int * len);

    SharedPtrRedisReply hkeys(const std::string & key);
    SharedPtrRedisReply hvals(const std::string & key);
    SharedPtrRedisReply hgetall(const std::string & key);
};

}   // namespace redisclient

#include <RedisClient/impl/RedisStructure.hpp>

#endif  // __REDISSTRUCTURE_H__

