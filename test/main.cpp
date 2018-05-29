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

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>
#include <inttypes.h>
#include <string.h>

#include <RedisClient/RedisClient.h>
#include <RedisClient/RedisStructure.h>

using namespace std;
using namespace redisclient;

const char * host = "127.0.0.1";
int port = 6379;

//void test_conn()
//{
//    RedisClient client(host, port);
//    assert(!client.is_connected());
//    
//    assert(std::string(client.get_error()).empty());
//    assert(client.is_connected());
//
//    assert(!client.connect(host, 1));
//    assert(!std::string(client.get_error()).empty());
//}

//void test_close_is_connected()
//{
//    RedisClient client(host, port);
//    
//    assert(std::string(client.get_error()).empty());
//
//    assert(client.is_connected());
//    
//    assert(!client.is_connected());
//}

void test_set()
{
    RedisClient client(host, port);
    

    char buf[10] = {0};
    assert(client.set("test_set_k", std::string(buf, sizeof(buf)), 10));
    assert(client.set("test_set_k1", "1 2 3", 10));
}

void test_auth()
{
    RedisClient client("192.168.0.49", 20806);

    assert(client.auth("test|test"));
}

void test_get()
{
    RedisClient client(host, port, "test:");

    assert(client.set("test_get_k1", "1", 60));

    //std::string tmp_str;
    //std::cin >> tmp_str;

    std::string ret_str;

    //if(!client.get("test_get_k1", &ret_str))
    //{
    //    std::cout << client.get_error() << std::endl;
    //}

    assert(client.get("test_get_k1", &ret_str));
    assert(ret_str == "1");

    char binary_buf[] = {1, 2, 3};
    assert(client.set("test_get_k2", std::string(binary_buf, sizeof(binary_buf)), 60));
    assert(client.get("test_get_k2", &ret_str));
    assert(ret_str.size() == sizeof(binary_buf));
    for(size_t i = 0; i < ret_str.size(); i++)
    {
        assert(binary_buf[i] == ret_str[i]);
    }

    client.del("test_get_k1"); 
    client.del("test_get_k2"); 
}

void test_prefix()
{
    RedisClient client(host, port, "prefix1");

    client.del("test_prefix_k1");

    assert(client.set("test_prefix_k1", "1 2 3", 10));
    std::string ret_str;
    assert(client.get("test_prefix_k1", &ret_str));
    assert(ret_str == "1 2 3");

    RedisClient client2(host, port, "prefix2");

    client2.del("test_prefix_k1");

    assert(client2.set("test_prefix_k1", "321", 10));
    assert(client2.get("test_prefix_k1", &ret_str));
    assert(ret_str == "321");

    client.del("test_prefix_k1");
    client2.del("test_prefix_k1");
    
}

void test_del()
{
    RedisClient client(host, port);
    

    assert(client.set("test_del_k1", "1", 60));
    int ret = -1;
    assert(client.del("test_del_k1", &ret));
    assert(1 == ret);
    assert(client.del("test_del_k2", &ret));
    assert(0 == ret);

    assert(client.del("test_del_k3", nullptr));

    
}

void test_exists()
{
    RedisClient client(host, port);
    

    assert(client.set("test_exists_k1", "1", 60));
    int ret = -1;
    assert(client.exists("test_exists_k1", &ret));
    assert(1 == ret);
    assert(client.del("test_exists_k1", &ret));
    assert(client.exists("test_exists_k1", &ret));
    assert(0 == ret);

    
}

void test_incr()
{
    RedisClient client(host, port);
    

    int ret = -1;
    assert(client.incr("test_incr_k1", &ret));
    assert(ret == 1);
    assert(client.incr("test_incr_k1", &ret));
    assert(ret == 2);

    assert(client.del("test_incr_k1", nullptr));

    
}

void test_decr()
{
    RedisClient client(host, port);
    

    int ret = -1;
    assert(client.decr("test_decr_k1", &ret));
    assert(ret == -1);
    assert(client.decr("test_decr_k1", &ret));
    assert(ret == -2);

    assert(client.del("test_decr_k1", nullptr));

    
}

