/*
 * @Author: your name
 * @Date: 2020-05-27 01:42:47
 * @LastEditTime: 2020-05-27 02:06:22
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \CloudComputingLabs\Lab3\src\command.cc
 */

#include "command.h"


/* 添加日志条
 * 参数输入：（类别，详请内容（，键，值）） 后两者为可选
 * 参数输出：【无】
 * -类别：UNDEF,ADD,DEL,GET
 */
void command_ADD(COMMAND_TYPE type, string key, string value)
{
    COMMAND com(type,key,value);
    commands.push_back(com);
}

/* 删除操作条command_entry
 * 参数输入：（无 或 次数）无-默认一次、次数-指定回滚条目数
 * 参数输出：【无】
 */
void command_POPBACK()
{
    if(!commands.empty())
    {
        commands.pop_back();
    }
}
void command_POPBACK(int times)
{
    for(int i=0;;++i)
    {
        if(i<times && !commands.empty())
        commands.pop_back();
        else break;
    }
}

vector<COMMAND>::iterator command_FIND(vector<COMMAND>::iterator it, COMMAND_TYPE type, string key, string value)
{
    if(it->getType() == type && it->getKey() == key && it->getValue() == value)
    {
        return it;
    }
    if(it == commands.begin()) return commands.end();
    return command_FIND(it,type,key,value);
}

// 删除指定（操作，键，值）的条目
void command_DEL(COMMAND_TYPE type, string key, string value)
{
    if(commands.empty()) return;
    vector<COMMAND>::iterator it = command_FIND(--commands.end(),type,key,value);
    if(it==commands.end()) return;
    else commands.erase(it);
}

/* 打印日志
 * 参数输入：（无）
 * 参数输出：【无】
 */
void command_PRINT()
{
    short num = 1;
    vector<COMMAND>::iterator it = commands.begin();
    for(;;++it,++num)
    {
        if(it == commands.end()) break;
        cout<<num<<" <";
        switch(it->getType())
        {
            case UNDEF:cout<<"UNDEFINED";break;
            case ADD:cout<<"ADD";break;
            case DEL:cout<<"DEL";break;
            case GET:cout<<"GET";break;
            default:break;
        }
        cout<<"> ";
        cout<<" "<<it->getKey()<<"-"<<it->getValue()<<" has been changed."<<endl;
    }
}
