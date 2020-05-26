/*
 * @Author: your name
 * @Date: 2020-05-27 03:22:59
 * @LastEditTime: 2020-05-27 06:16:39
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \CloudComputingLabs\Lab3\src\serverhandler.h
 */ 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <winsock.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
using namespace std;

#define DEFAULT_IP_Address "127.0.0.1"
#define DEFAULT_PORT_NUMBER 8888

// 线程
#define DEFAULT_PTHREAD_NUM 2
pthread_t *responseThread;

#define REV_BUF_SIZE 1024
#define SEND_BUF_SIZE 1024

enum RESPONSE_TYPE {
    SUCCESS = -3,
    ERROR = -2,
    RESP_ARRAY = -1,
    INTEGER = 0,
}


int pthread_num; //线程数
int launchTcpServer(char* IP_Address, int portNumber);
void ErrMsg(int errCode);
int Socket(int family, int type, int protocol);
void reuseAddr(int socket);
void* analyzeClientRequest(void* ptr)
int getOneLineFromSocket(int socket, char* BUFFER, int BUF_SIZE)