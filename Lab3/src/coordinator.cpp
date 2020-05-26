#include "coordinator.h"
#include "socklib.h"
#include "2pc.h" 



using namespace std;

bool system_failure = false;

timespec get_wall_time()      //获取特定时钟的时间
{
    struct timespec today_time;
	clock_gettime(CLOCK_REALTIME, &today_time);
	return today_time;
}

string char_to_str(char *str)      //字符数组转换为字符串 
{
	stringstream ss;
	string s;
	ss<<str;
	ss>>s;
	return s;
}

void print_res(void)   
{
	vector<float>::iterator it = commit_success.begin();
	for(; it != commit_success.end(); it++)
	{
		printf("%f\n", *it);
	}
	map<int, float>::iterator itt = timestamp_count.begin();
	for(; itt != timestamp_count.end(); itt++)
	{
		printf("%d %f\n", itt->first, itt->second);
	}
	return;
}

bool ask_vote(int newsockfd, int msg_type, char *operation)   //请求投票 
{
	char buffer[256];
	char reply_msg[256];
	bzero(buffer, sizeof(buffer));
	bzero(reply_msg, sizeof(reply_msg));
	int ret = send_msg(newsockfd, (char *)&msg_type, sizeof(int));   //发送某一请求信息给参与者
	if(ret == ERROR_SEND_FAIL)
	{
		perror("one sock pip broke down");
		return false;
	}
	else if(ret == ERROR_CLIENT_CLOSE)
	{
		perror("participant has closed");
		system_failure = true;
		return false;
	}
	int len = strlen(operation);
	ret = send_msg(newsockfd, (char *)&len, sizeof(int));  //发送操作的长度给参与者
	if(ret == ERROR_SEND_FAIL)
	{
		perror("one sock pip broke down");
		return false;
	}
	else if(ret == ERROR_CLIENT_CLOSE)
	{
		perror("participant has closed");
		system_failure = true;
		return false;
	}
	ret = send_msg(newsockfd, operation, strlen(operation)); //发送操作信息给参与者
	if(ret == ERROR_SEND_FAIL)
	{
		perror("one sock pip broke down");
		return false;
	}
	else if(ret == ERROR_CLIENT_CLOSE)
	{
		perror("participant has closed");
		return false;
	}
	ret = recv(newsockfd, &reply_msg, sizeof(int), 0);   //收到该参与者的回信
	if(ret == -1)	// recv timeout 
	{
		perror("recv timeout");
		return false;
	}
	else if(ret == 0)
	{
		perror("participant disconnected");
		system_failure = true;
		return false;
	}
	int response_type = *(int *) reply_msg;
//	printf("response %d\n", response_type);
	if(response_type == RPLY_YES)    //该参与者准备好提交
		return true;
	else return false;
}

bool loop_ask(int msg_type, char *operation)    //对所有参与者进行遍历请求 
{
	map<hostname, fd>::iterator it = participant_hostname_set.begin();
	for(; it != participant_hostname_set.end(); it++)
	{
		int participant_fd = it->second;
//		cout<<"ask "<<it->first<<" "<<it->second<<endl;
		if(ask_vote(participant_fd, msg_type, operation) ==false)
			return false;
	}
	return true;
}

void loop_abort()       // 对所有参与者发送终止信息  
{
	map<hostname, fd>::iterator it = participant_hostname_set.begin();
	for(; it != participant_hostname_set.end(); it++)
	{
		int participant_fd = it->second;
		int term = MSG_TERMINATE;
		send(participant_fd, (char *)&term, sizeof(int), 0); 
		close(participant_fd);
	}

}

bool loop_commit()      //对所有参与者发送提交信息 
{
	map<hostname, fd>::iterator it = participant_hostname_set.begin();
	int type = MSG_COMMIT;
	for(; it != participant_hostname_set.end(); it++)
	{
		if(system_failure)
			return false;
		int participant_fd = it->second;
		int type = MSG_COMMIT;
		int ret = send_msg(participant_fd, (char *)&type, sizeof(int));
		if(ret == ERROR_SEND_FAIL)
		{
			perror("one sock pip broke down");
			return false;
		}
		else if(ret == ERROR_participant_CLOSE)
		{
			perror("participant has closed");
			system_failure = true;
			return false;
		}
		ret = recv(participant_fd, (char *)&type, sizeof(int), 0);      
		if(ret == -1)	// recv timeout 
		{
			perror("recv timeout");
			return false;
		}
		else if(ret == 0)
		{
			perror("participant disconnected");
			system_failure = true;
			return false;
		}
		if(type == RPLY_NO)   //若收不到该参与者的提交返信  
			return false;
	}
	return true;
}

