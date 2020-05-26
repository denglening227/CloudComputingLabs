/*
 * @Author: your name
 * @Date: 2020-05-26 02:08:59
 * @LastEditTime: 2020-05-27 06:34:28
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \CloudComputingLabs\Lab3\src\kvstore2pcsystem.cpp
 */ 
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include "log.cc"
#include "config.cc"
#include "database.cc"
#include "serverHandler.cpp"
using namespace std;



int main(int argc, char* argv[])
{
  // 判断参数
  static struct option long_options[] =
  {
    {"config_path", required_argument, NULL,'1'},
    {NULL,  0,   NULL, 0},
  };
  //判断是否缺少参数
	if(argc < 3)
	{
    printf("<Usage> Insert Arguments Path of the Configuration File.\n");
    printf("<Exmaple> ./kvstore2pcsystem --config_path ./src/coordinator.conf\n");
    printf("<Exmaple1> ./kvstore2pcsystem --config_path ./src/participant1.conf\n");
    printf("<Exmaple2> ./kvstore2pcsystem --config_path ./src/participant2.conf\n");
    printf("<Exmaple3> ./kvstore2pcsystem --config_path ./src/participant3.conf\n");
		return 1;
	}
  // 获取参数值
  char*file_Path=argv[2];

  config.parse_arg(argc,argv,file_path);
  config.parse_config(file_path);

  if(mode == COORDINATOR_MODE)
  {
    RUN_COORDINATOR(config);
  }
  else if(mode == PARTICIPANT_MODE)
  {
    RUN_PARTICIPANT(file_Path);
  }
  return 0;
}

void RUN_COORDINATOR(Config config)
{
  serverhandler(config);
}
void RUN_PARTICIPANT(char* filename)
{
  participant(filename);
}