void test_expire_ttl_presist()
{
    RedisClient client(host, port);
    

    assert(client.set("test_expire_ttl_presist_k1", "1"));
    int ret = 0;
    assert(client.ttl("test_expire_ttl_presist_k2", &ret));
    assert(ret == -2);
    assert(client.ttl("test_expire_ttl_presist_k1", &ret));
    assert(ret == -1);
    assert(client.expire("test_expire_ttl_presist_k2", 100, &ret));
    assert(ret == 0);
    assert(client.expire("test_expire_ttl_presist_k1", 100, &ret));
    assert(ret == 1);
    assert(client.ttl("test_expire_ttl_presist_k1", &ret));
    assert(ret > 90);
    assert(client.persist("test_expire_ttl_presist_k2", &ret));
    assert(ret == 0);
    assert(client.persist("test_expire_ttl_presist_k1", &ret));
    assert(ret == 1);
    assert(client.ttl("test_expire_ttl_presist_k1", &ret));
    assert(ret == -1);
    assert(client.del("test_expire_ttl_presist_k1", nullptr));

    
}

void test_exec_command()
{
    RedisClient client(host, port);
    

    SharedPtrRedisReply reply = client.exec_command("SET %s %d", "test_exec_command_k1", 1);
    assert(reply);
    assert(reply->type() == RedisReply::REPLY_STATUS);

    reply = client.exec_command("DEL %s", "test_exec_command_k1");
    assert(reply);
    assert(reply->type() == RedisReply::REPLY_INTEGER);
    assert(reply->get_int32() == 1);
    assert(reply->get_int64() == 1);

    
}

void test_exec_command_argv()
{
    RedisClient client(host, port);
    
    
    std::vector<const char *>  argv_arr;
    std::vector<size_t> argvlen_arr;

    const char * cmd = "SET";
    argv_arr.emplace_back(cmd);
    argvlen_arr.emplace_back(strlen(cmd));

    const char * key = "test_exec_command_argv_k1";
    argv_arr.emplace_back(key);
    argvlen_arr.emplace_back(strlen(key));

    const char * value = "1";
    argv_arr.emplace_back(value);
    argvlen_arr.emplace_back(strlen(value));

    SharedPtrRedisReply reply = client.exec_command_argv(argv_arr.size(), &argv_arr[0], &argvlen_arr[0]);
    assert(reply);
    assert(reply->type() == RedisReply::REPLY_STATUS);

    argv_arr.clear();
    argvlen_arr.clear();

    const char * cmd_del = "DEL";
    argv_arr.emplace_back(cmd_del);
    argvlen_arr.emplace_back(strlen(cmd_del));

    argv_arr.emplace_back(key);
    argvlen_arr.emplace_back(strlen(key));

    reply = client.exec_command_argv(argv_arr.size(), &argv_arr[0], &argvlen_arr[0]);
    assert(reply);
    assert(reply->type() == RedisReply::REPLY_INTEGER);
    assert(reply->get_int32() == 1);
    assert(reply->get_int64() == 1);

    
}

void test_list()
{
    RedisClient client(host, port);
    
    
    RedisList list;
    list.bind_client(&client);

    int len = 0;

    client.del("test_list_k1", nullptr);
    client.del("test_list_k2", nullptr);
    client.del("test_list_k3", nullptr);
    client.del("test_list_k4", nullptr);

    assert(list.lpush("test_list_k1", "1", &len));
    assert(1 == len);
    assert(list.lpush("test_list_k1", "2", &len));
    assert(2 == len);

    assert(list.rpush("test_list_k2", "1", &len));
    assert(1 == len);
    assert(list.rpush("test_list_k2", "2", &len));
    assert(2 == len);

    assert(list.rpush("test_list_k3", "1", &len));
    assert(1 == len);
    assert(list.llen("test_list_k3", &len));
    assert(1 == len);
    assert(list.rpush("test_list_k3", "2", &len));
    assert(2 == len);
    assert(list.llen("test_list_k3", &len));
    assert(2 == len);

    assert(list.lpush("test_list_k4", "1", &len));
    assert(1 == len);
    assert(list.rpush("test_list_k4", "2", &len));
    assert(2 == len);
    assert(list.lpush("test_list_k4", "3", &len));
    assert(3 == len);
    assert(list.rpush("test_list_k4", "4", &len));
    assert(4 == len);
    std::string ret_str;
    
    assert(list.lindex("test_list_k4", 0, &ret_str));
    assert(ret_str == "3");
    assert(list.lindex("test_list_k4", 1, &ret_str));
    assert(ret_str == "1");
    assert(list.lindex("test_list_k4", 2, &ret_str));
    assert(ret_str == "2");
    assert(list.lindex("test_list_k4", 3, &ret_str));
    assert(ret_str == "4");

    SharedPtrRedisReply reply = list.lrange("test_list_k4", 1, 2);
    assert(reply);
    assert(reply->type() == RedisReply::REPLY_ARRAY);
    assert(reply->get_array_size() == 2);
    assert(reply->get_element(0));
    assert(reply->get_element(1));
    assert(reply->get_element(0)->type() == RedisReply::REPLY_STRING);
    assert(reply->get_element(1)->type() == RedisReply::REPLY_STRING);
    assert(strcmp(reply->get_element(0)->get_str(), "1") == 0);
    assert(strcmp(reply->get_element(1)->get_str(), "2") == 0);

    assert(list.lpop("test_list_k4", &ret_str));
    assert(ret_str == "3");
    assert(list.rpop("test_list_k4", &ret_str));
    assert(ret_str == "4");

    client.del("test_list_k1", nullptr);
    client.del("test_list_k2", nullptr);
    client.del("test_list_k3", nullptr);
    client.del("test_list_k4", nullptr);

    

}

