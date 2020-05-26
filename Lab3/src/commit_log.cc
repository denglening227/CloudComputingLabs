/*
 * @Author: your name
 * @Date: 2020-05-26 14:29:10
 * @LastEditTime: 2020-05-26 23:27:17
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \CloudComputingLabs\Lab3\src\log.cc
 */

#include "commit_log.h"

/* 添加日志条
 * 参数输入：（类别，详请内容（，键，值）） 后两者为可选
 * 参数输出：【无】
 * -类别：UNDEF,ERROR,INFO,REQ_TO_PRE,PREPARED,ABORT
 * ,COMMIT,GOT_COMMIT
 */
void log_ADD(LOG_TYPE type, string details)
{
    COMMIT_LOG log(type,details);
    commit_log.push_back(log);
}
void log_ADD(LOG_TYPE type, string details, string key, string value)
{
    COMMIT_LOG log(type,details,key,value);
    commit_log.push_back(log);
}

/* 回滚日志条
 * 参数输入：（无 或 次数）无-默认一次、次数-指定回滚条目数
 * 参数输出：【无】
 */
void log_ROLLBACK()
{
    if(!commit_log.empty())
    {
        commit_log.pop_back();
    }
}
void log_ROLLBACK(int times)
{
    for(int i=0;;++i)
    {
        if(i<times && !commit_log.empty())
        commit_log.pop_back();
        else break;
    }
}

vector<COMMIT_LOG>::iterator log_FIND(vector<COMMIT_LOG>::iterator it, LOG_TYPE type, string details, string key, string value)
{
    if(it->getType() == type && it->getDetails() == details)
    {
        if(key == "")
        {
            return it;
        }
        else if(it->getKey() == key && it->getValue() == value)
        {
            return it;
        }
    }
    if(it == commit_log.begin()) return commit_log.end();
    return log_FIND(it,type,details,key,value);
}
vector<COMMIT_LOG>::iterator log_FIND(vector<COMMIT_LOG>::iterator it, LOG_TYPE type, string details)
{
    return log_FIND(it,type,details,"","");
}

void log_DEL(LOG_TYPE type, string details, string key, string value)
{
    if(commit_log.empty()) return;
    vector<COMMIT_LOG>::iterator it = log_FIND(--commit_log.end(),type,details,key,value);
    if(it==commit_log.end()) return;
    else commit_log.erase(it);
}
void log_DEL(LOG_TYPE type, string details)
{
    log_DEL(type,details,"","");
}

/* 打印日志
 * 参数输入：（无）
 * 参数输出：【无】
 */
void log_PRINT()
{
    short num = 1;
    vector<COMMIT_LOG>::iterator it = commit_log.begin();
    for(;;++it,++num)
    {
        if(it == commit_log.end()) break;
        cout<<num<<" <";
        switch(it->getType())
        {
            case UNDEF:cout<<"UNDEFINED";break;
            case ERROR:cout<<"ERROR";break;
            case INFO:cout<<"INFO";break;
            case REQ_TO_PRE:cout<<"REQUEST_TO_PREPARE";break;
            case PREPARED:cout<<"PREPARED";break;
            case ABORT:cout<<"ABORT";break;
            case COMMIT:cout<<"COMMIT";break;
            case GOT_COMMIT:cout<<"GOT_COMMIT";break;
            default:break;
        }
        cout<<"> "<<it->getDetails();
        if(it->getKey() == "")
        {
            cout<<endl;
        }
        else
        {
            cout<<" "<<it->getKey()<<"-"<<it->getValue()<<" has been changed."<<endl;
        }
    }
}

/*
int main()
{
    cout<<"start set1"<<endl;
    log_ADD(PREPARED,"Answer REQ.");
    cout<<endl<<"start set2"<<endl;
    log_ADD(ABORT,"Some participant ABORTs.");
    log_PRINT();

    cout<<endl;
    cout<<endl<<"start rollback"<<endl;
    log_ROLLBACK();
    log_PRINT();

    cout<<endl;
    log_ADD(PREPARED,"Answer REQ.");
    log_ADD(ABORT,"Some participant ABORTs.");
    log_PRINT();
    cout<<endl<<"start rollback more times"<<endl;
    log_ROLLBACK(2);
    log_PRINT();

    cout<<endl;
    log_ADD(COMMIT,"GET","abc","123");
    log_PRINT();
    cout<<endl<<"start del"<<endl;
    log_DEL(COMMIT,"GET","abc","123");
    log_PRINT();
    return 0;
}
*/