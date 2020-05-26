/*
 * @Author: your name
 * @Date: 2020-05-26 13:55:06
 * @LastEditTime: 2020-05-26 17:30:53
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \CloudComputingLabs\Lab3\src\database.h
 */
#include <iostream>
#include <string.h>
#include <vector>
using namespace std;
/* 数据条目：键-值
 * 计数需外部变量以专门计数
 */


class DATABASE_ENTRY
{
    private:
        string key;
        string value;

    public:
        DATABASE_ENTRY(){key="";value="";}
        DATABASE_ENTRY(string a,string b){key=a;value=b;}
	DATABASE_ENTRY(const DATABASE_ENTRY & db);

        string getKey(){return key;}
        string getValue(){return value;}
        void setKey(string a){key=a;}
        void setValue(string b){value=b;}
};

DATABASE_ENTRY::DATABASE_ENTRY(const DATABASE_ENTRY & db)
{
	this->key = db.key;
	this->value = db.value;
}

vector<DATABASE_ENTRY> database;
int data_SET(string key, string value);
int data_UPDATE(string key, string value);
string data_GET(string key);
vector<DATABASE_ENTRY>::iterator data_FIND(vector<DATABASE_ENTRY>::iterator it, string key);
int data_DEL(string key);
void printDatabase();