void test_set_structure()
{
    RedisClient client(host, port);
    
    
    RedisSet set;
    set.bind_client(&client);

    client.del("test_set_structure_k1", nullptr);

    StringArray str_arr;
    str_arr.push_back("1");
    str_arr.push_back("2");
    str_arr.push_back("3");
    str_arr.push_back("4");

    int count = 0;
    assert(set.sadd("test_set_structure_k1", str_arr, &count));
    assert(count == 4);

    assert(set.sismember("test_set_structure_k1", "1", &count));
    assert(count == 1);

    assert(set.sismember("test_set_structure_k1", "10", &count));
    assert(count == 0);

    assert(set.scard("test_set_structure_k1", &count));
    assert(count == 4);

    str_arr.pop_back();
    str_arr.pop_back();
    assert(set.srem("test_set_structure_k1", str_arr, &count));
    assert(count == 2);

    SharedPtrRedisReply reply = set.smembers("test_set_structure_k1");
    assert(reply);
    assert(reply->type() == RedisReply::REPLY_ARRAY);
    assert(reply->get_array_size() == 2);
    assert(reply->get_element(0));
    assert(reply->get_element(0)->type() == RedisReply::REPLY_STRING);
    assert(strcmp(reply->get_element(0)->get_str(), "3") == 0
            || strcmp(reply->get_element(0)->get_str(), "4") == 0);
    assert(reply->get_element(1));
    assert(reply->get_element(1)->type() == RedisReply::REPLY_STRING);
    assert(strcmp(reply->get_element(1)->get_str(), "4") == 0
            || strcmp(reply->get_element(1)->get_str(), "5") == 0);

    reply = set.spop("test_set_structure_k1", 2);
    assert(reply);
    assert(reply->type() == RedisReply::REPLY_ARRAY);
    assert(reply->get_array_size() == 2);
    assert(reply->get_element(0));
    assert(reply->get_element(0)->type() == RedisReply::REPLY_STRING);
    assert(strcmp(reply->get_element(0)->get_str(), "3") == 0
            || strcmp(reply->get_element(0)->get_str(), "4") == 0);
    assert(reply->get_element(1));
    assert(reply->get_element(1)->type() == RedisReply::REPLY_STRING);
    assert(strcmp(reply->get_element(1)->get_str(), "3") == 0
            || strcmp(reply->get_element(1)->get_str(), "4") == 0);

    client.del("test_set_structure_k1", nullptr);
    
}