void loop(int msg_type)  //给所有参与者发送某一请求 
{
	map<hostname, fd>::iterator it = participant_hostname_set.begin();
	int type = msg_type;
	for(; it != participant_hostname_set.end(); it++)
	{
		int participant_fd = it->second;
		send_msg(participant_fd, (char *)&type, sizeof(int));
	}
	return;
}

void two_phase_commit(char *operation)   //参数为一个操作字符 
{
	
	int ask = 0;
	// phase one: loop through the cohorts set to see if all of them is available
	timespec start = get_wall_time();	//	start timer
	int item_count = 0;  //已完成事务的数量 
	int item_suc = 0;
	timespec time_count;
	time_count.tv_sec = 0;
	time_count.tv_nsec = 0;
	
		if(system_failure)
			break;
		int if_suc = false;
		timespec item_begin = get_wall_time();  //开始计时  
		if(loop_ask(ASK_COMMIT, operation))
		{
			//			puts("ask success, begin commit");
			if(!loop_commit())
			{
				puts("commit failed, begin roll back");
				loop(MSG_ROLLBACK);
			}
			else 
			{
				if_suc = true;
				//		puts("commit suc,");
				loop(MSG_COMPLETE); 
			}
		}
		else
		{
			puts("ask failed, begin abort");
			loop(MSG_ABORT);
		}
		timespec item_end = get_wall_time();      //结束计时 
		item_count++;    
		timespec time_elapse;
		if(if_suc)
		{
			time_elapse.tv_sec = item_end.tv_sec - item_begin.tv_sec;
			time_elapse.tv_nsec = item_end.tv_nsec - item_begin.tv_nsec;
			time_count.tv_sec += time_elapse.tv_sec;
			time_count.tv_nsec += time_elapse.tv_nsec;
			//			commit_success.push_back(time_elapse);
			item_suc++;
		}

	
	loop_abort();
	timespec end = get_wall_time();
	printf("sucess:%d all:%d\n", item_suc, item_count);
	long long time_used = (end.tv_sec - start.tv_sec) * 1000000000 + end.tv_nsec - start.tv_nsec;
	long long time_count_ll = time_count.tv_sec * 1000000000 + time_count.tv_nsec;
	printf("time used:%lfms latency: %lfms throughput: %f item per ms\n", time_used / 1000000.0, (time_count_ll / 1000000.0) / item_suc, item_suc / (time_used / 1000000.0));
	return;
}



int coordinator(char * filename)       //传递参数为配置文件
{
	
	int sockfd, option=1;
	
	int parnum = 0;   
        int num_par   //配置文件中参与者的数量
        
	

	sockfd = socket(AF_INET, SOCK_STREAM, 0);   // 创建套接字
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)(&option), sizeof(option));//设置选项值
	struct timeval timeout; 
	if (sockfd < 0) {
		perror("ERROR opening socket");
		return 1;
	}
	bzero((char *) &crd_addr, sizeof(crd_addr));// 给变量填充字符0
	crd_addr.sin_family = AF_INET;              // 表明使用 IPv4 地址（格式）
	crd_addr.sin_addr.s_addr = inet_addr(IP_Address);;       //具体 IP 地址
	crd_addr.sin_port = htons(portNumber);                   // 具体端口号

	socklen_t sockaddr_len = (socklen_t)sizeof(struct sockaddr);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sockaddr_len) < 0) // 绑定套接字、结构体变量
	{
		perror("ERROR on binding");
		return 1;
	}

	if(listen(sockfd, MAX_PARTS_NUM)==-1)       // 被动(睡眠)监听参与者发来请求
	{
		perror("ERROR on listening");
		return 1;
	}

	while(true)        //协调者与每个参与者创建通信
	{
		socklen_t parlen = sizeof(par_addr[parnum]);
		int newsockfd = accept(sockfd, (struct sockaddr *) &par_addr[parnum], &parlen);
		if (newsockfd < 0) 
		{
			perror("ERROR on accept");
			return 1;
		}
                
		int portNum=         //从配置文件中解析出来端口号
		
		parnum++;
		part_portnum_set[portNum] = newsockfd;
		client_fd_set[newsockfd] = portNum;
		if(parnum == num_par)
			break;
	}




	two_phase_commit(operation);//需要一个操作字符数组
	//	print_res();
	return 0;
}






