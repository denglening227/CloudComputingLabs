/*
 * @Author: your name
 * @Date: 2020-05-26 02:08:59
 * @LastEditTime: 2020-05-27 05:58:46
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \CloudComputingLabs\Lab3\src\config.h
 */ 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <getopt.h>
using namespace std;

enum MODE {
    UNDEFINED_MODE,
    COORDINATOR_MODE,
    PARTICIPANT_MODE
};

// 配置文件, 解析, 传递配置
class Config {
    public:
        enum MODE mode;
        // pair 代表 <ip, port>
        // self 代表自己(根据 mode的值)
        pair<string,string> self;
        vector< pair<string,string> > others;

    Config(){mode=UNDEFINED_MODE;}
    Config(const Config & config);

    // 解析配置文件
    void parse_config(const string &file_path);

    // 解析命令行参数
    static void parse_arg(int argc, char *argv[], string &file_path);

    // get banner
    static string getBanner(const string &path);

    // 分割字符
    static vector<string> split(const string &str, const string &delim);

};

Config::Config(const Config & config)
{
    this->mode = config.mode;
    this->self = make_pair(config.self.first,config.self.second);
    this->others.assign(config.others.begin(),config.others.end());
}