void test_zset()
{
    RedisClient client(host, port);
    
    
    RedisZset zset;
    zset.bind_client(&client);

    client.del("test_zset_k1", nullptr);

    MemberScoreArray ms_arr;
    ms_arr.push_back(std::make_pair("m1", "1"));
    ms_arr.push_back(std::make_pair("m2", "2"));
    ms_arr.push_back(std::make_pair("m3", "3"));
    ms_arr.push_back(std::make_pair("m4", "4"));

    int count = 1;
    assert(zset.zadd("test_zset_k1", ms_arr, &count));
    assert(count == 4);

    assert(zset.zcard("test_zset_k1", &count));
    assert(count == 4);

    int rank = -1;
    assert(zset.zrank("test_zset_k1", "m1", &rank));
    assert(rank == 0);
    assert(zset.zrank("test_zset_k1", "m2", &rank));
    assert(rank == 1);
    assert(zset.zrank("test_zset_k1", "m3", &rank));
    assert(rank == 2);
    assert(zset.zrank("test_zset_k1", "m4", &rank));
    assert(rank == 3);

    assert(zset.zrevrank("test_zset_k1", "m1", &rank));
    assert(rank == 3);
    assert(zset.zrevrank("test_zset_k1", "m2", &rank));
    assert(rank == 2);
    assert(zset.zrevrank("test_zset_k1", "m3", &rank));
    assert(rank == 1);
    assert(zset.zrevrank("test_zset_k1", "m4", &rank));
    assert(rank == 0);

    std::string score;
    assert(zset.zscore("test_zset_k1", "m2", &score));
    assert(score == "2");
    assert(zset.zincrby("test_zset_k1", "100", "m2", &score));
    assert(score == "102");
    assert(zset.zscore("test_zset_k1", "m2", &score));
    assert(score == "102");
    assert(!zset.zscore("test_zset_k1", "m200", &score));

    StringArray members = {"m2", "m3", "m200"};
    assert(zset.zrem("test_zset_k1", members, &count));
    assert(count == 2);

    assert(zset.zcard("test_zset_k1", &count));
    assert(count == 2);

    SharedPtrRedisReply reply = zset.zrange("test_zset_k1", 0, -1);
    assert(reply);
    assert(reply->type() == RedisReply::REPLY_ARRAY);
    assert(reply->get_array_size() == 4);
    assert(reply->get_element(0));
    assert(reply->get_element(0)->type() == RedisReply::REPLY_STRING);
    assert(strcmp(reply->get_element(0)->get_str(), "m1") == 0);

    assert(reply->get_element(1));
    assert(reply->get_element(1)->type() == RedisReply::REPLY_STRING);
    assert(strcmp(reply->get_element(1)->get_str(), "1") == 0);

    assert(reply->get_element(2));
    assert(reply->get_element(2)->type() == RedisReply::REPLY_STRING);
    assert(strcmp(reply->get_element(2)->get_str(), "m4") == 0);

    assert(reply->get_element(3));
    assert(reply->get_element(3)->type() == RedisReply::REPLY_STRING);
    assert(strcmp(reply->get_element(3)->get_str(), "4") == 0);

    reply = zset.zrange("test_zset_k1", 0, -1, false);
    assert(reply);
    assert(reply->type() == RedisReply::REPLY_ARRAY);
    assert(reply->get_array_size() == 2);
    assert(reply->get_element(0));
    assert(reply->get_element(0)->type() == RedisReply::REPLY_STRING);
    assert(strcmp(reply->get_element(0)->get_str(), "m1") == 0);

    assert(reply->get_element(1));
    assert(reply->get_element(1)->type() == RedisReply::REPLY_STRING);
    assert(strcmp(reply->get_element(1)->get_str(), "m4") == 0);
    
    reply = zset.zrevrange("test_zset_k1", 0, -1);
    assert(reply);
    assert(reply->type() == RedisReply::REPLY_ARRAY);
    assert(reply->get_array_size() == 4);
    assert(reply->get_element(0));
    assert(reply->get_element(0)->type() == RedisReply::REPLY_STRING);
    assert(strcmp(reply->get_element(0)->get_str(), "m4") == 0);

    assert(reply->get_element(1));
    assert(reply->get_element(1)->type() == RedisReply::REPLY_STRING);
    assert(strcmp(reply->get_element(1)->get_str(), "4") == 0);

    assert(reply->get_element(2));
    assert(reply->get_element(2)->type() == RedisReply::REPLY_STRING);
    assert(strcmp(reply->get_element(2)->get_str(), "m1") == 0);

    assert(reply->get_element(3));
    assert(reply->get_element(3)->type() == RedisReply::REPLY_STRING);
    assert(strcmp(reply->get_element(3)->get_str(), "1") == 0);

    reply = zset.zrevrange("test_zset_k1", 0, -1, false);
    assert(reply);
    assert(reply->type() == RedisReply::REPLY_ARRAY);
    assert(reply->get_array_size() == 2);
    assert(reply->get_element(0));
    assert(reply->get_element(0)->type() == RedisReply::REPLY_STRING);
    assert(strcmp(reply->get_element(0)->get_str(), "m4") == 0);

    assert(reply->get_element(1));
    assert(reply->get_element(1)->type() == RedisReply::REPLY_STRING);
    assert(strcmp(reply->get_element(1)->get_str(), "m1") == 0);

    client.del("test_zset_k1", nullptr);
    
}

