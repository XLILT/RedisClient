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

#ifndef __IMPL_REDIS_STRUCTURE_H__
#define __IMPL_REDIS_STRUCTURE_H__

#include <string.h>

namespace redisclient {

inline RedisStructure::RedisStructure()
    :_client(nullptr),
    _error_info()
{}

inline RedisStructure::~RedisStructure()
{}

inline void RedisStructure::bind_client(RedisClient * client)
{
    _client = client;
}

inline const char * RedisStructure::get_error()
{
    return _error_info.c_str();
}

inline bool RedisStructure::expect_integer_reply(std::vector<const char *> &  argv_arr, const std::vector<size_t> & argvlen_arr, int * value)
{
    bool ret = true;

    SharedPtrRedisReply reply = _client->exec_command_argv(argv_arr.size(), &argv_arr[0], &argvlen_arr[0]);

    do
    {
        if(!reply)
        {
            ret = false;
            
            _error_info = "reply is null";
           break; 
        } 

        if(reply->type() == RedisReply::REPLY_ERROR)
        {
            ret = false;

            _error_info = reply->get_str();
            break;
        }

        if(reply->type() != RedisReply::REPLY_INTEGER)
        {
            ret = false;

            char error_str[128] = {0};
            snprintf(error_str, sizeof(error_str), "reply type is not REPLY_INTEGER but %d", reply->type());

            _error_info = error_str;
            break;
        }

        if(value)
        {
            *value = reply->get_int32();
        }
    }while(0);
   
    return ret;
}

inline bool RedisStructure::expect_string_reply(std::vector<const char *> &  argv_arr, const std::vector<size_t> & argvlen_arr, std::string * value)
{
    bool ret = true;

    SharedPtrRedisReply reply = _client->exec_command_argv(argv_arr.size(), &argv_arr[0], &argvlen_arr[0]);

    do
    {
        if(!reply)
        {
            ret = false;
            
            _error_info = "reply is null";
           break; 
        } 

        if(reply->type() == RedisReply::REPLY_ERROR)
        {
            ret = false;

            _error_info = reply->get_str();
            break;
        }

        if(reply->type() != RedisReply::REPLY_STRING)
        {
            ret = false;

            char error_str[128] = {0};
            snprintf(error_str, sizeof(error_str), "reply type is not REPLY_STRING but %d", reply->type());

            _error_info = error_str;
            break;
        }

        if(value)
        {
            value->assign(reply->get_str(), reply->get_strlen());
        }
    }while(0);
   
    return ret;
}

inline RedisList::RedisList()
{}

inline RedisList::~RedisList()
{}

inline bool RedisList::lpush(const std::string & key, const std::string & value, int * len)
{
    if(!_client)
    {
        return false;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "LPUSH";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));
    
    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    argv_arr.emplace_back(value.c_str());
    argvlen_arr.emplace_back(value.size());      
   
    return expect_integer_reply(argv_arr, argvlen_arr, len);
}

inline bool RedisList::rpush(const std::string & key, const std::string & value, int * len)
{
    if(!_client)
    {
        return false;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "RPUSH";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    argv_arr.emplace_back(value.c_str());
    argvlen_arr.emplace_back(value.size());      
   
    return expect_integer_reply(argv_arr, argvlen_arr, len);
}

inline bool RedisList::lpop(const std::string & key, std::string * value)
{
    if(!_client)
    {
        return false;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "LPOP";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));
    
    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    return expect_string_reply(argv_arr, argvlen_arr, value);
}

inline bool RedisList::rpop(const std::string & key, std::string * value)
{
    if(!_client)
    {
        return false;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "RPOP";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));
    
    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    return expect_string_reply(argv_arr, argvlen_arr, value);
}

