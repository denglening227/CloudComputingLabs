/*
 * @Author: your name
 * @Date: 2020-05-26 02:53:08
 * @LastEditTime: 2020-05-26 03:56:13
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \CloudComputingLabs\Lab3\src\config.cc
 */
#include <config.h>

// Q1: LOG_F是什么函数？PS:协调员不需要记录，除非记录给咱看，看情况弄不弄？
// Q2: LOG_F函数的第一个参数是在哪里定义的？
// Q3: getBanner是个什么函数？

/* 解析长参数函数
 * 参数传入：（参数数量，参数字符，引用file_path的地址）
 * 参数传出：【无】直接更改file_path的值。
 */
void Config::parse_arg(int argc, char *argv[], std::string &file_path) {
    // 长参数解析
    const char *optstring = "c:";
    int c;
    struct option opts[] =
    {
        {"config_path", 1, nullptr, 'c'}
    };
    //
    while ((c = getopt_long(argc, argv, optstring, opts, nullptr)) != -1) 
    {
        switch (c) {
            case 'c':
                file_path = optarg;
                //LOG_F(INFO, "<ARG>File_path is %s", optarg);
                break;
            case '?':
                //LOG_F(ERROR, "Unknown Option.");
                break;
            default:
                //LOG_F(ERROR, "------\n");
                break;
        }
    }
    // 不再检查一下c字符后是否全正确？等一下拼错单词也给过嘛？
}

/* 解析配置文件函数
 * 参数传入：（引用file_path的地址）
 * 参数传出：【无】
 */
void Config::parse_config(const std::string &file_path) {
    // 打开文件流
    std::ifstream infile;
    infile.open(file_path, std::ios::in);
    // 判断是否打开
    if (!infile.is_open()) {
        //LOG_F(ERROR, "Failed to Open Configuration!!!");
        exit(1);
    }
    // 逐行读入并解析每一行
    std::string line;
    while (getline(infile, line))
    {
        // 除去空行、注释行
        if (line.size() == 0 || line[0] == '!') continue;
        //LOG_F(INFO, "<InputLine>%s", line.c_str());

        // 读入有效，按空格分离单词，若超过两个词则为错误表达，反之表达有效则继续解析
        std::vector<std::string> res = Config::split(line, " ");
        if (res.size() != 2)
        {
            //LOG_F(ERROR, "<PARSE_CONFIG>Wrong expressions in config.");
            continue;
        }
        if (res[0] == "mode")
        {
            std::string tmp;
            for (int i = 0; i < 11; i++)
            {
                tmp += res[1][i];
            }
            res[1] = tmp;
            if (res[1] == "coordinator")
            {
                mode = COORDINATOR_MODE;
                //LOG_F(INFO, "<PARSE_CONFIG>Mode is coordinator");
            } 
            else if (res[1] == "participant") 
            {
                mode = PARTICIPANT_MODE;
                //LOG_F(INFO, "<PARSE_CONFIG>Mode is participant");
            }
        } else {
            std::vector<std::string> ip_port = Config::split(res[1], ":");
            if ((mode==COORDINATOR_MODE && res[0] == "coordinator_info") ||
                (mode==PARTICIPANT_MODE && res[0] == "participant_info")) 
            {
                self = make_pair(ip_port[0], ip_port[1]);
            } 
            else if ((mode==PARTICIPANT_MODE && res[0] == "coordinator_info") ||
                     (mode==COORDINATOR_MODE && res[0] == "participant_info"))
            {
                // 去除后面的换行符
                std::string tmp;
                for (auto ch: ip_port[1]) {
                    if (ch >= '0' && ch <= '9') {
                        tmp += ch;
                    } else {
                        break;
                    }
                }
                ip_port[1] = tmp;
                others.push_back(make_pair(ip_port[0], ip_port[1]));
            }
            else if(mode==UNDEFINED_MODE)
            {
                //LOG_F(ERROR, "<PARSE_CONFIG>Wrong expression in config.");
            }
        }
    }
    infile.close();
}

/* 重置split函数，以参数 delim 指定分割 str
 * 参数传入：（引用“读入行”的地址，引用delim的地址）
 * 参数传出：【分割完毕的tokens字符串数组】
 */
std::vector<std::string> Config::split(const std::string &str, const std::string &delim) {
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do {
        pos = str.find(delim, prev);
        if (pos == std::string::npos) pos = str.length();
        std::string token = str.substr(prev, pos - prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
}

/* ?
 * 参数传入：（引用file_path的地址）
 * 参数传出：【ret?】
 */
std::string Config::getBanner(const std::string &path) {
    std::ifstream infile;
    infile.open(path, std::ios::in);

    if (!infile.is_open()) {
        //LOG_F(ERROR, "can not find banner");
    }
    std::string line;
    std::string ret;
    while (getline(infile, line)) {
        ret += (line + "\n");
    }
    return ret;
}
