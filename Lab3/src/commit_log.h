/*
 * @Author: your name
 * @Date: 2020-05-26 14:28:57
 * @LastEditTime: 2020-05-26 22:58:50
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \CloudComputingLabs\Lab3\src\log.h
 */
#include <iostream>
#include <string>
#include <vector>
using namespace std;

enum LOG_TYPE
{
    // Others
    UNDEF,
    ERROR,
    INFO,
    
    // 协调员与参与者
    REQ_TO_PRE,
    PREPARED,
    ABORT,
    COMMIT,
    GOT_COMMIT
};

/* 日志类
 * 成员1-种类-用以标注
 * 成员2-内容-具体的内容
 * 成员3-(可选)键-当对键的值发生变更时，记录此处，以便回滚时找到发生了改变的键
 * 成员4-(可选)值-当对成员3键的值发生变更时，记录此处，以便回滚时找到发生了改变的值
 */
class COMMIT_LOG
{
    private:
        enum LOG_TYPE type;
        string details;
        string key;
        string value;

    public:
        COMMIT_LOG(){type=UNDEF;details="";key="";value="";}
        COMMIT_LOG(enum LOG_TYPE t,string s){type=t;details=s;key="";value="";}
        COMMIT_LOG(enum LOG_TYPE t,string s,string k,string v){type=t;details=s;key=k;value=v;}
        COMMIT_LOG(const COMMIT_LOG & log);

        enum LOG_TYPE getType(){return type;}
        string getDetails(){return details;}
        string getKey(){return key;}
        string getValue(){return value;}
        enum LOG_TYPE setType(enum LOG_TYPE t){type=t;}
        void setDetails(string s){details=s;}
        void setKey(string a){key=a;}
        void setValue(string b){value=b;}
};

COMMIT_LOG::COMMIT_LOG(const COMMIT_LOG & log)
{
    this->type = log.type;
    this->details = log.details;
    this->key = log.key;
    this->value = log.value;
}

vector<COMMIT_LOG> commit_log;