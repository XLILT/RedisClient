/*************************************************************************
    > File Name: impl/RedisClient.h
    > Author: mxl
    > Mail: xiaolongicx@gmail.com
    > Created Time: 2018-05-15 11:40
 ************************************************************************/

#ifndef __IMPL_REDIS_CLIENT_H__
#define __IMPL_REDIS_CLIENT_H__

#include <RedisClient/detail/HiRedisClient.h>

namespace redisclient {

inline RedisReply::RedisReply(detail::RedisReplyImpl * impl, bool is_root)
    :_impl(impl),
    _is_root(is_root),
    _sub_arr()
{}

inline RedisReply::~RedisReply()
{
    if(_is_root)
    {
        if(!_sub_arr.empty())
        {
            for(const auto & sub_reply : _sub_arr)
            {
                if(sub_reply)
                {
                    delete sub_reply;
                }
            }
        }
    
        if(_impl)
        {
            delete _impl;
            _impl = nullptr;
        }
    }
}

inline void RedisReply::bind_reply(void * reply)
{
    _impl->bind_reply(reply);
}

inline int RedisReply::type()
{
    return _impl->type();
}

const char * RedisReply::get_str()
{
    return _impl->get_str();
}

size_t RedisReply::get_strlen()
{
    return _impl->get_strlen();
}

int32_t RedisReply::get_int32()
{
    return _impl->get_int32();
}

int64_t RedisReply::get_int64()
{
    return _impl->get_int64();
}

size_t RedisReply::get_array_size()
{
    return _impl->get_array_size();
}

RedisReply * RedisReply::get_element(int index)
{
    if(index < 0 || static_cast<size_t>(index) >= get_array_size())
    {
        return nullptr;
    }

    if(_sub_arr.empty())
    {
        _sub_arr.resize(get_array_size(), nullptr);
    }

    if(!_sub_arr[index])
    {
        detail::RedisReplyImpl * reply_impl = _impl->get_element(index);

        if(!reply_impl)
        {
            return nullptr;
        }

        _sub_arr[index] = new RedisReply(reply_impl, false);
    }

    return _sub_arr[index];
}

inline RedisClient::RedisClient()
{
    _impl = new detail::RedisClientImpl();
}

inline RedisClient::~RedisClient()
{
    if(_impl)
    {
        delete _impl;
        _impl = nullptr;
    }
}

inline const char * RedisClient::get_error()
{
    return _impl->get_error();
}

inline const char * RedisClient::prefix()
{
    return _impl->prefix();
}

inline bool RedisClient::connect(const char * ip, int port, const char * prefix, int timeout_ms)
{
   return _impl->connect(ip, port, prefix, timeout_ms); 
}

inline void  RedisClient::close()
{
    _impl->close();
}

inline bool RedisClient::is_connected()
{
    return _impl->is_connected();
}

inline bool RedisClient::auth(const std::string & passwd)
{
    return _impl->auth(passwd);
}

inline bool RedisClient::set(const std::string & key, const std::string & value, int expire_sec)
{
    return _impl->set(key, value, expire_sec);
}

inline bool RedisClient::get(const std::string & key, std::string & value)
{
    return _impl->get(key, value);
}

inline bool RedisClient::del(const std::string & key, int * value)
{
    return _impl->del(key, value);
}

inline bool RedisClient::exists(const std::string & key, int * value)
{
    return _impl->exists(key, value);
}

inline bool RedisClient::incr(const std::string & key, int * value)
{
    return _impl->incr(key, value);
}

inline bool RedisClient::decr(const std::string & key, int * value)
{
    return _impl->decr(key, value);
}

inline bool RedisClient::expire(const std::string & key, int sec, int * value)
{
    return _impl->expire(key, sec, value);
}

inline bool RedisClient::ttl(const std::string & key, int * value)
{
    return _impl->ttl(key, value);
}

inline bool RedisClient::persist(const std::string & key, int * value)
{
    return _impl->persist(key, value);
}

inline SharedPtrRedisReply RedisClient::exec_commandv(const char * format, va_list ap)
{
    detail::RedisReplyImpl * reply_impl = _impl->exec_commandv(format, ap);

    return SharedPtrRedisReply(new RedisReply(reply_impl));
}

inline SharedPtrRedisReply RedisClient::exec_command(const char * format, ...)
{
    va_list ap;
    va_start(ap,format);
    
    SharedPtrRedisReply sharedreply = exec_commandv(format, ap);
    va_end(ap);

    return sharedreply;
}

inline SharedPtrRedisReply RedisClient::exec_command_argv(int argc, const char ** argv, const size_t * argvlen)
{
    detail::RedisReplyImpl * reply_impl =  _impl->exec_command_argv(argc, argv, argvlen);

    return SharedPtrRedisReply(new RedisReply(reply_impl));
}

}   //namespace redisclient

#endif  //__IMPL_REDIS_CLIENT_H__

