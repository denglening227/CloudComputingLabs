#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <getopt.h>
#include <pthread.h>
#include <iostream>
using namespace std;

#define DEFAULT_IP_Address "127.0.0.1"
#define DEFAULT_PORT_NUMBER 8888
#define NOT_FOUND_FILEPATH "./src/404.html"
#define NOT_IMPLEMENTED_FILEPATH "./src/501.html"
#define INDEX_PAGE_FILEPATH "./src/index.html"

#define REV_BUF_SIZE 512
#define SEND_BUF_SIZE 512
#define MAX_CLIENT_COUNT 10

// 线程
#define DEFAULT_PTHREAD_NUM 2
pthread_t *responseThread;
// 浏览器的请求类型
enum RequestType{REQ_GET, REQ_POST, REQ_UNDEFINED};

int pthread_num; //线程数
int launchTcpServer(char* IP_Address, int portNumber);
void ErrMsg(int errCode);
void* analyzeClientRequest(void* ptr);
int Socket(int family, int type, int protocol);
void reuseAddr(int socket);
void sendStartLine(int socket, int returnNum);
void unimplemented(int socket);
void returnPost(int socket);
void pageNotFound(int socket);
void sendFile(int socket, int filefd);


void Write(int fd, void *ptr, size_t nbytes)
{
	if (write(fd, ptr, nbytes) != nbytes)
		printf("write error");
}


int main(int argc, char* argv[])
{
    // 判断参数
    static struct option long_options[] =
    {
        {"ip", required_argument, NULL, '1'},
        {"port", required_argument, NULL, '1'},
        {"number-thread", required_argument, NULL,'1'},
        {NULL,  0,   NULL, 0},
    };
    //判断是否缺少参数
	if(argc < 5)
	{
		printf(" ip and port are empty\n");
		return 1;
	}

	//获取输入的IP、端口和线程数
	char* IP_Addr = argv[2];
	int portNumber = atoi(argv[4]);
    if(argc==7)
		pthread_num = atoi(argv[6]);
	else
		pthread_num = DEFAULT_PTHREAD_NUM;

    // 启动服务器
    int serv_sock = launchTcpServer(IP_Addr,portNumber);

    // 线程池
    responseThread= (pthread_t *)calloc(pthread_num,sizeof(pthread_t));
    if(responseThread==NULL) printf("calloc error.\n");

    for(int i=0;i<pthread_num;i++)
    {
        // 接受客户端请求
        struct sockaddr_in clnt_addr;
        socklen_t clnt_addr_size = sizeof(clnt_addr);
        // Socket API: accpet() 函数返回一个套接字，并用新套接字来与客户端通信
        int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        if(clnt_sock == -1)
        {
            ErrMsg(204);
            exit(1);
        }
        printf("%s: %d linked!\n", inet_ntoa(clnt_addr.sin_addr), clnt_addr.sin_port);

        // 创建线程处理浏览器请求
        int threadReturn = pthread_create(&responseThread[i], NULL, &analyzeClientRequest, &clnt_sock);
        // 若pthread_create返回值！=0，表示发生错误
        if(threadReturn)
        {
            ErrMsg(600);
            printf(" Error is %d.\n",threadReturn);
            exit(1);
        }

    }

    return 0;
}

/*
 * 服务器套接字以及调试部分
 */
