/*
 * @Author: your name
 * @Date: 2020-05-27 03:24:06
 * @LastEditTime: 2020-05-27 06:09:34
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \CloudComputingLabs\Lab3\src\serverhandler.cc
 */
#include "serverhandler.h"
#include "coordinator.cpp"
#include "config.cc"
Config coordinator_config;

int serverHandler(Config config)
{
    coordinator_config = config;

    char* IP_Addr = DEFAULT_IP_Address;
    int portNumber = atoi(DEFAULT_PORT_NUMBER);

    // 启动服务器
    int serv_sock = launchTcpServer(IP_Addr,portNumber);[]
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
    if(n<0) {ErrMsg(200);}
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
 */
void ErrMsg(int errCode)
{
    switch(errCode)
    {
        case 100:printf("Error <100> | Open file failed.\n");break;
        case 101:printf("Error <101> | Create Post_Show HTML Failed.\n");break;

        case 200:printf("Error <200> | Cannot Create a Socket.\n");break;
        case 201:printf("Error <201> | ReuseAddr Failed.\n");break;
        case 202:printf("Error <202> | Binding Port or IP Failed.\n");break;
        case 203:printf("Error <203> | Listening Port Failed.\n");break;
        case 204:printf("Error <204> | Accept Client Socket Failed.\n");break;

        case 300:printf("Error <300> | Fail to Receive character after \\r\n");break;
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
void response2Clnt(int socket, int type, char* BUFFER)
{
    if(type == SUCCESS)
    {
        SendMsg2Clnt(socket,"+OK\r\n");
    }
    else if(type == ERROR)
    {
        SendMsg2Clnt(socket,"-ERROR\r\n");
    }
    else if(type == RESP_ARRAY)
    {
        BUFFER = "*"+BUFFER+"\r\n";
        SendMsg2Clnt(socket,BUFFER);
    }
    else if(type >= 0)
    {
        _itoa(type, BUFFER, 10);
        BUFFER = ":"+BUFFER+"\r\n";
        SendMsg2Clnt(socket,BUFFER);
    }
}

/*
 * 读取/解析请求部分
 */
void* analyzeClientRequest(void* ptr)
{
    int type = -4;
    int clnt_sock = *(int*)ptr;
    char BUFFER[REV_BUF_SIZE+1] = {0};
    int contentLength = 0;

    // 将 http数据包的信息头读完(信息头和正文间以空行分隔)
    getOneLineFromSocket(clnt_sock,BUFFER,REV_BUF_SIZE));
    type = coordinator(BUFFER,coordinator_config);
    response2Clnt(clnt_sock, type, BUFFER);
    close(clnt_sock);
    return NULL;
}
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
                int readno = read(socket,&tmp,1);
                BUFFER[charaCount++] = ' ';
                break;
            }
        }else{
            BUFFER[charaCount++] = tmp;
        }
    }
    return charaCount;
}