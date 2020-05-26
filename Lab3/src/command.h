/*
 * @Author: your name
 * @Date: 2020-05-27 01:34:17
 * @LastEditTime: 2020-05-27 01:43:48
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \CloudComputingLabs\Lab3\src\command.h
 */
#include <iostream>
#include <string>
#include <vector>
using namespace std;

enum COMMAND_TYPE {UNDEF,ADD,DEL,GET};


class COMMAND
{
    private:
        enum COMMAND_TYPE type;
        string key;
        string value;

    public:
        COMMAND(){type=UNDEF;key="";value="";}
        COMMAND(enum COMMAND_TYPE t,string k,string v){type=t;key=k;value=v;}
        COMMAND(const COMMAND & commands);

        enum COMMAND_TYPE getType(){return type;}
        string getKey(){return key;}
        string getValue(){return value;}
        enum COMMAND_TYPE setType(enum COMMAND_TYPE t){type=t;}
        void setKey(string a){key=a;}
        void setValue(string b){value=b;}

     // 分割字符
     static std::vector<std::string> split(const std::string &str, const std::string &delim);
};

COMMAND::COMMAND(const COMMAND & commands)
{
    this->type = commands.type;
    this->key = commands.key;
    this->value = commands.value;
}

vector<COMMAND> commands;


/* 示范


// [Example]
//搞个类的对象
vector<COMMAND> commands;
// 命令字符串用一个if-else转换，一个操作对应一个command_ADD
if(string = "???")
{
    command_ADD(ADD,"",key,value);
}
else if(...)
{
    command_ADD(ADD,"",key,value);
}
//...
// 反转vector空间，因为操作完直接弹出来后边最方便
commands.reverse(commands.first(),commands.end())

//然后轮流获取操作、键值
vector<COMMAND>::iterator it = commands.back(); //迭代器指向最后一个
COMMAND_TYPE type = it.getCommand();    //迭代器指向的对象，获取其操作、键值
string key = it.getKey();
string value = it.getValue();

//引用函数将操作发送到参与者
send_function();//这个名字不确定

//等参与者操作完成，删除最后一个，以及容器非空则迭代器前移
if(!commands.empty())
{
    --it;
}
commands.pop_back();

//然后一个轮回就完了

*/
