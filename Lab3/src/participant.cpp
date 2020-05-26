#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include <set>
#include <sstream>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <map>
#include <time.h>       /* time */
#include "socklib.h"
#include "2pc.h" 
#include <signal.h>
#include <vector>
#include "command.h"
#include "commit_log.h"
#define PROB -1

using namespace std;


string char_to_str(char *str)      //字符数组转换为字符串 
{
	stringstream ss;
	string s;
	ss<<str;
	ss>>s;
	return s;
}



void real_commit(int sockfd)   //确定提交阶段  
{
	int prob  = rand() % 100;
	int len;                    //操作信息的长度
	if(recv(sockfd, (char *)&len, sizeof(int), 0) == -1)
	{
		perror("failed to recv data from coordinator");
		return;
	}
	char msg[len];      //操作信息  
	if(recv_msg(sockfd, msg, len) != 0)
	{
		perror("failed to recv coordinator data");
		return;
	}
	printf("recv:[%s]\n", msg);
	int reply = COMMIT_SUCCESS;
	if(prob <= PROB)
		reply = COMMIT_FAIL;
	else if(redo_log.index == -1)
		reply == COMMIT_FAIL;
	send(sockfd, (char *)&reply, sizeof(int), 0);


        string line=char_to_str(msg);

	std::vector<std::string> res = COMMAND::split(line, " ");
	vector<int>::iterator it;
        it=res.begin();
	string op_type=*it;
 	if(op_type=="SET")
        {
             it++;
             string key=*it;
	     it++;
             string value=*it;
             data_SET(key, value);
        }
	else if(op_type=="GET")
	{
	     it++;
             string key=*it;
	     data_GET(key);
        }
	else if(op_type=="DEL")
        {
                for(it=it++;it!=res.end();it++)
    		{
        	   string key=*it;
		   data_DEL(key);
    		}
        }
        log_ADD( GOT_COMMIT, "");
	return;
}

void terminate(int sockfd)        //参与者停止 
{
	puts("coordinator ask me to terminate");
/*	for(int i = 0; i < 20; i++)
	{
		printf("index %d\n", i);
		cout<<"["<<database[i].st<<"]"<<endl;
	}
*/ 
        log_ADD(ABORT, "");
	exit(EXIT_SUCCESS);
	return;
}

void answer_commit(int sockfd)    //对提交请求的返信 
{
	int index = redo_log.index;
	int prob  = rand() % 100;
        int reply = RPLY_YES;
	if(strlen(msg)==0)
		reply = RPLY_NO;
	if(prob <= PROB)
		reply = RPLY_NO;
	send(sockfd, (char *)&reply, sizeof(int), 0);     //返信 
	


       log_ADD(COMMIT, "");
	
	return;
}

void real_abort(int sockfd)    //停止阶段 
{
        log_ADD(ABORT, "");
	return;
}

void rollback(int sockfd)    //回滚阶段 
{
	
	real_abort(sockfd);
	return;
}

void handle_requests(int sockfd)
{
	/* initialize random seed: */
	srand (time(NULL));
	char buffer[256];
	memset(buffer, 0, sizeof(buffer));
	uint8_t content_len;
	bool ifterminate = false;
	while(true)
	{
		int type;
		if(recv(sockfd, (char *)&type, sizeof(int), 0) == -1)   //接收请求的类型
		{
			perror("failed to recv data from coordinator");
			return;
		}
		printf("msg type%d\n", type);
		switch(type)
		{
			case ASK_COMMIT: answer_commit(sockfd);
							 break;
			case MSG_COMMIT: real_commit(sockfd);
							 break;
			case MSG_ABORT: real_abort(sockfd);
							break;
			case MSG_ROLLBACK:	rollback(sockfd);
								break;
			case MSG_COMPLETE:	real_abort(sockfd);
								break;
			case MSG_TERMINATE:	terminate(sockfd);
								ifterminate = true;
								break;
			default:	break;
		}
		if(ifterminate)
		{
			close(sockfd);
			break;
		}
	}
	return;
}

int participant(char * filename)
{
	
	int portno;
	int sockfd;
	struct sockaddr_in crd_addr;

	portno = ;                //解析
        char* IP_Address= ;       //解析
	sockfd = socket(AF_INET, SOCK_STREAM, 0); // 创建套接字
	if(sockfd < 0) 
		perror("ERROR opening socket");
	
	memset((char *) &crd_addr, 0, sizeof(crd_addr));
	crd_addr.sin_family = AF_INET;
	crd_addr.sin_addr.s_addr = inet_addr(IP_Address); //具体 IP 地址
	crd_addr.sin_port = htons(portno);
	int newsockfd = connect(sockfd,(struct sockaddr *) &crd_addr,sizeof(crd_addr));
	if(newsockfd<0)
	{
		perror("ERROR connecting");
		return 1;
	}

	handle_requests(sockfd);
	return 0;
}
