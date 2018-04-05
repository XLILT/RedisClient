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

namespace redisclient {

class RedisClient;
class RedisReply;

class RedisStructure
{
public:
    RedisStructure() {}
    virtual ~RedisStructure() {}

    int bind_client(RedisClient * client);

private:
    RedisClient * _client;
};

class RedisList : public RedisStructure
{
public:
    RedisList() {}
    virtual ~RedisList() {}

    int push_front(const char * key, const char * value);
    int push_back(const char * key, const char * value);
    int pop_front(const char *key, RedisReply * reply = nullptr);
    int pop_back(const char * key, RedisReply * reply = nullptr);

    int get_item(const char * key, int index, RedisReply * reply);

    int length(const char * key);
    int range(const char * key, int start, int end, RedisReply * reply);
};

class RedisSet : public RedisStructure
{
public:
    RedisSet() {}
    virtual ~RedisSet() {}

    int add(const char * key, const char * value);
    int rem(const char * key, const char * value);
    int pop(const char * key, RedisReply * reply);

    int is_member(const char * key, const char * value);
    int members(const char * key, RedisReply * reply);

    int length(const char * key);    
};

class RedisZset : public RedisStructure
{
public:
    RedisZset() {}
    virtual ~RedisZset() {}

    int zadd(const char * key, float score, const char * member);
    int zincby(const char * key, float increment, const char * member);
    int zrem(const char * key, const char * member);

    int zremrangebyrank(const char * key, int start, int end);
    int zrange(const char * key, int start, int end, RedisReply * reply);
};

class RedisHash : public RedisStructure
{
public:
    RedisHash() {}
    virtual ~RedisHash() {}

    int set(const char * key, const char * field, const char * value);
    int get(const char * key, const char * field, RedisReply * reply);
    int mset(const char * key, const std::vector<std::string> & fileds, std::vector<RedisReply>);
    int mget(const char * key, const std::vector<std::string> & fileds, std::vector<RedisReply>);

    int exist(const char * key, const char * field);
    int del(const char * key, const char * field);
    int incrby(const char * key, const char *field, )

    int get_all_fileds(const char * key, std::vector<std::string> & fileds);
    int get_all_values(const char * key, std::vector<RedisReply>);
    int get_all(const char * key, std::vector<std::string> & fileds, std::vector<RedisReply>);

    int length(const char * key);   
};

}   // namespace redisclient

#endif  // __REDISSTRUCTURE_H__

