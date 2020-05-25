/*
 * @Author: your name
 * @Date: 2020-05-26 02:08:59
 * @LastEditTime: 2020-05-26 03:56:52
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \CloudComputingLabs\Lab3\src\config.h
 */ 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <getopt.h>

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
        std::pair<std::string, std::string> self;
        std::vector<std::pair<std::string, std::string>> others;

    Config(){mode=UNDEFINED_MODE;}

    // 解析配置文件
    void parse_config(const std::string &file_path);

    // 解析命令行参数
    static void parse_arg(int argc, char *argv[], std::string &file_path);

    // get banner
    static std::string getBanner(const std::string &path);

    // 分割字符
    static std::vector<std::string> split(const std::string &str, const std::string &delim);

};