// 开启服务器套接字
int launchTcpServer(char* IP_Address, int portNumber)
{
    // 创建套接字
    int serv_sock = Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // 将套接字与IP与端口号相连接
    struct sockaddr_in serv_addr;              // 创建sockaddr_in结构体变量
    memset(&serv_addr, 0, sizeof(serv_addr)); // 给变量填充字符0
    serv_addr.sin_family = AF_INET;          // 表明使用 IPv4 地址（格式）
    serv_addr.sin_addr.s_addr = inet_addr(IP_Address); //具体 IP 地址
    serv_addr.sin_port = htons(portNumber); // 具体端口号
    reuseAddr(serv_sock);

    // Socket API: bind() 绑定套接字、结构体变量
    if( bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
    {
        ErrMsg(202);
        exit(1);
    }
    
    // Socket API: listen() 被动(睡眠)监听客户端发来请求
    if( listen(serv_sock, MAX_CLIENT_COUNT) == -1)
    {
        ErrMsg(203);
        exit(1);
    }

    return serv_sock;
}
// 保证套接字正确创建
int Socket(int family, int type, int protocol)
{
    int n = socket(family,type,protocol);
    if(n<0) {ErrMsg(100);}
    return n;
}
// 解决端口被占用问题
void reuseAddr(int socket)
{
    int on = 1;
    int ret = setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if(ret == -1)
    {
        ErrMsg(201);
        exit(1);
    }
}
/* 输出错误信息以便调试 / 分类
 * I类 --- 读取文件相关
 * II类 -- 套接字相关
 * III类 - 读取网络内容问题
 * IV类 -- 页面存在问题
 * V类 --- 功能未实现
 */
void ErrMsg(int errCode)
{
    switch(errCode)
    {
        case 100:printf("Error <100> | Open file failed.\n");

        case 200:printf("Error <200> | Cannot Create a Socket.\n");break;
        case 201:printf("Error <201> | ReuseAddr Failed.\n");break;
        case 202:printf("Error <202> | Binding Port or IP Failed.\n");break;
        case 203:printf("Error <203> | Listening Port Failed.\n");break;
        case 204:printf("Error <204> | Accept Client Socket Failed.\n");break;

        case 300:printf("Error <300> | Fail to Receive character after \\r\n");break;

        case 401:printf("Error <401> | ?\n");break;
        case 404:printf("Error <404> | Page Not Found.\n");break;

        case 501:printf("Error <501> | Method Not Implemented.\n");break;

        case 600:printf("Error <600> | Fail to Create Thread.");break;
        default:break;
    }
}




/*
 * 回应请求部分
 */
ssize_t SendMsg2Clnt(int socket, const char* msg)
{
    return write(socket, msg, strlen(msg));
}
void response2Clnt(int socket,const char* filepath, enum RequestType requestType)
{
    if(requestType == REQ_UNDEFINED)
    {
        sendStartLine(socket,501);
        unimplemented(socket);
    }
    else if(requestType == REQ_POST)
    {
        sendStartLine(socket,200);
        returnPost(socket);
    }
    else if(requestType == REQ_GET)
    {
        // 未指定访问文件，则自动转接到 Index页面
        if(strcmp(filepath,"./") == 0)
        {
            filepath = "./index.html";
        }
        int filefd = open(filepath, O_RDONLY);
        if(filefd == -1)
        {
            ErrMsg(100);
            sendStartLine(socket,404);
            pageNotFound(socket);
        }
        else
        {
            sendStartLine(socket,200);
            sendFile(socket,filefd);
        }
        close(filefd);
    }
}

void unimplemented(int socket)
{
    ErrMsg(501);
    int filefd = open(NOT_IMPLEMENTED_FILEPATH, O_RDONLY);
    sendFile(socket,filefd);
    close(filefd);
}
void pageNotFound(int socket)
{
    ErrMsg(404);
    int filefd = open(NOT_FOUND_FILEPATH, O_RDONLY);
    sendFile(socket,filefd);
    close(filefd);
}
void sendStartLine(int socket, int returnNum)
{
    char BUFFER[SEND_BUF_SIZE] = {0};
    switch(returnNum)
    {
        case 200:sprintf(BUFFER,"HTTP/1.1 200 OK\r\n");break;
        case 404:sprintf(BUFFER,"HTTP/1.1 404 NOT FOUND\r\n");break;
        case 501:sprintf(BUFFER,"HTTP/1.1 501 Method Not Implemented\r\n");break;
        default:sprintf(BUFFER,"HTTP/1.1 %d Undefined Return Number\r\n",returnNum);break;
    }
    SendMsg2Clnt(socket,BUFFER);
}
void sendFile(int socket, int filefd)
{
    char BUFFER[SEND_BUF_SIZE];
    int readBytes = read(filefd, BUFFER, SEND_BUF_SIZE);
    while(readBytes > 0)
    {
        Write(socket, BUFFER, readBytes);
        readBytes = read(filefd, BUFFER, SEND_BUF_SIZE);
    }
}
void returnPost(int socket)
{
    
}

/*
 * 读取/解析请求部分
 */
/* 从套接字中读取一行到BUFFER中，
 * 返回读取到的一行内容之字符数
 */
int getOneLineFromSocket(int socket, char* BUFFER, int BUF_SIZE)
{
    memset(BUFFER, 0, BUF_SIZE);
    int charaCount = 0;
    char tmp;

    while( read(socket,&tmp,1) && charaCount<BUF_SIZE )
    {
        // 字符为'\r'
        if(tmp == '\r')
        {
            // 之后接收不到内容
            if(recv(socket,&tmp,1,MSG_PEEK) == -1)
            {
                ErrMsg(300);
                exit(1);
            }
            // 之后接收到字符'\n',表示一行结束
            if(tmp == '\n' && charaCount<BUF_SIZE)
            {
                read(socket,&tmp,1);
                BUFFER[charaCount++] = '\n';
                break;
            }
            BUFFER[charaCount++] = '\r';
        }else{
            BUFFER[charaCount++] = tmp;
        }
    }
    return charaCount;
}
// 接收浏览器端的数据,并返回一个http包
void* analyzeClientRequest(void* ptr)
{
    int clnt_sock = *(int*)ptr;
    char c;
    char BUFFER[REV_BUF_SIZE+1] = {0};
    int contentLength = 0;
    enum RequestType requestType = REQ_UNDEFINED;

    // 定义存放请求文件名的内存块
    #define FILE_PATH_LENGTH 128
    char requestFilePath[FILE_PATH_LENGTH] = {0};

    // 定义存放查询参数字符串的内存库
    #define QUERY_STRING_LENGTH 128
    char requestQueryString[QUERY_STRING_LENGTH] = {0};

    // 将 http数据包的信息头读完(信息头和正文间以空行分隔)
    while(getOneLineFromSocket(clnt_sock,BUFFER,REV_BUF_SIZE))
    {
        printf("%s",BUFFER);
        if(strcmp(BUFFER, "\n")==0) break;
        if(requestType == REQ_UNDEFINED)
        {
            int pFileName = 0;
            int pQueryString = 0;
            int pRecvBuf = 0;
            if(strncmp(BUFFER,"GET",3) == 0){
                // GET 请求
                requestType = REQ_GET;
                pRecvBuf = 4;
            }else if(strncmp(BUFFER,"POST",4) == 0){
                // POST 请求
                requestType = REQ_POST;
                pRecvBuf = 5;
            }
            // [START]此处开始“未思考逻辑”
            // 获取请求的文件路径 查询参数
            if(pRecvBuf){
                requestFilePath[pFileName++] = '.';
                while(pFileName<FILE_PATH_LENGTH && BUFFER[pRecvBuf]
                    && BUFFER[pRecvBuf]!=' ' && BUFFER[pRecvBuf]!='?'){
                    requestFilePath[pFileName++] = BUFFER[pRecvBuf++];
                }
                if(pFileName<FILE_PATH_LENGTH && BUFFER[pRecvBuf]=='?'){
                    ++pRecvBuf;
                    while(pQueryString<QUERY_STRING_LENGTH &&
                        BUFFER[pRecvBuf] && BUFFER[pRecvBuf]!=' '){
                        requestQueryString[pQueryString++] = BUFFER[pRecvBuf++];
                    }
                }
            }   // [END]此处结束“未思考逻辑”
        }else if(requestType==REQ_GET){
        }else if(requestType==REQ_POST){
            if(strncmp(BUFFER, "Content-Length:", 48)==0){
                contentLength = atoi(BUFFER+48);
            }
        }
    }

    //如果是REQ_GET或REQ_UNDEFINED类型，不再读取http正文的内容
    //如果是REQ_POST类型,读取contentLength长度的数据
    if(requestType==REQ_POST && contentLength){
        if(contentLength > QUERY_STRING_LENGTH){
            fprintf(stderr, "Query string buffer is smaller than content length\n");
            contentLength = QUERY_STRING_LENGTH;
        }
        read(clnt_sock, requestQueryString, contentLength);
    }

    switch(requestType){
        case REQ_GET:
            response2Clnt(clnt_sock, requestFilePath, REQ_GET);break;
        case REQ_POST:
            {
                if(contentLength==0){
                    response2Clnt(clnt_sock, NOT_FOUND_FILEPATH, REQ_POST);
                }
            }break;
        case REQ_UNDEFINED:response2Clnt(clnt_sock, NOT_IMPLEMENTED_FILEPATH, REQ_UNDEFINED);break;
        default:break;
    }
    close(clnt_sock);
    return NULL;
}
