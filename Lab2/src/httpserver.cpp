/*
 * @Author: your name
 * @Date: 2020-04-27 17:37:01
 * @LastEditTime: 2020-04-27 18:12:10
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: home/denglening/Github/CloudComputingLabs/Lab2/src/httpserver.cpp
 */
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
#include <pthread.h>


#define IP_Address "127.0.0.1"
#define portNumber 1234
#define THREAD_NUM 2

#define BUF_SIZE 100
#define FILE_NAME "test"

pthread_t *responseThread;

// 浏览器的请求类型
enum RequestType{REQUEST_GET, REQUEST_POST, REQUEST_UNDEFINED};

//线程: 接收浏览器端的数据,并返回一个http包

void* responseBrowserRequest(void* ptr){

    int browserSocket = *(int*)ptr;

    char c;

    char recvBuf[RECV_BUF_SIZE+1] = {0};

    int contentLength = 0;

    enum RequestType requestType = REQUEST_UNDEFINED;

    

    // 定义存放请求文件名的内存块

    #define FILE_PATH_LENGTH 128

    char requestFilePath[FILE_PATH_LENGTH] = {0};



    // 定义存放查询参数字符串的内存库

    #define QUERY_STRING_LENGTH 128

    char requestQueryString[QUERY_STRING_LENGTH] = {0};



    // 请求的文件是否为可执行文件(需要动态生成页面)

    int isXFile = 0;



    // 将 http数据包的信息头读完(信息头和正文间以空行分隔)

    while(getOneLineFromSocket(browserSocket,recvBuf,RECV_BUF_SIZE)){

        // printf("%s",recvBuf);

        if(strcmp(recvBuf, "\n")==0){

            break;

        }



        if(requestType==REQUEST_UNDEFINED){

            int pFileName = 0;

            int pQueryString = 0;

            int pRecvBuf = 0;



            if(strncmp(recvBuf,"GET",3)==0){

                // GET 请求

                requestType = REQUEST_GET;

                pRecvBuf = 4;

                

            }else if(strncmp(recvBuf,"POST",4)==0){

                // POST 请求

                requestType = REQUEST_POST;

                pRecvBuf = 5;

            }



            // 获取请求的文件路径 查询参数

            if(pRecvBuf){

                requestFilePath[pFileName++] = '.';

                while(pFileName<FILE_PATH_LENGTH && recvBuf[pRecvBuf]

                    && recvBuf[pRecvBuf]!=' ' && recvBuf[pRecvBuf]!='?'){

                    requestFilePath[pFileName++] = recvBuf[pRecvBuf++];

                }



                if(pFileName<FILE_PATH_LENGTH && recvBuf[pRecvBuf]=='?'){

                    ++pRecvBuf;

                    while(pQueryString<QUERY_STRING_LENGTH && 

                        recvBuf[pRecvBuf] && recvBuf[pRecvBuf]!=' '){

                        requestQueryString[pQueryString++] = recvBuf[pRecvBuf++];

                    }

                }

            }

        }else if(requestType==REQUEST_GET){



        }else if(requestType==REQUEST_POST){

            if(strncmp(recvBuf, "Content-Length:", 15)==0){

                contentLength = atoi(recvBuf+15);

            }

        }

    }



    //如果是REQUEST_GET或REQUEST_UNDEFINED类型，不再读取http正文的内容

    //如果是REQUEST_POST类型,在读取contentLength长度的数据

    if(requestType==REQUEST_POST && contentLength){

        if(contentLength > QUERY_STRING_LENGTH){

            fprintf(stderr, "Query string buffer is smaller than content length\n");

            contentLength = QUERY_STRING_LENGTH;

        }

        read(browserSocket, requestQueryString, contentLength);

    }

    

    // 判断请求的文件是否是文件夹

    struct stat fileInfo;

    stat(requestFilePath,&fileInfo);

    if(S_ISDIR(fileInfo.st_mode)){

        //是文件夹的情况

    }else{

        //非文件夹的情况

        // 判断请求的文件是否是可执行文件

        if(access(requestFilePath,X_OK)==0){

            isXFile = 1;

        }

    }



    switch(requestType){

        case REQUEST_GET:

            if(isXFile==0){

                responseStaticFile(browserSocket,200,requestFilePath,NULL);

            }else{

                execCGI(browserSocket,requestFilePath,requestQueryString);

            }

            break;

        case REQUEST_POST:

            {

                if(contentLength==0){

                    responseStaticFile(browserSocket,400,"./err400.html","text/html");

                    break;

                }

                execCGI(browserSocket,requestFilePath,requestQueryString);

            }

            break;

        case REQUEST_UNDEFINED:

            {

                responseStaticFile(browserSocket, 501, "./err501.html","text/html");

            }

            break;

        default:

            break;

    }

    close(browserSocket);
    return NULL;
}






int main()
{

    nthreads=THREAD_NUM;

    // Check the file out
    char* filename = FILE_NAME;
    FILE *testFile = fopen(filename, "r");
    if (testFile == NULL) 
    {
        printf("Error <100> | Open file failed. Run this program one more time.");
        exit(0);
    }

    // Create socket
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    // Tie socket up with IP & Port
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr)); // Fill up with '0'
    serv_addr.sin_family = AF_INET;    // Use IPv4 address
    serv_addr.sin_addr.s_addr = inet_addr(IP_Address); // IP address
    serv_addr.sin_port = htons(portNumber); //Port
    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    // Listening & Waiting User send something
    listen(serv_sock, 20);


    //Array of threads
     responseThread= (pthread_t *)calloc(nthreads,sizeof(pthread_t));
     if(responseThread==NULL)err_sys("calloc error");



    // Send data till it ends
    char buffer[BUF_SIZE] = {0};
    int nCount;
    while((nCount = fread(buffer, 1, BUF_SIZE, testFile))>0)
    {
        // Send Data to Client
        send(clnt_sock, buffer, nCount, 0);
    }
    
   //deal request
   for(int i=0;i<nthreads;i++)
   {

        struct sockaddr_in browserSocketAddr;

        int browserLen = sizeof(browserSocketAddr);

        int browserSocket = accept(serv_sock,(struct sockaddr*)&browserSocketAddr,&browserLen);

        if(browserSocket==-1){

            fprintf(stderr,"Error: fail to accept, error is %d\n",errno);

            exit(1);

        }

        printf("%s:%d linked !\n",inet_ntoa(browserSocketAddr.sin_addr),

            browserSocketAddr.sin_port);

  

        int threadReturn = pthread_create(&responseThread[i],

            NULL,responseBrowserRequest,&browserSocket);

        // 如果pthread_create返回不为0,表示发生错误

        if(threadReturn){

            fprintf(stderr,"Error: fail to create thread, error is %d\n",threadReturn);

            exit(1);

        }

    }



    // Close socket
    fclose(testFile);
    close(serv_sock);
    close(clnt_sock);

    return 0;
}
