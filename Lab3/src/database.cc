/*
 * @Author: your name
 * @Date: 2020-05-26 13:56:34
 * @LastEditTime: 2020-05-26 19:27:55
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \CloudComputingLabs\Lab3\src\database.cc
 */
#include "database.h"

/* 添加数据
 * 参数输入：（键，值）
 * 参数输出：【-1-完全重复；0-失败；1-键重复更改失败；2-键重复更新成功；3-添加成功】
 */
int data_SET(string key, string value)
{
	string temp = data_GET(key);
    if(temp == "")       // 情况1：不存在相同键
    {
        DATABASE_ENTRY temp(key,value);
        database.push_back(temp);
        // 检查是否成功添加
        if(data_GET(key) == value) return 3;
        return 0;
    }
    else if(temp == value) // 情况2：完全重合
    {
        return -1;
    }
    else                            // 情况3：存在相同键，但值不同，则无条件覆盖
    {
        return 1+data_UPDATE(key,value);
    }
}
/* 更新数据
 * 参数输入：（键，值）
 * 参数输出：【0-失败；1-更新成功】
 */
int data_UPDATE(string key, string value)
{
    vector<DATABASE_ENTRY>::iterator it = data_FIND(database.begin(),key);
    it->setValue(value);
    if(data_GET(key) == value) return 1;
    else return 0;
}


/* 获取对应值数据
 * 参数输入：（键）
 * 参数输出：【string-找到对应键，输出其值；NULL-找不到】
 */
string data_GET(string key)
{
    vector<DATABASE_ENTRY>::iterator it = data_FIND(database.begin(),key);
    if(it==database.end()) return "";
    else return it->getValue();
}
/* 迭代器递归查找数据
 * 参数输入：（iterator迭代器，键）
 * 参数输出：【iterator-找到对应键时的迭代器，输出其值；NULL-找不到】
 */
vector<DATABASE_ENTRY>::iterator data_FIND(vector<DATABASE_ENTRY>::iterator it, string key)
{
	if(it == database.end()) return it;
    if(it->getKey() == key) return it;
    return data_FIND(++it,key);
}

/* 删除数据
 * 参数输入：（键，值）
 * 参数输出：【0-失败；1-数据库本身空；2-键不存在；3-删除成功】
 */
int data_DEL(string key)
{
    if(database.empty()) return 1;
    vector<DATABASE_ENTRY>::iterator it = data_FIND(database.begin(),key);
    if(it==database.end()) return 2;
    else database.erase(it);
    if(data_FIND(database.begin(),key)==database.end()) return 3;
    else return 0;
}

void database_PRINT()
{
    short num = 1;
    vector<DATABASE_ENTRY>::iterator it = database.begin();
    for(;;++it,++num)
    {
        if(it == database.end()) break;
        cout<<num<<" "<<it->getKey()<<": "<<it->getValue()<<endl;
    }
}


/* 测试用
int main()
{
	cout<<"start set1"<<endl;
    cout<<data_SET("abc","abc")<<endl;
	cout<<endl<<"start set2"<<endl;
    cout<<data_SET("abd","123")<<endl;
    database_PRINT();

    cout<<endl<<"start get1"<<endl;
    cout<<data_GET("abc")<<endl;

    cout<<endl<<"start set3"<<endl;
    cout<<data_SET("abc","456")<<endl;
    database_PRINT();
    cout<<endl<<"start set4"<<endl;
    cout<<data_SET("abc","456")<<endl;
    database_PRINT();

    cout<<endl<<"start del"<<endl;
    cout<<data_DEL("abd")<<endl;
    database_PRINT();
}
*/