void test_hash()
{
    RedisClient client(host, port, "test:");
    
    RedisHash hash;
    hash.bind_client(&client);

    client.del("test_hash_k1");

    int count = 0;
    assert(hash.hset("test_hash_k1", "f1", "v1", &count));
    assert(count == 1);

    std::string value;
    assert(hash.hget("test_hash_k1", "f1", &value));
    assert(value == "v1");

    FieldValueArray fv_arr = {std::make_pair("f2", "v2"), std::make_pair("f3", "v3"), std::make_pair("f4", "4")};
    assert(hash.hmset("test_hash_k1", fv_arr));

    StringArray field_arr = {"f1", "f2", "not_f"};
    SharedPtrRedisReply reply = hash.hmget("test_hash_k1", field_arr);
    assert(reply);
    assert(reply->type() == RedisReply::REPLY_ARRAY);
    assert(reply->get_array_size() == 3);
    assert(reply->get_element(0));
    assert(reply->get_element(0)->type() == RedisReply::REPLY_STRING);
    assert(strcmp(reply->get_element(0)->get_str(), "v1") == 0);

    assert(reply->get_element(1));
    assert(reply->get_element(1)->type() == RedisReply::REPLY_STRING);
    assert(strcmp(reply->get_element(1)->get_str(), "v2") == 0);

    assert(reply->get_element(2));
    assert(reply->get_element(2)->type() == RedisReply::REPLY_NIL);

    assert(hash.hexists("test_hash_k1", "f1", &count));
    assert(count == 1);
    assert(hash.hexists("test_hash_k1", "not_f1", &count));
    assert(count == 0);

    assert(hash.hlen("test_hash_k1", &count));
    assert(count == 4);
    assert(hash.hdel("test_hash_k1", field_arr, &count));
    assert(count == 2);
    assert(hash.hlen("test_hash_k1", &count));
    assert(count == 2);

    assert(hash.hget("test_hash_k1", "f4", &value));
    assert(value == "4");
    assert(hash.hincrby("test_hash_k1", "f4", 10, &count));
    assert(count == 14);
    assert(hash.hget("test_hash_k1", "f4", &value));
    assert(value == "14");

    assert(hash.hlen("test_hash_k1", &count));
    assert(count == 2);

    reply = hash.hkeys("test_hash_k1");
    assert(reply);
    assert(reply->type() == RedisReply::REPLY_ARRAY);
    assert(reply->get_array_size() == 2);
    assert(reply->get_element(0));
    assert(reply->get_element(0)->type() == RedisReply::REPLY_STRING);
    assert(strcmp(reply->get_element(0)->get_str(), "f3") == 0
            || strcmp(reply->get_element(0)->get_str(), "f3") == 0);

    assert(reply->get_element(1));
    assert(reply->get_element(1)->type() == RedisReply::REPLY_STRING);
    assert(strcmp(reply->get_element(1)->get_str(), "f4") == 0
            || strcmp(reply->get_element(1)->get_str(), "f3") == 0);

    reply = hash.hvals("test_hash_k1");
    assert(reply);
    assert(reply->type() == RedisReply::REPLY_ARRAY);
    assert(reply->get_array_size() == 2);
    assert(reply->get_element(0));
    assert(reply->get_element(0)->type() == RedisReply::REPLY_STRING);
    assert(strcmp(reply->get_element(0)->get_str(), "v3") == 0 ||
            strcmp(reply->get_element(0)->get_str(), "14") == 0);

    assert(reply->get_element(1));
    assert(reply->get_element(1)->type() == RedisReply::REPLY_STRING);
    assert(strcmp(reply->get_element(1)->get_str(), "14") == 0
            || strcmp(reply->get_element(1)->get_str(), "v3") == 0);

    reply = hash.hgetall("test_hash_k1");
    assert(reply);
    assert(reply->type() == RedisReply::REPLY_ARRAY);
    assert(reply->get_array_size() == 4);

    assert(reply->get_element(0));
    assert(reply->get_element(0)->type() == RedisReply::REPLY_STRING);
    assert(strcmp(reply->get_element(0)->get_str(), "f3") == 0);

    assert(reply->get_element(1));
    assert(reply->get_element(1)->type() == RedisReply::REPLY_STRING);
    assert(strcmp(reply->get_element(1)->get_str(), "v3") == 0);

    assert(reply->get_element(2));
    assert(reply->get_element(2)->type() == RedisReply::REPLY_STRING);
    assert(strcmp(reply->get_element(2)->get_str(), "f4") == 0);

    assert(reply->get_element(3));
    assert(reply->get_element(3)->type() == RedisReply::REPLY_STRING);
    assert(strcmp(reply->get_element(3)->get_str(), "14") == 0);

    client.del("test_hash_k1");
}

int main()
{
    //test_conn();
    //test_close_is_connected();
    test_auth();
    test_set();
    test_get();
    test_prefix();
    test_del();
    test_exists();
    test_incr();
    test_decr();
    test_expire_ttl_presist();
    test_exec_command();
    test_exec_command_argv();

    test_list();

    test_set_structure();

    test_zset();
    
    test_hash();

    return 0;
}

