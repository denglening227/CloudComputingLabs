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
#define PROB -1

using namespace std;


struct database_entry  //数据库 
{
	string st;
	int len;
}database[20];

struct log       //日志
{
	int index;
	string st;
}undo_log, redo_log;

void answer_commit(int sockfd)   //对提交请求的返信 
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
	int reply = RPLY_YES;
	if(strlen(msg)==0)
		reply = RPLY_NO;
	if(prob <= PROB)
		reply = RPLY_NO;
	send(sockfd, (char *)&reply, sizeof(int), 0);
	stringstream ss; 
	string op_type, op_line, op_content = "";      //将操作信息数组分开 
	ss<<msg;
	ss>>op_type;
	ss>>op_line;
	if(op_type.compare("INSERT") == 0)
		ss>>op_content;
	cout<<op_type<<" "<<op_line<<" "<<op_content<<endl;
	redo_log.index = atoi(op_line.c_str());   //准备提交阶段，先写入日志  
	redo_log.st = op_content;
	undo_log.index = redo_log.index;
	undo_log.st = database[redo_log.index].st;
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
	exit(EXIT_SUCCESS);
	return;
}

void real_commit(int sockfd)    //确定提交阶段 
{
	int index = redo_log.index;
	int prob  = rand() % 100;
	int reply = COMMIT_SUCCESS;
	if(prob <= PROB)
		reply = COMMIT_FAIL;
	else if(redo_log.index == -1)
		reply == COMMIT_FAIL;
	send(sockfd, (char *)&reply, sizeof(int), 0);     //返信 
	if(reply == COMMIT_FAIL)
		return;
	database[index].st = redo_log.st;     //事务已完成，清除 
	database[index].len = redo_log.st.size(); 
	redo_log.index = -1;
	redo_log.st = "";
	return;
}

void real_abort(int sockfd)    //停止阶段 
{

	redo_log.index = -1;
	redo_log.st = "";
	undo_log.index = -1;
	undo_log.st = "";
	return;
}

void rollback(int sockfd)    //回退阶段 
{
	int index = undo_log.index;
	database[index].st = undo_log.st;
	database[index].len = undo_log.st.size();
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