inline bool RedisList::lindex(const std::string & key, int index, std::string * value)
{
    if(!_client)
    {
        return false;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "LINDEX";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    char num_str[16] = {0};
    snprintf(num_str, sizeof(num_str), "%d", index);

    argv_arr.emplace_back(num_str);
    argvlen_arr.emplace_back(strlen(num_str));

    return expect_string_reply(argv_arr, argvlen_arr, value);
}

inline bool RedisList::llen(const std::string & key, int * len)
{
    if(!_client)
    {
        return false;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "LLEN";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    return expect_integer_reply(argv_arr, argvlen_arr, len);
}

inline SharedPtrRedisReply RedisList::lrange(const std::string & key, int start, int stop)
{
    if(!_client)
    {
        return nullptr;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "LRANGE";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    char start_str[16] = {0};
    snprintf(start_str, sizeof(start_str), "%d", start);

    argv_arr.emplace_back(start_str);
    argvlen_arr.emplace_back(strlen(start_str));

    char stop_str[16] = {0};
    snprintf(stop_str, sizeof(stop_str), "%d", stop);

    argv_arr.emplace_back(stop_str);
    argvlen_arr.emplace_back(strlen(stop_str));

    return _client->exec_command_argv(argv_arr.size(), &argv_arr[0], &argvlen_arr[0]);
}

inline RedisSet::RedisSet()
{}

inline RedisSet::~RedisSet()
{}

inline bool RedisSet::sadd(const std::string & key, const StringArray & member_array, int * add_count)
{
    if(!_client)
    {
        return false;
    }

    if(member_array.empty())
    {
        return true;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "SADD";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    for(const auto & member : member_array)
    {
        argv_arr.emplace_back(member.c_str());
        argvlen_arr.emplace_back(member.size());
    }

    return expect_integer_reply(argv_arr, argvlen_arr, add_count);
}

inline bool RedisSet::srem(const std::string & key, const StringArray & member_array, int * add_count)
{
    if(!_client)
    {
        return false;
    }

    if(member_array.empty())
    {
        return true;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "SREM";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    for(const auto & member : member_array)
    {
        argv_arr.emplace_back(member.c_str());
        argvlen_arr.emplace_back(member.size());
    }

    return expect_integer_reply(argv_arr, argvlen_arr, add_count);
}

inline SharedPtrRedisReply RedisSet::spop(const std::string & key, uint32_t count)
{
    if(!_client)
    {
        return nullptr;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "SPOP";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    char num_str[16] = {0};
    snprintf(num_str, sizeof(num_str), "%d", count);

    argv_arr.emplace_back(num_str);
    argvlen_arr.emplace_back(strlen(num_str));

    return _client->exec_command_argv(argv_arr.size(), &argv_arr[0], &argvlen_arr[0]);
}

inline bool RedisSet::sismember(const std::string & key, const std::string &member, int * is_member)
{
    if(!_client)
    {
        return false;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "SISMEMBER";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    argv_arr.emplace_back(member.c_str());
    argvlen_arr.emplace_back(member.size());

    return expect_integer_reply(argv_arr, argvlen_arr, is_member);
}

inline SharedPtrRedisReply RedisSet::smembers(const std::string & key)
{
    if(!_client)
    {
        return nullptr;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "SMEMBERS";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    return _client->exec_command_argv(argv_arr.size(), &argv_arr[0], &argvlen_arr[0]);
}

inline bool RedisSet::scard(const std::string & key, int * len)
{
    if(!_client)
    {
        return false;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "SCARD";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    return expect_integer_reply(argv_arr, argvlen_arr, len);
}

inline RedisZset::RedisZset()
{}

inline RedisZset::~RedisZset()
{}

inline bool RedisZset::zadd(const std::string & key, const MemberScoreArray & ms_array, int * add_count)
{
    if(!_client)
    {
        return false;
    }

    if(ms_array.empty())
    {
        return true;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "ZADD";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    for(const auto & ms_pair : ms_array)
    {

        argv_arr.emplace_back(ms_pair.second.c_str());
        argvlen_arr.emplace_back(ms_pair.second.size());

        argv_arr.emplace_back(ms_pair.first.c_str());
        argvlen_arr.emplace_back(ms_pair.first.size());
    }

    return expect_integer_reply(argv_arr, argvlen_arr, add_count);
}

inline bool RedisZset::zincrby(const std::string & key, const std::string & increment, const std::string & member, std::string * score)
{
    if(!_client)
    {
        return false;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "ZINCRBY";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    argv_arr.emplace_back(increment.c_str());
    argvlen_arr.emplace_back(increment.size());

    argv_arr.emplace_back(member.c_str());
    argvlen_arr.emplace_back(member.size());

    return expect_string_reply(argv_arr, argvlen_arr, score);
}

inline bool RedisZset::zrem(const std::string & key, const StringArray & member_array, int * rem_count)
{
    if(!_client)
    {
        return false;
    }

    if(member_array.empty())
    {
        return true;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "ZREM";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    for(const auto & member : member_array)
    {
        argv_arr.emplace_back(member.c_str());
        argvlen_arr.emplace_back(member.size());
    }

    return expect_integer_reply(argv_arr, argvlen_arr, rem_count);
}

inline bool RedisZset::zscore(const std::string & key, const std::string & member, std::string * score)
{
    if(!_client)
    {
        return false;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "ZSCORE";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    argv_arr.emplace_back(member.c_str());
    argvlen_arr.emplace_back(member.size());

    return expect_string_reply(argv_arr, argvlen_arr, score);
}

inline bool RedisZset::zrank(const std::string & key, const std::string & member, int * rank)
{
    if(!_client)
    {
        return false;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "ZRANK";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    argv_arr.emplace_back(member.c_str());
    argvlen_arr.emplace_back(member.size());

    return expect_integer_reply(argv_arr, argvlen_arr, rank);
}

inline bool RedisZset::zrevrank(const std::string & key, const std::string & member, int * rank)
{
    if(!_client)
    {
        return false;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "ZREVRANK";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    argv_arr.emplace_back(member.c_str());
    argvlen_arr.emplace_back(member.size());

    return expect_integer_reply(argv_arr, argvlen_arr, rank);
}

inline bool RedisZset::zcard(const std::string & key, int * len)
{
    if(!_client)
    {
        return false;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "ZCARD";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    return expect_integer_reply(argv_arr, argvlen_arr, len);
}

inline SharedPtrRedisReply RedisZset::zrange(const std::string & key, int start, int stop, bool withscores)
{
    if(!_client)
    {
        return nullptr;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "ZRANGE";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    char start_str[16] = {0};
    snprintf(start_str, sizeof(start_str), "%d", start);

    argv_arr.emplace_back(start_str);
    argvlen_arr.emplace_back(strlen(start_str));

    char stop_str[16] = {0};
    snprintf(stop_str, sizeof(stop_str), "%d", stop);

    argv_arr.emplace_back(stop_str);
    argvlen_arr.emplace_back(strlen(stop_str));

    if(withscores)
    {
        const char * withscoresstr = "WITHSCORES";

        argv_arr.emplace_back(withscoresstr );
        argvlen_arr.emplace_back(strlen(withscoresstr));
    }

    return _client->exec_command_argv(argv_arr.size(), &argv_arr[0], &argvlen_arr[0]);
}

inline SharedPtrRedisReply RedisZset::zrevrange(const std::string & key, int start, int stop, bool withscores)
{
    if(!_client)
    {
        return nullptr;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "ZREVRANGE";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    char start_str[16] = {0};
    snprintf(start_str, sizeof(start_str), "%d", start);

    argv_arr.emplace_back(start_str);
    argvlen_arr.emplace_back(strlen(start_str));

    char stop_str[16] = {0};
    snprintf(stop_str, sizeof(stop_str), "%d", stop);

    argv_arr.emplace_back(stop_str);
    argvlen_arr.emplace_back(strlen(stop_str));

    if(withscores)
    {
        const char * withscoresstr = "WITHSCORES";

        argv_arr.emplace_back(withscoresstr );
        argvlen_arr.emplace_back(strlen(withscoresstr));
    }

    return _client->exec_command_argv(argv_arr.size(), &argv_arr[0], &argvlen_arr[0]);
}

inline RedisHash::RedisHash()
{}

inline RedisHash::~RedisHash()
{}

bool RedisHash::hset(const std::string & key, const std::string & field, const std::string & value, int * add_count)
{
    if(!_client)
    {
        return false;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "HSET";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    argv_arr.emplace_back(field.c_str());
    argvlen_arr.emplace_back(field.size());

    argv_arr.emplace_back(value.c_str());
    argvlen_arr.emplace_back(value.size());

    return expect_integer_reply(argv_arr, argvlen_arr, add_count);
}

bool RedisHash::hget(const std::string & key, const std::string & field, std::string * value)
{
    if(!_client)
    {
        return false;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "HGET";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    argv_arr.emplace_back(field.c_str());
    argvlen_arr.emplace_back(field.size());

    return expect_string_reply(argv_arr, argvlen_arr, value);
}

bool RedisHash::hmset(const std::string & key, const FieldValueArray & fv_array)
{
    if(!_client)
    {
        return false;
    }

    if(fv_array.empty())
    {
        return true;
    }

    bool ret = true;

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "HMSET";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    for(const auto & fv_pair : fv_array)
    {
        argv_arr.emplace_back(fv_pair.first.c_str());
        argvlen_arr.emplace_back(fv_pair.first.size());
        
        argv_arr.emplace_back(fv_pair.second.c_str());
        argvlen_arr.emplace_back(fv_pair.second.size());
    }

    SharedPtrRedisReply reply = _client->exec_command_argv(argv_arr.size(), &argv_arr[0], &argvlen_arr[0]);

    do
    {
        if(!reply)
        {
            ret = false;
            
            _error_info = "reply is null";
           break; 
        } 

        if(reply->type() == RedisReply::REPLY_ERROR)
        {
            ret = false;

            _error_info = reply->get_str();
            break;
        }

        if(reply->type() != RedisReply::REPLY_STATUS)
        {
            ret = false;

            char error_str[128] = {0};
            snprintf(error_str, sizeof(error_str), "reply type is not REPLY_STATUS but %d", reply->type());

            _error_info = error_str;
            break;
        }
    }while(0);
   
    return ret;
}

SharedPtrRedisReply RedisHash::hmget(const std::string & key, const StringArray & field_array)
{
    if(!_client)
    {
        return nullptr;
    }

    if(field_array.empty())
    {
        return nullptr;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "HMGET";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    for(const auto & field : field_array)
    {
        argv_arr.emplace_back(field.c_str());
        argvlen_arr.emplace_back(field.size());
    }

    return _client->exec_command_argv(argv_arr.size(), &argv_arr[0], &argvlen_arr[0]);
}

bool RedisHash::hexists(const std::string & key, const std::string & field, int * is_exist)
{
    if(!_client)
    {
        return false;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "HEXISTS";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    argv_arr.emplace_back(field.c_str());
    argvlen_arr.emplace_back(field.size());

    return expect_integer_reply(argv_arr, argvlen_arr, is_exist);
}

bool RedisHash::hdel(const std::string & key, const StringArray & field_array, int * del_count)
{
    if(!_client)
    {
        return false;
    }

    if(field_array.empty())
    {
        return true;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "HDEL";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    for(const auto & field : field_array)
    {
        argv_arr.emplace_back(field.c_str());
        argvlen_arr.emplace_back(field.size());
    }
   
    return expect_integer_reply(argv_arr, argvlen_arr, del_count);
}

bool RedisHash::hincrby(const std::string & key, const std::string & field, int increment, int * value)
{
    if(!_client)
    {
        return false;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "HINCRBY";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    argv_arr.emplace_back(field.c_str());
    argvlen_arr.emplace_back(field.size());

    char num_str[16] = {0};
    snprintf(num_str, sizeof(num_str), "%d", increment);

    argv_arr.emplace_back(num_str);
    argvlen_arr.emplace_back(strlen(num_str));

    return expect_integer_reply(argv_arr, argvlen_arr, value);
}

bool RedisHash::hlen(const std::string & key, int * len)
{
    if(!_client)
    {
        return false;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "HLEN";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    return expect_integer_reply(argv_arr, argvlen_arr, len);
}

SharedPtrRedisReply RedisHash::hkeys(const std::string & key)
{
    if(!_client)
    {
        return nullptr;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "HKEYS";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    return _client->exec_command_argv(argv_arr.size(), &argv_arr[0], &argvlen_arr[0]);
}

SharedPtrRedisReply RedisHash::hvals(const std::string & key)
{
    if(!_client)
    {
        return nullptr;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "HVALS";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    return _client->exec_command_argv(argv_arr.size(), &argv_arr[0], &argvlen_arr[0]);
}

SharedPtrRedisReply RedisHash::hgetall(const std::string & key)
{
    if(!_client)
    {
        return nullptr;
    }

    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "HGETALL";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    std::string real_key = _client->prefix();
    real_key += key;

    argv_arr.emplace_back(real_key.c_str());
    argvlen_arr.emplace_back(real_key.size());

    return _client->exec_command_argv(argv_arr.size(), &argv_arr[0], &argvlen_arr[0]);
}

}   //namespace redisclient

#endif  //__IMPL_REDIS_STRUCTURE_H__

